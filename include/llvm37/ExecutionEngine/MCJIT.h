//===-- MCJIT.h - MC-Based Just-In-Time Execution Engine --------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file forces the MCJIT to link in on certain operating systems.
// (Windows).
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_EXECUTIONENGINE_MCJIT_H
#define LLVM37_EXECUTIONENGINE_MCJIT_H

#include "llvm37/ExecutionEngine/ExecutionEngine.h"
#include <cstdlib>

extern "C" void LLVM37LinkInMCJIT();

namespace {
  struct ForceMCJITLinking {
    ForceMCJITLinking() {
      // We must reference MCJIT in such a way that compilers will not
      // delete it all as dead code, even with whole program optimization,
      // yet is effectively a NO-OP. As the compiler isn't smart enough
      // to know that getenv() never returns -1, this will do the job.
      if (std::getenv("bar") != (char*) -1)
        return;

      LLVM37LinkInMCJIT();
    }
  } ForceMCJITLinking;
}

#endif
