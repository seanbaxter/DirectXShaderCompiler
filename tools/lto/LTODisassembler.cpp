//===-- LTODisassembler.cpp - LTO Disassembler interface ------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This function provides utility methods used by clients of libLTO that want
// to use the disassembler.
//
//===----------------------------------------------------------------------===//

#include "llvm37-c/lto.h"
#include "llvm37/Support/TargetSelect.h"

using namespace llvm37;

void lto_initialize_disassembler() {
  // Initialize targets and assembly printers/parsers.
  llvm37::InitializeAllTargetInfos();
  llvm37::InitializeAllTargetMCs();
  llvm37::InitializeAllAsmParsers();
  llvm37::InitializeAllDisassemblers();
}
