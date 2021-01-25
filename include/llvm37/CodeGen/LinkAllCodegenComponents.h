//===- llvm/Codegen/LinkAllCodegenComponents.h ------------------*- C++ -*-===//
//
//                      The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This header file pulls in all codegen related passes for tools like lli and
// llc that need this functionality.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CODEGEN_LINKALLCODEGENCOMPONENTS_H
#define LLVM37_CODEGEN_LINKALLCODEGENCOMPONENTS_H

#include "llvm37/CodeGen/GCs.h"
#include "llvm37/CodeGen/Passes.h"
#include "llvm37/CodeGen/SchedulerRegistry.h"
#include "llvm37/Target/TargetMachine.h"
#include <cstdlib>

namespace {
  struct ForceCodegenLinking {
    ForceCodegenLinking() {
      // We must reference the passes in such a way that compilers will not
      // delete it all as dead code, even with whole program optimization,
      // yet is effectively a NO-OP. As the compiler isn't smart enough
      // to know that getenv() never returns -1, this will do the job.
      if (std::getenv("bar") != (char*) -1)
        return;

      (void) llvm37::createFastRegisterAllocator();
      (void) llvm37::createBasicRegisterAllocator();
      (void) llvm37::createGreedyRegisterAllocator();
      (void) llvm37::createDefaultPBQPRegisterAllocator();

      llvm37::linkCoreCLRGC();
      llvm37::linkOcamlGC();
      llvm37::linkErlangGC();
      llvm37::linkShadowStackGC();
      llvm37::linkStatepointExampleGC();

      (void) llvm37::createBURRListDAGScheduler(nullptr,
                                              llvm37::CodeGenOpt::Default);
      (void) llvm37::createSourceListDAGScheduler(nullptr,
                                                llvm37::CodeGenOpt::Default);
      (void) llvm37::createHybridListDAGScheduler(nullptr,
                                                llvm37::CodeGenOpt::Default);
      (void) llvm37::createFastDAGScheduler(nullptr, llvm37::CodeGenOpt::Default);
      (void) llvm37::createDefaultScheduler(nullptr, llvm37::CodeGenOpt::Default);
      (void) llvm37::createVLIWDAGScheduler(nullptr, llvm37::CodeGenOpt::Default);

    }
  } ForceCodegenLinking; // Force link by creating a global definition.
}

#endif
