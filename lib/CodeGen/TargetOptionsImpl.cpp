//===-- TargetOptionsImpl.cpp - Options that apply to all targets ----------==//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the methods in the TargetOptions.
//
//===----------------------------------------------------------------------===//

#include "llvm37/IR/Function.h"
#include "llvm37/IR/Module.h"
#include "llvm37/CodeGen/MachineFrameInfo.h"
#include "llvm37/CodeGen/MachineFunction.h"
#include "llvm37/Target/TargetFrameLowering.h"
#include "llvm37/Target/TargetOptions.h"
#include "llvm37/Target/TargetSubtargetInfo.h"
using namespace llvm37;

/// DisableFramePointerElim - This returns true if frame pointer elimination
/// optimization should be disabled for the given machine function.
bool TargetOptions::DisableFramePointerElim(const MachineFunction &MF) const {
  // Check to see if we should eliminate all frame pointers.
  if (MF.getSubtarget().getFrameLowering()->noFramePointerElim(MF))
    return true;

  // Check to see if we should eliminate non-leaf frame pointers.
  if (MF.getFunction()->hasFnAttribute("no-frame-pointer-elim-non-leaf"))
    return MF.getFrameInfo()->hasCalls();

  return false;
}

/// LessPreciseFPMAD - This flag return true when -enable-fp-mad option
/// is specified on the command line.  When this flag is off(default), the
/// code generator is not allowed to generate mad (multiply add) if the
/// result is "less precise" than doing those operations individually.
bool TargetOptions::LessPreciseFPMAD() const {
  return UnsafeFPMath || LessPreciseFPMADOption;
}

/// HonorSignDependentRoundingFPMath - Return true if the codegen must assume
/// that the rounding mode of the FPU can change from its default.
bool TargetOptions::HonorSignDependentRoundingFPMath() const {
  return !UnsafeFPMath && HonorSignDependentRoundingFPMathOption;
}
