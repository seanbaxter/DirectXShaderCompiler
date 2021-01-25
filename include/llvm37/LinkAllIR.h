//===----- LinkAllIR.h - Reference All VMCore Code --------------*- C++ -*-===//
//
//                      The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This header file pulls in all the object modules of the VMCore library so
// that tools like llc, opt, and lli can ensure they are linked with all symbols
// from libVMCore.a It should only be used from a tool's main program.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_LINKALLIR_H
#define LLVM37_LINKALLIR_H

#include "llvm37/IR/InlineAsm.h"
#include "llvm37/IR/Instructions.h"
#include "llvm37/IR/IntrinsicInst.h"
#include "llvm37/IR/LLVMContext.h"
#include "llvm37/IR/Module.h"
#include "llvm37/IR/Verifier.h"
#include "llvm37/Support/Dwarf.h"
#include "llvm37/Support/DynamicLibrary.h"
#include "llvm37/Support/MathExtras.h"
#include "llvm37/Support/Memory.h"
#include "llvm37/Support/Mutex.h"
#include "llvm37/Support/Path.h"
#include "llvm37/Support/Process.h"
#include "llvm37/Support/Program.h"
#include "llvm37/Support/Signals.h"
#include "llvm37/Support/TimeValue.h"
#include <cstdlib>

namespace {
  struct ForceVMCoreLinking {
    ForceVMCoreLinking() {
      // We must reference VMCore in such a way that compilers will not
      // delete it all as dead code, even with whole program optimization,
      // yet is effectively a NO-OP. As the compiler isn't smart enough
      // to know that getenv() never returns -1, this will do the job.
      if (std::getenv("bar") != (char*) -1)
        return;
      (void)new llvm37::Module("", llvm37::getGlobalContext());
      (void)new llvm37::UnreachableInst(llvm37::getGlobalContext());
      (void)    llvm37::createVerifierPass(); 
    }
  } ForceVMCoreLinking;
}

#endif
