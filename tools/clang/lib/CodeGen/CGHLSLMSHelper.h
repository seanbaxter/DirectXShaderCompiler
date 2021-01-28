

#pragma once

#include "clang/Basic/SourceLocation.h"

#include "llvm37/ADT/StringMap.h"
#include "llvm37/ADT/MapVector.h"

#include "dxc/DXIL/DxilCBuffer.h"

#include <memory>
#include <vector>

namespace clang {
class HLSLPatchConstantFuncAttr;
namespace CodeGen {
class CodeGenModule;
}
}

namespace llvm37 {
class Function;
class Module;
class Value;
class DebugLoc;
class Constant;
class GlobalVariable;
class CallInst;
class Instruction;
template <typename T, unsigned N> class SmallVector;
}

namespace hlsl {
class HLModule;
struct DxilResourceProperties;
struct DxilFunctionProps;
class DxilFieldAnnotation;
enum class IntrinsicOp;
namespace dxilutil {
class ExportMap;
}
}

namespace CGHLSLMSHelper {

struct EntryFunctionInfo {
  clang::SourceLocation SL = clang::SourceLocation();
  llvm37::Function *Func = nullptr;
};

  // Map to save patch constant functions
struct PatchConstantInfo {
  clang::SourceLocation SL = clang::SourceLocation();
  llvm37::Function *Func = nullptr;
  std::uint32_t NumOverloads = 0;
};

/// Use this class to represent HLSL cbuffer in high-level DXIL.
class HLCBuffer : public hlsl::DxilCBuffer {
public:
  HLCBuffer(bool bIsView, bool bIsTBuf)
      : bIsView(bIsView), bIsTBuf(bIsTBuf), bIsArray(false), ResultTy(nullptr) {}
  virtual ~HLCBuffer() = default;

  void AddConst(std::unique_ptr<DxilResourceBase> &pItem) {
    pItem->SetID(constants.size());
    constants.push_back(std::move(pItem));
  }

  std::vector<std::unique_ptr<DxilResourceBase>> &GetConstants() {
    return constants;
  }

  bool IsView() { return bIsView; }
  bool IsTBuf() { return bIsTBuf; }
  bool IsArray() { return bIsArray; }
  void SetIsArray() { bIsArray = true; }
  llvm37::Type *GetResultType() { return ResultTy; }
  void SetResultType(llvm37::Type *Ty) { ResultTy = Ty; }

private:
  std::vector<std::unique_ptr<DxilResourceBase>>
      constants; // constants inside const buffer
  bool bIsView;
  bool bIsTBuf;
  bool bIsArray;
  llvm37::Type *ResultTy;
};
// Scope to help transform multiple returns.
struct Scope {
 enum class ScopeKind {
   IfScope,
   SwitchScope,
   LoopScope,
   ReturnScope,
   FunctionScope,
 };
 ScopeKind kind;
 llvm37::BasicBlock *EndScopeBB;
 // Save loopContinueBB to create dxBreak.
 llvm37::BasicBlock *loopContinueBB;
 // For case like
 // if () {
 //   ...
 //   return;
 // } else {
 //   ...
 //   return;
 // }
 //
 // both path is returned.
 // When whole scope is returned, go to parent scope directly.
 // Anything after it is unreachable.
 bool bWholeScopeReturned;
 unsigned parentScopeIndex;
};

class ScopeInfo {
public:
  ScopeInfo(){}
  ScopeInfo(llvm37::Function *F);
  void AddIf(llvm37::BasicBlock *endIfBB);
  void AddSwitch(llvm37::BasicBlock *endSwitchBB);
  void AddLoop(llvm37::BasicBlock *loopContinue, llvm37::BasicBlock *endLoopBB);
  void AddRet(llvm37::BasicBlock *bbWithRet);
  void EndScope(bool bScopeFinishedWithRet);
  Scope &GetScope(unsigned i);
  const llvm37::SmallVector<unsigned, 2> &GetRetScopes() { return rets; }
  void LegalizeWholeReturnedScope();
  llvm37::SmallVector<Scope, 16> &GetScopes() { return scopes; }
  bool CanSkipStructurize();

private:
  void AddScope(Scope::ScopeKind k, llvm37::BasicBlock *endScopeBB);
  llvm37::SmallVector<unsigned, 2> rets;
  unsigned maxRetLevel;
  bool bAllReturnsInIf;
  llvm37::SmallVector<unsigned, 8> scopeStack;
  // save all scopes.
  llvm37::SmallVector<Scope, 16> scopes;
};

// Map from value to resource properties.
// This only collect object variables(global/local/parameter), not object fields inside struct.
// Object fields inside struct is saved by TypeAnnotation.
struct DxilObjectProperties {
  bool AddResource(llvm37::Value *V, const hlsl::DxilResourceProperties &RP);
  bool IsResource(llvm37::Value *V);
  hlsl::DxilResourceProperties GetResource(llvm37::Value *V);

  // MapVector for deterministic iteration order.
  llvm37::MapVector<llvm37::Value *, hlsl::DxilResourceProperties> resMap;
};

// Align cbuffer offset in legacy mode (16 bytes per row).
unsigned AlignBufferOffsetInLegacy(unsigned offset, unsigned size,
                                   unsigned scalarSizeInBytes,
                                   bool bNeedNewRow);

void FinishEntries(hlsl::HLModule &HLM, const EntryFunctionInfo &Entry,
                   clang::CodeGen::CodeGenModule &CGM,
                   llvm37::StringMap<EntryFunctionInfo> &entryFunctionMap,
                   std::unordered_map<llvm37::Function *,
                                      const clang::HLSLPatchConstantFuncAttr *>
                       &HSEntryPatchConstantFuncAttr,
                   llvm37::StringMap<PatchConstantInfo> &patchConstantFunctionMap,
                   std::unordered_map<llvm37::Function *,
                                      std::unique_ptr<hlsl::DxilFunctionProps>>
                       &patchConstantFunctionPropsMap);

void FinishIntrinsics(
    hlsl::HLModule &HLM,
    std::vector<std::pair<llvm37::Function *, unsigned>> &intrinsicMap,
    DxilObjectProperties &valToResPropertiesMap);

void AddDxBreak(llvm37::Module &M, const llvm37::SmallVector<llvm37::BranchInst*, 16> &DxBreaks);

void ReplaceConstStaticGlobals(
    std::unordered_map<llvm37::GlobalVariable *, std::vector<llvm37::Constant *>>
        &staticConstGlobalInitListMap,
    std::unordered_map<llvm37::GlobalVariable *, llvm37::Function *> &staticConstGlobalCtorMap);

void FinishClipPlane(hlsl::HLModule &HLM, std::vector<llvm37::Function *> &clipPlaneFuncList,
                    std::unordered_map<llvm37::Value *, llvm37::DebugLoc> &debugInfoMap,
                    clang::CodeGen::CodeGenModule &CGM);

void AddRegBindingsForResourceInConstantBuffer(
    hlsl::HLModule &HLM,
    llvm37::DenseMap<llvm37::Constant *,
                   llvm37::SmallVector<std::pair<hlsl::DXIL::ResourceClass, unsigned>,
                                     1>> &constantRegBindingMap);

void FinishCBuffer(
    hlsl::HLModule &HLM, llvm37::Type *CBufferType,
    std::unordered_map<llvm37::Constant *, hlsl::DxilFieldAnnotation>
        &AnnotationMap);

void ProcessCtorFunctions(llvm37::Module &M,
                          llvm37::SmallVector<llvm37::Function *, 2> &Ctors,
                          llvm37::Function *Entry,
                          llvm37::Function *PatchConstantFn);

void CollectCtorFunctions(llvm37::Module &M, llvm37::StringRef globalName,
                          llvm37::SmallVector<llvm37::Function *, 2> &Ctors,
                          clang::CodeGen::CodeGenModule &CGM);

void TranslateRayQueryConstructor(hlsl::HLModule &HLM);

void UpdateLinkage(
    hlsl::HLModule &HLM, clang::CodeGen::CodeGenModule &CGM,
    hlsl::dxilutil::ExportMap &exportMap,
    llvm37::StringMap<EntryFunctionInfo> &entryFunctionMap,
    llvm37::StringMap<PatchConstantInfo> &patchConstantFunctionMap);

void StructurizeMultiRet(llvm37::Module &M,
                         clang::CodeGen::CodeGenModule &CGM,
                         llvm37::DenseMap<llvm37::Function *, ScopeInfo> &ScopeMap,
                         bool bWaveEnabledStage,
                         llvm37::SmallVector<llvm37::BranchInst *, 16> &DxBreaks);

llvm37::Value *TryEvalIntrinsic(llvm37::CallInst *CI, hlsl::IntrinsicOp intriOp, unsigned hlslVersion);
void SimpleTransformForHLDXIR(llvm37::Module *pM);
void ExtensionCodeGen(hlsl::HLModule &HLM, clang::CodeGen::CodeGenModule &CGM);
} // namespace CGHLSLMSHelper
