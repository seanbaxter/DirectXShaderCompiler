//===-- DxilTargetLowering.cpp - Implement the DxilTargetLowering class ---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Empty implementation of TargetLoweringBase::InstructionOpcodeToISD and
// TargetLoweringBase::getTypeLegalizationCost to make TargetTransformInfo
// compile.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Target/TargetLowering.h"

namespace llvm37 {

//===----------------------------------------------------------------------===//
//  TargetTransformInfo Helpers
//===----------------------------------------------------------------------===//

int TargetLoweringBase::InstructionOpcodeToISD(unsigned Opcode) const {
  return 0;
}

std::pair<unsigned, MVT>
TargetLoweringBase::getTypeLegalizationCost(const DataLayout &DL,
                                            Type *Ty) const {
  EVT MTy = getValueType(DL, Ty);
  unsigned Cost = 1;
  return std::make_pair(Cost, MTy.getSimpleVT());
}
}