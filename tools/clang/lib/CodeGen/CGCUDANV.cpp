//===----- CGCUDANV.cpp - Interface to NVIDIA CUDA Runtime ----------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This provides a class for CUDA code generation targeting the NVIDIA CUDA
// runtime library.
//
//===----------------------------------------------------------------------===//

#include "CGCUDARuntime.h"
#include "CodeGenFunction.h"
#include "CodeGenModule.h"
#include "clang/AST/Decl.h"
#include "llvm37/IR/BasicBlock.h"
#include "llvm37/IR/CallSite.h"
#include "llvm37/IR/Constants.h"
#include "llvm37/IR/DerivedTypes.h"

using namespace clang;
using namespace CodeGen;

namespace {

class CGNVCUDARuntime : public CGCUDARuntime {

private:
  llvm37::Type *IntTy, *SizeTy, *VoidTy;
  llvm37::PointerType *CharPtrTy, *VoidPtrTy, *VoidPtrPtrTy;

  /// Convenience reference to LLVM37 Context
  llvm37::LLVM37Context &Context;
  /// Convenience reference to the current module
  llvm37::Module &TheModule;
  /// Keeps track of kernel launch stubs emitted in this module
  llvm37::SmallVector<llvm37::Function *, 16> EmittedKernels;
  /// Keeps track of variables containing handles of GPU binaries. Populated by
  /// ModuleCtorFunction() and used to create corresponding cleanup calls in
  /// ModuleDtorFunction()
  llvm37::SmallVector<llvm37::GlobalVariable *, 16> GpuBinaryHandles;

  llvm37::Constant *getSetupArgumentFn() const;
  llvm37::Constant *getLaunchFn() const;

  /// Creates a function to register all kernel stubs generated in this module.
  llvm37::Function *makeRegisterKernelsFn();

  /// Helper function that generates a constant string and returns a pointer to
  /// the start of the string.  The result of this function can be used anywhere
  /// where the C code specifies const char*.
  llvm37::Constant *makeConstantString(const std::string &Str,
                                     const std::string &Name = "",
                                     unsigned Alignment = 0) {
    llvm37::Constant *Zeros[] = {llvm37::ConstantInt::get(SizeTy, 0),
                               llvm37::ConstantInt::get(SizeTy, 0)};
    auto *ConstStr = CGM.GetAddrOfConstantCString(Str, Name.c_str());
    return llvm37::ConstantExpr::getGetElementPtr(ConstStr->getValueType(),
                                                ConstStr, Zeros);
 }

  void emitDeviceStubBody(CodeGenFunction &CGF, FunctionArgList &Args);

public:
  CGNVCUDARuntime(CodeGenModule &CGM);

  void emitDeviceStub(CodeGenFunction &CGF, FunctionArgList &Args) override;
  /// Creates module constructor function
  llvm37::Function *makeModuleCtorFunction() override;
  /// Creates module destructor function
  llvm37::Function *makeModuleDtorFunction() override;
};

}

CGNVCUDARuntime::CGNVCUDARuntime(CodeGenModule &CGM)
    : CGCUDARuntime(CGM), Context(CGM.getLLVM37Context()),
      TheModule(CGM.getModule()) {
  CodeGen::CodeGenTypes &Types = CGM.getTypes();
  ASTContext &Ctx = CGM.getContext();

  IntTy = Types.ConvertType(Ctx.IntTy);
  SizeTy = Types.ConvertType(Ctx.getSizeType());
  VoidTy = llvm37::Type::getVoidTy(Context);

  CharPtrTy = llvm37::PointerType::getUnqual(Types.ConvertType(Ctx.CharTy));
  VoidPtrTy = cast<llvm37::PointerType>(Types.ConvertType(Ctx.VoidPtrTy));
  VoidPtrPtrTy = VoidPtrTy->getPointerTo();
}

llvm37::Constant *CGNVCUDARuntime::getSetupArgumentFn() const {
  // cudaError_t cudaSetupArgument(void *, size_t, size_t)
  std::vector<llvm37::Type*> Params;
  Params.push_back(VoidPtrTy);
  Params.push_back(SizeTy);
  Params.push_back(SizeTy);
  return CGM.CreateRuntimeFunction(llvm37::FunctionType::get(IntTy,
                                                           Params, false),
                                   "cudaSetupArgument");
}

llvm37::Constant *CGNVCUDARuntime::getLaunchFn() const {
  // cudaError_t cudaLaunch(char *)
  return CGM.CreateRuntimeFunction(
      llvm37::FunctionType::get(IntTy, CharPtrTy, false), "cudaLaunch");
}

void CGNVCUDARuntime::emitDeviceStub(CodeGenFunction &CGF,
                                     FunctionArgList &Args) {
  EmittedKernels.push_back(CGF.CurFn);
  emitDeviceStubBody(CGF, Args);
}

void CGNVCUDARuntime::emitDeviceStubBody(CodeGenFunction &CGF,
                                         FunctionArgList &Args) {
  // Build the argument value list and the argument stack struct type.
  SmallVector<llvm37::Value *, 16> ArgValues;
  std::vector<llvm37::Type *> ArgTypes;
  for (FunctionArgList::const_iterator I = Args.begin(), E = Args.end();
       I != E; ++I) {
    llvm37::Value *V = CGF.GetAddrOfLocalVar(*I);
    ArgValues.push_back(V);
    assert(isa<llvm37::PointerType>(V->getType()) && "Arg type not PointerType");
    ArgTypes.push_back(cast<llvm37::PointerType>(V->getType())->getElementType());
  }
  llvm37::StructType *ArgStackTy = llvm37::StructType::get(Context, ArgTypes);

  llvm37::BasicBlock *EndBlock = CGF.createBasicBlock("setup.end");

  // Emit the calls to cudaSetupArgument
  llvm37::Constant *cudaSetupArgFn = getSetupArgumentFn();
  for (unsigned I = 0, E = Args.size(); I != E; ++I) {
    llvm37::Value *Args[3];
    llvm37::BasicBlock *NextBlock = CGF.createBasicBlock("setup.next");
    Args[0] = CGF.Builder.CreatePointerCast(ArgValues[I], VoidPtrTy);
    Args[1] = CGF.Builder.CreateIntCast(
        llvm37::ConstantExpr::getSizeOf(ArgTypes[I]),
        SizeTy, false);
    Args[2] = CGF.Builder.CreateIntCast(
        llvm37::ConstantExpr::getOffsetOf(ArgStackTy, I),
        SizeTy, false);
    llvm37::CallSite CS = CGF.EmitRuntimeCallOrInvoke(cudaSetupArgFn, Args);
    llvm37::Constant *Zero = llvm37::ConstantInt::get(IntTy, 0);
    llvm37::Value *CSZero = CGF.Builder.CreateICmpEQ(CS.getInstruction(), Zero);
    CGF.Builder.CreateCondBr(CSZero, NextBlock, EndBlock);
    CGF.EmitBlock(NextBlock);
  }

  // Emit the call to cudaLaunch
  llvm37::Constant *cudaLaunchFn = getLaunchFn();
  llvm37::Value *Arg = CGF.Builder.CreatePointerCast(CGF.CurFn, CharPtrTy);
  CGF.EmitRuntimeCallOrInvoke(cudaLaunchFn, Arg);
  CGF.EmitBranch(EndBlock);

  CGF.EmitBlock(EndBlock);
}

/// Creates internal function to register all kernel stubs generated in this
/// module with the CUDA runtime.
/// \code
/// void __cuda_register_kernels(void** GpuBinaryHandle) {
///    __cudaRegisterFunction(GpuBinaryHandle,Kernel0,...);
///    ...
///    __cudaRegisterFunction(GpuBinaryHandle,KernelM,...);
/// }
/// \endcode
llvm37::Function *CGNVCUDARuntime::makeRegisterKernelsFn() {
  llvm37::Function *RegisterKernelsFunc = llvm37::Function::Create(
      llvm37::FunctionType::get(VoidTy, VoidPtrPtrTy, false),
      llvm37::GlobalValue::InternalLinkage, "__cuda_register_kernels", &TheModule);
  llvm37::BasicBlock *EntryBB =
      llvm37::BasicBlock::Create(Context, "entry", RegisterKernelsFunc);
  CGBuilderTy Builder(Context);
  Builder.SetInsertPoint(EntryBB);

  // void __cudaRegisterFunction(void **, const char *, char *, const char *,
  //                             int, uint3*, uint3*, dim3*, dim3*, int*)
  std::vector<llvm37::Type *> RegisterFuncParams = {
      VoidPtrPtrTy, CharPtrTy, CharPtrTy, CharPtrTy, IntTy,
      VoidPtrTy,    VoidPtrTy, VoidPtrTy, VoidPtrTy, IntTy->getPointerTo()};
  llvm37::Constant *RegisterFunc = CGM.CreateRuntimeFunction(
      llvm37::FunctionType::get(IntTy, RegisterFuncParams, false),
      "__cudaRegisterFunction");

  // Extract GpuBinaryHandle passed as the first argument passed to
  // __cuda_register_kernels() and generate __cudaRegisterFunction() call for
  // each emitted kernel.
  llvm37::Argument &GpuBinaryHandlePtr = *RegisterKernelsFunc->arg_begin();
  for (llvm37::Function *Kernel : EmittedKernels) {
    llvm37::Constant *KernelName = makeConstantString(Kernel->getName());
    llvm37::Constant *NullPtr = llvm37::ConstantPointerNull::get(VoidPtrTy);
    llvm37::Value *args[] = {
        &GpuBinaryHandlePtr, Builder.CreateBitCast(Kernel, VoidPtrTy),
        KernelName, KernelName, llvm37::ConstantInt::get(IntTy, -1), NullPtr,
        NullPtr, NullPtr, NullPtr,
        llvm37::ConstantPointerNull::get(IntTy->getPointerTo())};
    Builder.CreateCall(RegisterFunc, args);
  }

  Builder.CreateRetVoid();
  return RegisterKernelsFunc;
}

/// Creates a global constructor function for the module:
/// \code
/// void __cuda_module_ctor(void*) {
///     Handle0 = __cudaRegisterFatBinary(GpuBinaryBlob0);
///     __cuda_register_kernels(Handle0);
///     ...
///     HandleN = __cudaRegisterFatBinary(GpuBinaryBlobN);
///     __cuda_register_kernels(HandleN);
/// }
/// \endcode
llvm37::Function *CGNVCUDARuntime::makeModuleCtorFunction() {
  // void __cuda_register_kernels(void* handle);
  llvm37::Function *RegisterKernelsFunc = makeRegisterKernelsFn();
  // void ** __cudaRegisterFatBinary(void *);
  llvm37::Constant *RegisterFatbinFunc = CGM.CreateRuntimeFunction(
      llvm37::FunctionType::get(VoidPtrPtrTy, VoidPtrTy, false),
      "__cudaRegisterFatBinary");
  // struct { int magic, int version, void * gpu_binary, void * dont_care };
  llvm37::StructType *FatbinWrapperTy =
      llvm37::StructType::get(IntTy, IntTy, VoidPtrTy, VoidPtrTy, nullptr);

  llvm37::Function *ModuleCtorFunc = llvm37::Function::Create(
      llvm37::FunctionType::get(VoidTy, VoidPtrTy, false),
      llvm37::GlobalValue::InternalLinkage, "__cuda_module_ctor", &TheModule);
  llvm37::BasicBlock *CtorEntryBB =
      llvm37::BasicBlock::Create(Context, "entry", ModuleCtorFunc);
  CGBuilderTy CtorBuilder(Context);

  CtorBuilder.SetInsertPoint(CtorEntryBB);

  // For each GPU binary, register it with the CUDA runtime and store returned
  // handle in a global variable and save the handle in GpuBinaryHandles vector
  // to be cleaned up in destructor on exit. Then associate all known kernels
  // with the GPU binary handle so CUDA runtime can figure out what to call on
  // the GPU side.
  for (const std::string &GpuBinaryFileName :
       CGM.getCodeGenOpts().CudaGpuBinaryFileNames) {
    llvm37::ErrorOr<std::unique_ptr<llvm37::MemoryBuffer>> GpuBinaryOrErr =
        llvm37::MemoryBuffer::getFileOrSTDIN(GpuBinaryFileName);
    if (std::error_code EC = GpuBinaryOrErr.getError()) {
      CGM.getDiags().Report(diag::err_cannot_open_file) << GpuBinaryFileName
                                                        << EC.message();
      continue;
    }

    // Create initialized wrapper structure that points to the loaded GPU binary
    llvm37::Constant *Values[] = {
        llvm37::ConstantInt::get(IntTy, 0x466243b1), // Fatbin wrapper magic.
        llvm37::ConstantInt::get(IntTy, 1),          // Fatbin version.
        makeConstantString(GpuBinaryOrErr.get()->getBuffer(), "", 16), // Data.
        llvm37::ConstantPointerNull::get(VoidPtrTy)}; // Unused in fatbin v1.
    llvm37::GlobalVariable *FatbinWrapper = new llvm37::GlobalVariable(
        TheModule, FatbinWrapperTy, true, llvm37::GlobalValue::InternalLinkage,
        llvm37::ConstantStruct::get(FatbinWrapperTy, Values),
        "__cuda_fatbin_wrapper");

    // GpuBinaryHandle = __cudaRegisterFatBinary(&FatbinWrapper);
    llvm37::CallInst *RegisterFatbinCall = CtorBuilder.CreateCall(
        RegisterFatbinFunc,
        CtorBuilder.CreateBitCast(FatbinWrapper, VoidPtrTy));
    llvm37::GlobalVariable *GpuBinaryHandle = new llvm37::GlobalVariable(
        TheModule, VoidPtrPtrTy, false, llvm37::GlobalValue::InternalLinkage,
        llvm37::ConstantPointerNull::get(VoidPtrPtrTy), "__cuda_gpubin_handle");
    CtorBuilder.CreateStore(RegisterFatbinCall, GpuBinaryHandle, false);

    // Call __cuda_register_kernels(GpuBinaryHandle);
    CtorBuilder.CreateCall(RegisterKernelsFunc, RegisterFatbinCall);

    // Save GpuBinaryHandle so we can unregister it in destructor.
    GpuBinaryHandles.push_back(GpuBinaryHandle);
  }

  CtorBuilder.CreateRetVoid();
  return ModuleCtorFunc;
}

/// Creates a global destructor function that unregisters all GPU code blobs
/// registered by constructor.
/// \code
/// void __cuda_module_dtor(void*) {
///     __cudaUnregisterFatBinary(Handle0);
///     ...
///     __cudaUnregisterFatBinary(HandleN);
/// }
/// \endcode
llvm37::Function *CGNVCUDARuntime::makeModuleDtorFunction() {
  // void __cudaUnregisterFatBinary(void ** handle);
  llvm37::Constant *UnregisterFatbinFunc = CGM.CreateRuntimeFunction(
      llvm37::FunctionType::get(VoidTy, VoidPtrPtrTy, false),
      "__cudaUnregisterFatBinary");

  llvm37::Function *ModuleDtorFunc = llvm37::Function::Create(
      llvm37::FunctionType::get(VoidTy, VoidPtrTy, false),
      llvm37::GlobalValue::InternalLinkage, "__cuda_module_dtor", &TheModule);
  llvm37::BasicBlock *DtorEntryBB =
      llvm37::BasicBlock::Create(Context, "entry", ModuleDtorFunc);
  CGBuilderTy DtorBuilder(Context);
  DtorBuilder.SetInsertPoint(DtorEntryBB);

  for (llvm37::GlobalVariable *GpuBinaryHandle : GpuBinaryHandles) {
    DtorBuilder.CreateCall(UnregisterFatbinFunc,
                           DtorBuilder.CreateLoad(GpuBinaryHandle, false));
  }

  DtorBuilder.CreateRetVoid();
  return ModuleDtorFunc;
}

CGCUDARuntime *CodeGen::CreateNVCUDARuntime(CodeGenModule &CGM) {
  return new CGNVCUDARuntime(CGM);
}
