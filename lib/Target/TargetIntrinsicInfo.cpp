//===-- TargetIntrinsicInfo.cpp - Target Instruction Information ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the TargetIntrinsicInfo class.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Target/TargetIntrinsicInfo.h"
#include "llvm37/ADT/StringMap.h"
#include "llvm37/IR/Function.h"
using namespace llvm37;

TargetIntrinsicInfo::TargetIntrinsicInfo() {
}

TargetIntrinsicInfo::~TargetIntrinsicInfo() {
}

unsigned TargetIntrinsicInfo::getIntrinsicID(Function *F) const {
  const ValueName *ValName = F->getValueName();
  if (!ValName)
    return 0;
  return lookupName(ValName->getKeyData(), ValName->getKeyLength());
}
