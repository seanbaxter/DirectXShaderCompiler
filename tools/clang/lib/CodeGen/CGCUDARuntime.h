//===----- CGCUDARuntime.h - Interface to CUDA Runtimes ---------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This provides an abstract class for CUDA code generation.  Concrete
// subclasses of this implement code generation for specific CUDA
// runtime libraries.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_LIB_CODEGEN_CGCUDARUNTIME_H
#define LLVM37_CLANG_LIB_CODEGEN_CGCUDARUNTIME_H

namespace llvm37 {
class Function;
}

namespace clang {

class CUDAKernelCallExpr;

namespace CodeGen {

class CodeGenFunction;
class CodeGenModule;
class FunctionArgList;
class ReturnValueSlot;
class RValue;

class CGCUDARuntime {
protected:
  CodeGenModule &CGM;

public:
  CGCUDARuntime(CodeGenModule &CGM) : CGM(CGM) {}
  virtual ~CGCUDARuntime();

  virtual RValue EmitCUDAKernelCallExpr(CodeGenFunction &CGF,
                                        const CUDAKernelCallExpr *E,
                                        ReturnValueSlot ReturnValue);

  /// Emits a kernel launch stub.
  virtual void emitDeviceStub(CodeGenFunction &CGF, FunctionArgList &Args) = 0;

  /// Constructs and returns a module initialization function or nullptr if it's
  /// not needed. Must be called after all kernels have been emitted.
  virtual llvm37::Function *makeModuleCtorFunction() = 0;

  /// Returns a module cleanup function or nullptr if it's not needed.
  /// Must be called after ModuleCtorFunction
  virtual llvm37::Function *makeModuleDtorFunction() = 0;
};

/// Creates an instance of a CUDA runtime class.
CGCUDARuntime *CreateNVCUDARuntime(CodeGenModule &CGM);

}
}

#endif
