///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilOperations.h                                                          //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// Implementation of DXIL operation tables.                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once

namespace llvm37 {
class LLVM37Context;
class Module;
class Type;
class StructType;
class Function;
class Constant;
class Value;
class Instruction;
class CallInst;
}
#include "llvm37/IR/Attributes.h"
#include "llvm37/ADT/StringRef.h"
#include "llvm37/ADT/MapVector.h"

#include "DxilConstants.h"
#include <unordered_map>

namespace hlsl {

/// Use this utility class to interact with DXIL operations.
class OP {
public:
  using OpCode = DXIL::OpCode;
  using OpCodeClass = DXIL::OpCodeClass;

public:
  OP() = delete;
  OP(llvm37::LLVM37Context &Ctx, llvm37::Module *pModule);

  void RefreshCache();
  void FixOverloadNames();

  llvm37::Function *GetOpFunc(OpCode OpCode, llvm37::Type *pOverloadType);
  const llvm37::SmallMapVector<llvm37::Type *, llvm37::Function *, 8> &GetOpFuncList(OpCode OpCode) const;
  bool IsDxilOpUsed(OpCode opcode) const;
  void RemoveFunction(llvm37::Function *F);
  llvm37::LLVM37Context &GetCtx() { return m_Ctx; }
  llvm37::Type *GetHandleType() const;
  llvm37::Type *GetResourcePropertiesType() const;
  llvm37::Type *GetResourceBindingType() const;
  llvm37::Type *GetDimensionsType() const;
  llvm37::Type *GetSamplePosType() const;
  llvm37::Type *GetBinaryWithCarryType() const;
  llvm37::Type *GetBinaryWithTwoOutputsType() const;
  llvm37::Type *GetSplitDoubleType() const;
  llvm37::Type *GetFourI32Type() const;
  llvm37::Type *GetFourI16Type() const;

  llvm37::Type *GetResRetType(llvm37::Type *pOverloadType);
  llvm37::Type *GetCBufferRetType(llvm37::Type *pOverloadType);
  llvm37::Type *GetVectorType(unsigned numElements, llvm37::Type *pOverloadType);
  bool IsResRetType(llvm37::Type *Ty);

  // Try to get the opcode class for a function.
  // Return true and set `opClass` if the given function is a dxil function.
  // Return false if the given function is not a dxil function.
  bool GetOpCodeClass(const llvm37::Function *F, OpCodeClass &opClass);

  // To check if operation uses strict precision types
  bool UseMinPrecision();
  // Set if operation uses strict precision types or not.
  void SetMinPrecision(bool bMinPrecision);

  // Get the size of the type for a given layout
  uint64_t GetAllocSizeForType(llvm37::Type *Ty);

  // LLVM37 helpers. Perhaps, move to a separate utility class.
  llvm37::Constant *GetI1Const(bool v);
  llvm37::Constant *GetI8Const(char v);
  llvm37::Constant *GetU8Const(unsigned char v);
  llvm37::Constant *GetI16Const(int v);
  llvm37::Constant *GetU16Const(unsigned v);
  llvm37::Constant *GetI32Const(int v);
  llvm37::Constant *GetU32Const(unsigned v);
  llvm37::Constant *GetU64Const(unsigned long long v);
  llvm37::Constant *GetFloatConst(float v);
  llvm37::Constant *GetDoubleConst(double v);

  static llvm37::Type *GetOverloadType(OpCode OpCode, llvm37::Function *F);
  static OpCode GetDxilOpFuncCallInst(const llvm37::Instruction *I);
  static const char *GetOpCodeName(OpCode OpCode);
  static const char *GetAtomicOpName(DXIL::AtomicBinOpCode OpCode);
  static OpCodeClass GetOpCodeClass(OpCode OpCode);
  static const char *GetOpCodeClassName(OpCode OpCode);
  static llvm37::Attribute::AttrKind GetMemAccessAttr(OpCode opCode);
  static bool IsOverloadLegal(OpCode OpCode, llvm37::Type *pType);
  static bool CheckOpCodeTable();
  static bool IsDxilOpFuncName(llvm37::StringRef name);
  static bool IsDxilOpFunc(const llvm37::Function *F);
  static bool IsDxilOpFuncCallInst(const llvm37::Instruction *I);
  static bool IsDxilOpFuncCallInst(const llvm37::Instruction *I, OpCode opcode);
  static bool IsDxilOpWave(OpCode C);
  static bool IsDxilOpGradient(OpCode C);
  static bool IsDxilOpFeedback(OpCode C);
  static bool IsDxilOpTypeName(llvm37::StringRef name);
  static bool IsDxilOpType(llvm37::StructType *ST);
  static bool IsDupDxilOpType(llvm37::StructType *ST);
  static llvm37::StructType *GetOriginalDxilOpType(llvm37::StructType *ST,
                                                 llvm37::Module &M);
  static void GetMinShaderModelAndMask(OpCode C, bool bWithTranslation,
                                       unsigned &major, unsigned &minor,
                                       unsigned &mask);
  static void GetMinShaderModelAndMask(const llvm37::CallInst *CI, bool bWithTranslation,
                                       unsigned valMajor, unsigned valMinor,
                                       unsigned &major, unsigned &minor,
                                       unsigned &mask);

private:
  // Per-module properties.
  llvm37::LLVM37Context &m_Ctx;
  llvm37::Module *m_pModule;

  llvm37::Type *m_pHandleType;
  llvm37::Type *m_pResourcePropertiesType;
  llvm37::Type *m_pResourceBindingType;
  llvm37::Type *m_pDimensionsType;
  llvm37::Type *m_pSamplePosType;
  llvm37::Type *m_pBinaryWithCarryType;
  llvm37::Type *m_pBinaryWithTwoOutputsType;
  llvm37::Type *m_pSplitDoubleType;
  llvm37::Type *m_pFourI32Type;
  llvm37::Type *m_pFourI16Type;

  DXIL::LowPrecisionMode m_LowPrecisionMode;

  static const unsigned kUserDefineTypeSlot = 9;
  static const unsigned kObjectTypeSlot = 10;
  static const unsigned kNumTypeOverloads = 11; // void, h,f,d, i1, i8,i16,i32,i64, udt, obj

  llvm37::Type *m_pResRetType[kNumTypeOverloads];
  llvm37::Type *m_pCBufferRetType[kNumTypeOverloads];

  struct OpCodeCacheItem {
    llvm37::SmallMapVector<llvm37::Type *, llvm37::Function *, 8> pOverloads;
  };
  OpCodeCacheItem m_OpCodeClassCache[(unsigned)OpCodeClass::NumOpClasses];
  std::unordered_map<const llvm37::Function *, OpCodeClass> m_FunctionToOpClass;
  void UpdateCache(OpCodeClass opClass, llvm37::Type * Ty, llvm37::Function *F);
private:
  // Static properties.
  struct OpCodeProperty {
    OpCode opCode;
    const char *pOpCodeName;
    OpCodeClass opCodeClass;
    const char *pOpCodeClassName;
    bool bAllowOverload[kNumTypeOverloads];   // void, h,f,d, i1, i8,i16,i32,i64, udt
    llvm37::Attribute::AttrKind FuncAttr;
  };
  static const OpCodeProperty m_OpCodeProps[(unsigned)OpCode::NumOpCodes];

  static const char *m_OverloadTypeName[kNumTypeOverloads];
  static const char *m_NamePrefix;
  static const char *m_TypePrefix;
  static const char *m_MatrixTypePrefix;
  static unsigned GetTypeSlot(llvm37::Type *pType);
  static const char *GetOverloadTypeName(unsigned TypeSlot);
  static llvm37::StringRef GetTypeName(llvm37::Type *Ty, std::string &str);
  static llvm37::StringRef ConstructOverloadName(llvm37::Type *Ty, DXIL::OpCode opCode,
                                               std::string &funcNameStorage);
};

} // namespace hlsl
