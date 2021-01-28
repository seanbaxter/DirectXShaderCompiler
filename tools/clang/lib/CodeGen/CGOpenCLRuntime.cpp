//===----- CGOpenCLRuntime.cpp - Interface to OpenCL Runtimes -------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This provides an abstract class for OpenCL code generation.  Concrete
// subclasses of this implement code generation for specific OpenCL
// runtime libraries.
//
//===----------------------------------------------------------------------===//

#include "CGOpenCLRuntime.h"
#include "CodeGenFunction.h"
#include "llvm37/IR/DerivedTypes.h"
#include "llvm37/IR/GlobalValue.h"
#include <assert.h>

using namespace clang;
using namespace CodeGen;

// HLSL Change Starts
// No OpenCL codegen support, so simply skip all of this compilation.
// Here are enough stubs to link the current targets.
#if 0
// HLSL Change Ends

CGOpenCLRuntime::~CGOpenCLRuntime() {}

void CGOpenCLRuntime::EmitWorkGroupLocalVarDecl(CodeGenFunction &CGF,
                                                const VarDecl &D) {
  return CGF.EmitStaticVarDecl(D, llvm37::GlobalValue::InternalLinkage);
}

llvm37::Type *CGOpenCLRuntime::convertOpenCLSpecificType(const Type *T) {
  assert(T->isOpenCLSpecificType() &&
         "Not an OpenCL specific type!");

  llvm37::LLVM37Context& Ctx = CGM.getLLVM37Context();
  uint32_t ImgAddrSpc =
    CGM.getContext().getTargetAddressSpace(LangAS::opencl_global);
  switch (cast<BuiltinType>(T)->getKind()) {
  default: 
    llvm37_unreachable("Unexpected opencl builtin type!");
    return nullptr;
  case BuiltinType::OCLImage1d:
    return llvm37::PointerType::get(llvm37::StructType::create(
                           Ctx, "opencl.image1d_t"), ImgAddrSpc);
  case BuiltinType::OCLImage1dArray:
    return llvm37::PointerType::get(llvm37::StructType::create(
                           Ctx, "opencl.image1d_array_t"), ImgAddrSpc);
  case BuiltinType::OCLImage1dBuffer:
    return llvm37::PointerType::get(llvm37::StructType::create(
                           Ctx, "opencl.image1d_buffer_t"), ImgAddrSpc);
  case BuiltinType::OCLImage2d:
    return llvm37::PointerType::get(llvm37::StructType::create(
                           Ctx, "opencl.image2d_t"), ImgAddrSpc);
  case BuiltinType::OCLImage2dArray:
    return llvm37::PointerType::get(llvm37::StructType::create(
                           Ctx, "opencl.image2d_array_t"), ImgAddrSpc);
  case BuiltinType::OCLImage3d:
    return llvm37::PointerType::get(llvm37::StructType::create(
                           Ctx, "opencl.image3d_t"), ImgAddrSpc);
  case BuiltinType::OCLSampler:
    return llvm37::IntegerType::get(Ctx, 32);
  case BuiltinType::OCLEvent:
    return llvm37::PointerType::get(llvm37::StructType::create(
                           Ctx, "opencl.event_t"), 0);
  }
}

#endif // HLSL Change
