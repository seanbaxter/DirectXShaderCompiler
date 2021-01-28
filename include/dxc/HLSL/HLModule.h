///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// HLModule.h                                                                //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// HighLevel DX IR module.                                                   //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "dxc/Support/Global.h"
#include "dxc/DXIL/DxilMetadataHelper.h"
#include "dxc/DXIL/DxilConstants.h"
#include "dxc/HLSL/HLResource.h"
#include "dxc/HLSL/HLOperations.h"
#include "dxc/DXIL/DxilSampler.h"
#include "dxc/DXIL/DxilShaderModel.h"
#include "dxc/DXIL/DxilSignature.h"
#include "dxc/DXIL/DxilFunctionProps.h"
#include "dxc/DXIL/DxilSubobject.h"
#include "dxc/DXIL/DxilResourceProperties.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace llvm37 {
template<typename T> class ArrayRef;
class LLVM37Context;
class Module;
class Function;
class Instruction;
class CallInst;
class MDTuple;
class MDNode;
class GlobalVariable;
class DIGlobalVariable;
class DebugInfoFinder;
class GetElementPtrInst;
}


namespace hlsl {

class ShaderModel;
class OP;

struct HLOptions {
  HLOptions()
      : bDefaultRowMajor(false), bIEEEStrict(false), bAllResourcesBound(false), bDisableOptimizations(false),
        bLegacyCBufferLoad(false), PackingStrategy(0), bUseMinPrecision(false), bDX9CompatMode(false),
        bFXCCompatMode(false), bLegacyResourceReservation(false), bForceZeroStoreLifetimes(false), unused(0) {
  }
  uint32_t GetHLOptionsRaw() const;
  void SetHLOptionsRaw(uint32_t data);
  unsigned bDefaultRowMajor        : 1;
  unsigned bIEEEStrict             : 1;
  unsigned bAllResourcesBound      : 1;
  unsigned bDisableOptimizations   : 1;
  unsigned bLegacyCBufferLoad      : 1;
  unsigned PackingStrategy         : 2;
  static_assert((unsigned)DXIL::PackingStrategy::Invalid < 4, "otherwise 2 bits is not enough to store PackingStrategy");
  unsigned bUseMinPrecision        : 1;
  unsigned bDX9CompatMode          : 1;
  unsigned bFXCCompatMode          : 1;
  unsigned bLegacyResourceReservation : 1;
  unsigned bForceZeroStoreLifetimes : 1;
  unsigned unused                  : 20;
};

typedef std::unordered_map<const llvm37::Function *, std::unique_ptr<DxilFunctionProps>> DxilFunctionPropsMap;

/// Use this class to manipulate HLDXIR of a shader.
class HLModule {
public:
  HLModule(llvm37::Module *pModule);
  ~HLModule();
  using Domain = DXIL::TessellatorDomain;
  // Subsystems.
  llvm37::LLVM37Context &GetCtx() const;
  llvm37::Module *GetModule() const;
  OP *GetOP() const;
  void SetShaderModel(const ShaderModel *pSM);
  const ShaderModel *GetShaderModel() const;
  void SetValidatorVersion(unsigned ValMajor, unsigned ValMinor);
  void GetValidatorVersion(unsigned &ValMajor, unsigned &ValMinor) const;
  void SetForceZeroStoreLifetimes(bool ForceZeroStoreLifetimes);
  bool GetForceZeroStoreLifetimes() const;

  // HLOptions
  void SetHLOptions(HLOptions &opts);
  const HLOptions &GetHLOptions() const;

  // AutoBindingSpace also enables automatic binding for libraries if set.
  // UINT_MAX == unset
  void SetAutoBindingSpace(uint32_t Space);
  uint32_t GetAutoBindingSpace() const;

  // Entry function.
  llvm37::Function *GetEntryFunction() const;
  void SetEntryFunction(llvm37::Function *pEntryFunc);
  const std::string &GetEntryFunctionName() const;
  void SetEntryFunctionName(const std::string &name);
  llvm37::Function *GetPatchConstantFunction();

  // Resources.
  unsigned AddCBuffer(std::unique_ptr<DxilCBuffer> pCB);
  DxilCBuffer &GetCBuffer(unsigned idx);
  const DxilCBuffer &GetCBuffer(unsigned idx) const;
  const std::vector<std::unique_ptr<DxilCBuffer> > &GetCBuffers() const;

  unsigned AddSampler(std::unique_ptr<DxilSampler> pSampler);
  DxilSampler &GetSampler(unsigned idx);
  const DxilSampler &GetSampler(unsigned idx) const;
  const std::vector<std::unique_ptr<DxilSampler> > &GetSamplers() const;

  unsigned AddSRV(std::unique_ptr<HLResource> pSRV);
  HLResource &GetSRV(unsigned idx);
  const HLResource &GetSRV(unsigned idx) const;
  const std::vector<std::unique_ptr<HLResource> > &GetSRVs() const;

  unsigned AddUAV(std::unique_ptr<HLResource> pUAV);
  HLResource &GetUAV(unsigned idx);
  const HLResource &GetUAV(unsigned idx) const;
  const std::vector<std::unique_ptr<HLResource> > &GetUAVs() const;

  void RemoveGlobal(llvm37::GlobalVariable *GV);
  void RemoveFunction(llvm37::Function *F);

  // ThreadGroupSharedMemory.
  typedef std::vector<llvm37::GlobalVariable*>::iterator tgsm_iterator;
  tgsm_iterator tgsm_begin();
  tgsm_iterator tgsm_end();
  void AddGroupSharedVariable(llvm37::GlobalVariable *GV);

  // Signatures.
  std::vector<uint8_t> &GetSerializedRootSignature();
  void SetSerializedRootSignature(const uint8_t *pData, unsigned size);

  // DxilFunctionProps.
  bool HasDxilFunctionProps(llvm37::Function *F);
  DxilFunctionProps &GetDxilFunctionProps(llvm37::Function *F);
  void AddDxilFunctionProps(llvm37::Function *F, std::unique_ptr<DxilFunctionProps> &info);
  void SetPatchConstantFunctionForHS(llvm37::Function *hullShaderFunc, llvm37::Function *patchConstantFunc);
  bool IsGraphicsShader(llvm37::Function *F); // vs,hs,ds,gs,ps
  bool IsPatchConstantShader(llvm37::Function *F);
  bool IsComputeShader(llvm37::Function *F);

  // Is an entry function that uses input/output signature conventions?
  // Includes: vs/hs/ds/gs/ps/cs as well as the patch constant function.
  bool IsEntryThatUsesSignatures(llvm37::Function *F);
  // Is F an entry?
  // Includes: IsEntryThatUsesSignatures and all ray tracing shaders.
  bool IsEntry(llvm37::Function *F);

  DxilFunctionAnnotation *GetFunctionAnnotation(llvm37::Function *F);
  DxilFunctionAnnotation *AddFunctionAnnotation(llvm37::Function *F);

  // Float Denorm mode.
  void SetFloat32DenormMode(const DXIL::Float32DenormMode mode);
  DXIL::Float32DenormMode GetFloat32DenormMode() const;

  // Default function linkage for libraries
  DXIL::DefaultLinkage GetDefaultLinkage() const;
  void SetDefaultLinkage(const DXIL::DefaultLinkage linkage);

  // HLDXIR metadata manipulation.
  /// Serialize HLDXIR in-memory form to metadata form.
  void EmitHLMetadata();
  /// Deserialize HLDXIR metadata form into in-memory form.
  void LoadHLMetadata();
  /// Delete any HLDXIR from the specified module.
  static void ClearHLMetadata(llvm37::Module &M);
  /// Create Metadata from a resource.
  llvm37::MDNode *DxilSamplerToMDNode(const DxilSampler &S);
  llvm37::MDNode *DxilSRVToMDNode(const DxilResource &SRV);
  llvm37::MDNode *DxilUAVToMDNode(const DxilResource &UAV);
  llvm37::MDNode *DxilCBufferToMDNode(const DxilCBuffer &CB);
  void LoadDxilResourceBaseFromMDNode(llvm37::MDNode *MD, DxilResourceBase &R);
  void LoadDxilResourceFromMDNode(llvm37::MDNode *MD, DxilResource &R);
  void LoadDxilSamplerFromMDNode(llvm37::MDNode *MD, DxilSampler &S);
  DxilResourceBase *
  AddResourceWithGlobalVariableAndProps(llvm37::Constant *GV,
                                        DxilResourceProperties &RP);
  unsigned GetBindingForResourceInCB(llvm37::GetElementPtrInst *CbPtr,
                                     llvm37::GlobalVariable *CbGV,
                                     DXIL::ResourceClass RC);

  // Type related methods.
  static bool IsStreamOutputPtrType(llvm37::Type *Ty);
  static bool IsStreamOutputType(llvm37::Type *Ty);
  static void GetParameterRowsAndCols(llvm37::Type *Ty, unsigned &rows, unsigned &cols,
                                      DxilParameterAnnotation &paramAnnotation);

  static void MergeGepUse(llvm37::Value *V);

  // HL code gen.
  template<class BuilderTy>
  static llvm37::CallInst *EmitHLOperationCall(BuilderTy &Builder,
                                          HLOpcodeGroup group, unsigned opcode,
                                          llvm37::Type *RetType,
                                          llvm37::ArrayRef<llvm37::Value *> paramList,
                                          llvm37::Module &M);

  // Caller must handle conversions to bool and no-ops
  static unsigned GetNumericCastOp(
    llvm37::Type *SrcTy, bool SrcIsUnsigned, llvm37::Type *DstTy, bool DstIsUnsigned);

  // Precise attribute.
  // Note: Precise will be marked on alloca inst with metadata in code gen.
  //       But mem2reg will remove alloca inst, so need mark precise with
  //       function call before mem2reg.
  static bool HasPreciseAttributeWithMetadata(llvm37::Instruction *I);
  static void MarkPreciseAttributeWithMetadata(llvm37::Instruction *I);
  static void ClearPreciseAttributeWithMetadata(llvm37::Instruction *I);
  template<class BuilderTy>
  static void MarkPreciseAttributeOnValWithFunctionCall(llvm37::Value *V,
                                                        BuilderTy &Builder, llvm37::Module &M);
  static void MarkPreciseAttributeOnPtrWithFunctionCall(llvm37::Value *Ptr,
                                                        llvm37::Module &M);
  static bool HasPreciseAttribute(llvm37::Function *F);

  // DXIL type system.
  DxilTypeSystem &GetTypeSystem();

  /// Emit llvm37.used array to make sure that optimizations do not remove unreferenced globals.
  void EmitLLVM37Used();
  std::vector<llvm37::GlobalVariable* > &GetLLVM37Used();

  // Release functions used to transfer ownership.
  DxilTypeSystem *ReleaseTypeSystem();
  OP *ReleaseOP();
  DxilFunctionPropsMap &&ReleaseFunctionPropsMap();

  llvm37::DebugInfoFinder &GetOrCreateDebugInfoFinder();
  // Create global variable debug info for element global variable based on the
  // whole global variable.
  static void CreateElementGlobalVariableDebugInfo(
      llvm37::GlobalVariable *GV, llvm37::DebugInfoFinder &DbgInfoFinder,
      llvm37::GlobalVariable *EltGV, unsigned sizeInBits, unsigned alignInBits,
      unsigned offsetInBits, llvm37::StringRef eltName);
  // Replace GV with NewGV in GlobalVariable debug info.
  static void
  UpdateGlobalVariableDebugInfo(llvm37::GlobalVariable *GV,
                                llvm37::DebugInfoFinder &DbgInfoFinder,
                                llvm37::GlobalVariable *NewGV);

  DxilSubobjects *GetSubobjects();
  const DxilSubobjects *GetSubobjects() const;
  DxilSubobjects *ReleaseSubobjects();
  void ResetSubobjects(DxilSubobjects *subobjects);

  // Reg binding for resource in cb.
  void AddRegBinding(unsigned CbID, unsigned ConstantIdx, unsigned Srv, unsigned Uav, unsigned Sampler);

private:
  // Signatures.
  std::vector<uint8_t> m_SerializedRootSignature;

  // Shader resources.
  std::vector<std::unique_ptr<HLResource> > m_SRVs;
  std::vector<std::unique_ptr<HLResource> > m_UAVs;
  std::vector<std::unique_ptr<DxilCBuffer> > m_CBuffers;
  std::vector<std::unique_ptr<DxilSampler> > m_Samplers;

  // ThreadGroupSharedMemory.
  std::vector<llvm37::GlobalVariable*>  m_TGSMVariables;

  // High level function info.
  std::unordered_map<const llvm37::Function *, std::unique_ptr<DxilFunctionProps>>  m_DxilFunctionPropsMap;
  std::unordered_set<llvm37::Function *>  m_PatchConstantFunctions;

  // Resource bindings for res in cb.
  // Key = CbID << 32 | ConstantIdx. Val is reg binding.
  std::unordered_map<uint64_t, unsigned> m_SrvBindingInCB;
  std::unordered_map<uint64_t, unsigned> m_UavBindingInCB;
  std::unordered_map<uint64_t, unsigned> m_SamplerBindingInCB;

private:
  llvm37::LLVM37Context &m_Ctx;
  llvm37::Module *m_pModule;
  llvm37::Function *m_pEntryFunc;
  std::string m_EntryName;
  std::unique_ptr<DxilMDHelper> m_pMDHelper;
  std::unique_ptr<llvm37::DebugInfoFinder> m_pDebugInfoFinder;
  const ShaderModel *m_pSM;
  unsigned m_DxilMajor;
  unsigned m_DxilMinor;
  unsigned m_ValMajor;
  unsigned m_ValMinor;
  DXIL::Float32DenormMode m_Float32DenormMode;
  HLOptions m_Options;
  std::unique_ptr<OP> m_pOP;
  size_t m_pUnused;
  uint32_t m_AutoBindingSpace;
  DXIL::DefaultLinkage m_DefaultLinkage;
  std::unique_ptr<DxilSubobjects> m_pSubobjects;

  // DXIL metadata serialization/deserialization.
  llvm37::MDTuple *EmitHLResources();
  void LoadHLResources(const llvm37::MDOperand &MDO);
  llvm37::MDTuple *EmitHLShaderProperties();
  void LoadHLShaderProperties(const llvm37::MDOperand &MDO);
  llvm37::MDTuple *EmitDxilShaderProperties();
  // LLVM37 used.
  std::vector<llvm37::GlobalVariable*> m_LLVM37Used;

  // Type annotations.
  std::unique_ptr<DxilTypeSystem> m_pTypeSystem;

  // Helpers.
  template<typename T> unsigned AddResource(std::vector<std::unique_ptr<T> > &Vec, std::unique_ptr<T> pRes);
};


/// Use this class to manipulate metadata of extra metadata record properties that are specific to high-level DX IR.
class HLExtraPropertyHelper : public DxilExtraPropertyHelper {
public:
  HLExtraPropertyHelper(llvm37::Module *pModule);
  virtual ~HLExtraPropertyHelper() {}

  virtual void EmitSignatureElementProperties(const DxilSignatureElement &SE, std::vector<llvm37::Metadata *> &MDVals);
  virtual void LoadSignatureElementProperties(const llvm37::MDOperand &MDO, DxilSignatureElement &SE);
};

} // namespace hlsl

