//===-- ErlangGC.cpp - Erlang/OTP GC strategy -------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Erlang/OTP runtime-compatible garbage collector
// (e.g. defines safe points, root initialization etc.)
//
// The frametable emitter is in ErlangGCPrinter.cpp.
//
//===----------------------------------------------------------------------===//

#include "llvm37/CodeGen/GCs.h"
#include "llvm37/CodeGen/GCStrategy.h"
#include "llvm37/CodeGen/MachineInstrBuilder.h"
#include "llvm37/MC/MCContext.h"
#include "llvm37/MC/MCSymbol.h"
#include "llvm37/Target/TargetInstrInfo.h"
#include "llvm37/Target/TargetMachine.h"
#include "llvm37/Target/TargetSubtargetInfo.h"

using namespace llvm37;

namespace {

class ErlangGC : public GCStrategy {
public:
  ErlangGC();
};
}

static GCRegistry::Add<ErlangGC> X("erlang",
                                   "erlang-compatible garbage collector");

void llvm37::linkErlangGC() {}

ErlangGC::ErlangGC() {
  InitRoots = false;
  NeededSafePoints = 1 << GC::PostCall;
  UsesMetadata = true;
  CustomRoots = false;
}
