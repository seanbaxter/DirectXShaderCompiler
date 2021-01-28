///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilUtil.h                                                                //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// DXIL helper functions.                                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include <unordered_set>
#include <string>
#include <memory>
#include "llvm37/ADT/StringRef.h"
#include "llvm37/ADT/Twine.h"
#include "llvm37/IR/Constants.h"
#include "dxc/DXIL/DxilConstants.h"
#include "dxc/DXIL/DxilResourceProperties.h"

namespace llvm37 {
class Type;
class GlobalVariable;
class Function;
class Module;
class MemoryBuffer;
class LLVM37Context;
class DiagnosticInfo;
class Value;
class Instruction;
class CallInst;
class BasicBlock;
class raw_ostream;
class ModulePass;
class PassRegistry;
class DebugInfoFinder;
class DebugLoc;
class DIGlobalVariable;
class ConstantInt;
class SwitchInst;

ModulePass *createDxilLoadMetadataPass();
void initializeDxilLoadMetadataPass(llvm37::PassRegistry&);
}

namespace hlsl {

class DxilFieldAnnotation;
class DxilModule;
class DxilTypeSystem;
class OP;

namespace dxilutil {
  extern const char ManglingPrefix[];
  extern const char EntryPrefix[];
  extern const char *kResourceMapErrorMsg;

  unsigned
  GetLegacyCBufferFieldElementSize(DxilFieldAnnotation &fieldAnnotation,
                                   llvm37::Type *Ty, DxilTypeSystem &typeSys);
  llvm37::Type *GetArrayEltTy(llvm37::Type *Ty);
  bool HasDynamicIndexing(llvm37::Value *V);

  // Find alloca insertion point, given instruction
  llvm37::Instruction *FindAllocaInsertionPt(llvm37::Instruction* I); // Considers entire parent function
  llvm37::Instruction *FindAllocaInsertionPt(llvm37::BasicBlock* BB); // Only considers provided block
  llvm37::Instruction *FindAllocaInsertionPt(llvm37::Function* F);
  llvm37::Instruction *SkipAllocas(llvm37::Instruction *I);
  // Get first non-alloca insertion point, to avoid inserting non-allocas before alloca
  llvm37::Instruction *FirstNonAllocaInsertionPt(llvm37::Instruction* I); // Considers entire parent function
  llvm37::Instruction *FirstNonAllocaInsertionPt(llvm37::BasicBlock* BB); // Only considers provided block
  llvm37::Instruction *FirstNonAllocaInsertionPt(llvm37::Function* F);

  bool IsStaticGlobal(llvm37::GlobalVariable *GV);
  bool IsSharedMemoryGlobal(llvm37::GlobalVariable *GV);
  bool RemoveUnusedFunctions(llvm37::Module &M, llvm37::Function *EntryFunc,
                             llvm37::Function *PatchConstantFunc, bool IsLib);

  llvm37::DIGlobalVariable *FindGlobalVariableDebugInfo(llvm37::GlobalVariable *GV,
                                                llvm37::DebugInfoFinder &DbgInfoFinder);

  void EmitErrorOnInstruction(llvm37::Instruction *I, llvm37::Twine Msg);
  void EmitWarningOnInstruction(llvm37::Instruction *I, llvm37::Twine Msg);
  void EmitErrorOnFunction(llvm37::Function *F, llvm37::Twine Msg);
  void EmitWarningOnFunction(llvm37::Function *F, llvm37::Twine Msg);
  void EmitErrorOnGlobalVariable(llvm37::GlobalVariable *GV, llvm37::Twine Msg);
  void EmitWarningOnGlobalVariable(llvm37::GlobalVariable *GV, llvm37::Twine Msg);
  void EmitErrorOnContext(llvm37::LLVM37Context &Ctx, llvm37::Twine Msg);
  void EmitWarningOnContext(llvm37::LLVM37Context &Ctx, llvm37::Twine Msg);
  void EmitNoteOnContext(llvm37::LLVM37Context &Ctx, llvm37::Twine Msg);

  void EmitResMappingError(llvm37::Instruction *Res);
  // Simple demangle just support case "\01?name@" pattern.
  llvm37::StringRef DemangleFunctionName(llvm37::StringRef name);
  // ReplaceFunctionName replaces the undecorated portion of originalName with undecorated newName
  std::string ReplaceFunctionName(llvm37::StringRef originalName, llvm37::StringRef newName);
  void PrintEscapedString(llvm37::StringRef Name, llvm37::raw_ostream &Out);
  void PrintUnescapedString(llvm37::StringRef Name, llvm37::raw_ostream &Out);
  // Change select/phi on operands into select/phi on operation.
  // phi0 = phi a0, b0, c0
  // phi1 = phi a1, b1, c1
  // Inst = Add(phi0, phi1);
  // into
  // A = Add(a0, a1);
  // B = Add(b0, b1);
  // C = Add(c0, c1);
  // NewInst = phi A, B, C
  // Only support 1 operand now, other oerands should be Constant.
  llvm37::Value * SelectOnOperation(llvm37::Instruction *Inst, unsigned operandIdx);
  // Collect all select operand used by Inst.
  void CollectSelect(llvm37::Instruction *Inst,
                   std::unordered_set<llvm37::Instruction *> &selectSet);
  // If all operands are the same for a select inst, replace it with the operand.
  // Returns replacement value if successful
  llvm37::Value *MergeSelectOnSameValue(llvm37::Instruction *SelInst,
                                      unsigned startOpIdx,
                                      unsigned numOperands);
  bool SimplifyTrivialPHIs(llvm37::BasicBlock *BB);
  llvm37::BasicBlock *GetSwitchSuccessorForCond(llvm37::SwitchInst *Switch, llvm37::ConstantInt *Cond);
  void MigrateDebugValue(llvm37::Value *Old, llvm37::Value *New);
  void TryScatterDebugValueToVectorElements(llvm37::Value *Val);
  std::unique_ptr<llvm37::Module> LoadModuleFromBitcode(llvm37::StringRef BC,
    llvm37::LLVM37Context &Ctx, std::string &DiagStr);
  std::unique_ptr<llvm37::Module> LoadModuleFromBitcode(llvm37::MemoryBuffer *MB,
    llvm37::LLVM37Context &Ctx, std::string &DiagStr);
  std::unique_ptr<llvm37::Module> LoadModuleFromBitcodeLazy(std::unique_ptr<llvm37::MemoryBuffer> &&MB,
    llvm37::LLVM37Context &Ctx, std::string &DiagStr);
  void PrintDiagnosticHandler(const llvm37::DiagnosticInfo &DI, void *Context);
  bool IsIntegerOrFloatingPointType(llvm37::Type *Ty);
  // Returns true if type contains HLSL Object type (resource)
  bool ContainsHLSLObjectType(llvm37::Type *Ty);
  std::pair<bool, DxilResourceProperties> GetHLSLResourceProperties(llvm37::Type *Ty);
  bool IsHLSLResourceType(llvm37::Type *Ty);
  bool IsHLSLObjectType(llvm37::Type *Ty);
  bool IsHLSLRayQueryType(llvm37::Type *Ty);
  bool IsHLSLResourceDescType(llvm37::Type *Ty);
  bool IsResourceSingleComponent(llvm37::Type *Ty);
  uint8_t GetResourceComponentCount(llvm37::Type *Ty);
  bool IsSplat(llvm37::ConstantDataVector *cdv);

  llvm37::Type* StripArrayTypes(llvm37::Type *Ty, llvm37::SmallVectorImpl<unsigned> *OuterToInnerLengths = nullptr);
  llvm37::Type* WrapInArrayTypes(llvm37::Type *Ty, llvm37::ArrayRef<unsigned> OuterToInnerLengths);

  llvm37::CallInst *TranslateCallRawBufferLoadToBufferLoad(
    llvm37::CallInst *CI, llvm37::Function *newFunction, hlsl::OP *op);
  void ReplaceRawBufferLoadWithBufferLoad(llvm37::Function *F, hlsl::OP *op);

  llvm37::CallInst *TranslateCallRawBufferStoreToBufferStore(
    llvm37::CallInst *CI, llvm37::Function *newFunction, hlsl::OP *op);
  void ReplaceRawBufferStoreWithBufferStore(llvm37::Function *F, hlsl::OP *op);

  void ReplaceRawBufferLoad64Bit(llvm37::Function *F, llvm37::Type *EltTy, hlsl::OP *hlslOP);
  void ReplaceRawBufferStore64Bit(llvm37::Function *F, llvm37::Type *ETy, hlsl::OP *hlslOP);
}

}
