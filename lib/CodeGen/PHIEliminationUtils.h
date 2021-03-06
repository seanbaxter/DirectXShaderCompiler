//=- PHIEliminationUtils.h - Helper functions for PHI elimination -*- C++ -*-=//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_LIB_CODEGEN_PHIELIMINATIONUTILS_H
#define LLVM37_LIB_CODEGEN_PHIELIMINATIONUTILS_H

#include "llvm37/CodeGen/MachineBasicBlock.h"

namespace llvm37 {
    /// findPHICopyInsertPoint - Find a safe place in MBB to insert a copy from
    /// SrcReg when following the CFG edge to SuccMBB. This needs to be after
    /// any def of SrcReg, but before any subsequent point where control flow
    /// might jump out of the basic block.
    MachineBasicBlock::iterator
    findPHICopyInsertPoint(MachineBasicBlock* MBB, MachineBasicBlock* SuccMBB,
                           unsigned SrcReg);
}

#endif
