//===----- CGMSHLSLRuntime.h - Interface to HLSL Runtime ----------------===//
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// CGHLSLRuntime.h                                                           //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
//  This provides a class for HLSL code generation.                          //
//
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <functional>
#include <llvm37/ADT/SmallVector.h> // HLSL Change

namespace llvm37 {
class Function;
class Value;
class Constant;
class TerminatorInst;
class GlobalVariable;
class Type;
class BasicBlock;
class BranchInst;
class SwitchInst;
template <typename T> class ArrayRef;
}

namespace clang {
class Decl;
class QualType;
class ExtVectorType;
class ASTContext;
class FunctionDecl;
class CallExpr;
class InitListExpr;
class Expr;
class Stmt;
class Attr;
class VarDecl;
class HLSLRootSignatureAttr;

namespace CodeGen {
class CodeGenModule;
class ReturnValueSlot;
class CodeGenFunction;

class RValue;
class LValue;

class CGHLSLRuntime {
protected:
  CodeGenModule &CGM;
  llvm37::SmallVector<llvm37::BranchInst*, 16> m_DxBreaks;

public:
  CGHLSLRuntime(CodeGenModule &CGM) : CGM(CGM) {}
  virtual ~CGHLSLRuntime();

  virtual void addResource(Decl *D) = 0;
  virtual void addSubobject(Decl *D) = 0;
  virtual void FinishCodeGen() = 0;
  virtual RValue EmitHLSLBuiltinCallExpr(CodeGenFunction &CGF,
                                         const FunctionDecl *FD,
                                         const CallExpr *E,
                                         ReturnValueSlot ReturnValue) = 0;
  // Is E is a c++ init list not a hlsl init list which only match size.
  virtual bool IsTrivalInitListExpr(CodeGenFunction &CGF, InitListExpr *E) = 0;
  virtual llvm37::Value *EmitHLSLInitListExpr(CodeGenFunction &CGF, InitListExpr *E,
      // The destPtr when emiting aggregate init, for normal case, it will be null.
      llvm37::Value *DestPtr) = 0;
  virtual llvm37::Constant *EmitHLSLConstInitListExpr(CodeGenModule &CGM, InitListExpr *E) = 0;

  virtual void EmitHLSLOutParamConversionInit(
      CodeGenFunction &CGF, const FunctionDecl *FD, const CallExpr *E,
      llvm37::SmallVector<LValue, 8> &castArgList,
      llvm37::SmallVector<const Stmt *, 8> &argList,
      llvm37::SmallVector<LValue, 8> &lifetimeCleanupList,
      const std::function<void(const VarDecl *, llvm37::Value *)> &TmpArgMap) = 0;
  virtual void EmitHLSLOutParamConversionCopyBack(
      CodeGenFunction &CGF, llvm37::SmallVector<LValue, 8> &castArgList,
      llvm37::SmallVector<LValue, 8> &lifetimeCleanupList) = 0;
  virtual void MarkRetTemp(CodeGenFunction &CGF, llvm37::Value *V,
                          clang::QualType QaulTy) = 0;
  virtual llvm37::Value *EmitHLSLMatrixOperationCall(CodeGenFunction &CGF, const clang::Expr *E, llvm37::Type *RetType,
      llvm37::ArrayRef<llvm37::Value*> paramList) = 0;
  virtual void EmitHLSLDiscard(CodeGenFunction &CGF) = 0;
  virtual llvm37::BranchInst *EmitHLSLCondBreak(CodeGenFunction &CGF, llvm37::Function *F, llvm37::BasicBlock *DestBB, llvm37::BasicBlock *AltBB) = 0;

  // For [] on matrix
  virtual llvm37::Value *EmitHLSLMatrixSubscript(CodeGenFunction &CGF,
                                          llvm37::Type *RetType,
                                          llvm37::Value *Ptr, llvm37::Value *Idx,
                                          clang::QualType Ty) = 0;
  // For ._m on matrix
  virtual llvm37::Value *EmitHLSLMatrixElement(CodeGenFunction &CGF,
                                          llvm37::Type *RetType,
                                          llvm37::ArrayRef<llvm37::Value*> paramList,
                                          clang::QualType Ty) = 0;

  virtual llvm37::Value *EmitHLSLMatrixLoad(CodeGenFunction &CGF,
                                          llvm37::Value *Ptr,
                                          clang::QualType Ty) = 0;
  virtual void EmitHLSLMatrixStore(CodeGenFunction &CGF, llvm37::Value *Val,
                                   llvm37::Value *DestPtr,
                                   clang::QualType Ty) = 0;
  virtual void EmitHLSLAggregateCopy(CodeGenFunction &CGF, llvm37::Value *SrcPtr,
                                   llvm37::Value *DestPtr,
                                   clang::QualType Ty) = 0;
  virtual void EmitHLSLAggregateStore(CodeGenFunction &CGF, llvm37::Value *Val,
                                   llvm37::Value *DestPtr,
                                   clang::QualType Ty) = 0;
  virtual void EmitHLSLFlatConversion(CodeGenFunction &CGF, llvm37::Value *Val,
                                   llvm37::Value *DestPtr,
                                   clang::QualType Ty, clang::QualType SrcTy) = 0;
  virtual void EmitHLSLFlatConversionAggregateCopy(CodeGenFunction &CGF, llvm37::Value *SrcPtr,
                                   clang::QualType SrcTy,
                                   llvm37::Value *DestPtr,
                                   clang::QualType DestTy) = 0;
  virtual void EmitHLSLRootSignature(CodeGenFunction &CGF,
                                     clang::HLSLRootSignatureAttr *RSA,
                                     llvm37::Function *Fn) = 0;
  virtual llvm37::Value *EmitHLSLLiteralCast(CodeGenFunction &CGF, llvm37::Value *Src, clang::QualType SrcType,
                                               clang::QualType DstType) = 0;

  virtual void AddHLSLFunctionInfo(llvm37::Function *, const FunctionDecl *FD) = 0;
  virtual void EmitHLSLFunctionProlog(llvm37::Function *, const FunctionDecl *FD) = 0;

  
  virtual void AddControlFlowHint(CodeGenFunction &CGF, const Stmt &S, llvm37::TerminatorInst *TI, llvm37::ArrayRef<const Attr *> Attrs) = 0;

  virtual void FinishAutoVar(CodeGenFunction &CGF, const VarDecl &D,
                             llvm37::Value *V) = 0;
  virtual void MarkIfStmt(CodeGenFunction &CGF, llvm37::BasicBlock *endIfBB) = 0;
  virtual void MarkSwitchStmt(CodeGenFunction &CGF,
                              llvm37::SwitchInst *switchInst,
                              llvm37::BasicBlock *endSwitch) = 0;
  virtual void MarkReturnStmt(CodeGenFunction &CGF,
                              llvm37::BasicBlock *bbWithRet) = 0;
  virtual void MarkLoopStmt(CodeGenFunction &CGF,
                             llvm37::BasicBlock *loopContinue,
                             llvm37::BasicBlock *loopExit) = 0;

  virtual void MarkScopeEnd(CodeGenFunction &CGF) = 0;
};

/// Create an instance of a HLSL runtime class.
CGHLSLRuntime *CreateMSHLSLRuntime(CodeGenModule &CGM);
}
}
