//===-- Vectorize.cpp -----------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements common infrastructure for libLLVMVectorizeOpts.a, which
// implements several vectorization transformations over the LLVM intermediate
// representation, including the C bindings for that library.
//
//===----------------------------------------------------------------------===//

#if HLSL_VECTORIZATION_ENABLED // HLSL Change - don't build vectorization passes

#include "llvm37/Transforms/Vectorize.h"
#include "llvm-c/Initialization.h"
#include "llvm-c/Transforms/Vectorize.h"
#include "llvm37/Analysis/Passes.h"
#include "llvm37/IR/Verifier.h"
#include "llvm37/InitializePasses.h"
#include "llvm37/IR/LegacyPassManager.h"

using namespace llvm37;

/// initializeVectorizationPasses - Initialize all passes linked into the
/// Vectorization library.
void llvm37::initializeVectorization(PassRegistry &Registry) {
  initializeBBVectorizePass(Registry);
  initializeLoopVectorizePass(Registry);
  initializeSLPVectorizerPass(Registry);
}

void LLVMInitializeVectorization(LLVMPassRegistryRef R) {
  initializeVectorization(*unwrap(R));
}

void LLVMAddBBVectorizePass(LLVMPassManagerRef PM) {
  unwrap(PM)->add(createBBVectorizePass());
}

void LLVMAddLoopVectorizePass(LLVMPassManagerRef PM) {
  unwrap(PM)->add(createLoopVectorizePass());
}

void LLVMAddSLPVectorizePass(LLVMPassManagerRef PM) {
  unwrap(PM)->add(createSLPVectorizerPass());
}

#else

namespace llvm37 {
class PassRegistry;

void initializeVectorization(PassRegistry &) {}

}

#endif // HLSL Change - don't build vectorization passes
