//===-- MCTargetOptionsCommandFlags.h --------------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains machine code-specific flags that are shared between
// different command line tools.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_MC_MCTARGETOPTIONSCOMMANDFLAGS_H
#define LLVM37_MC_MCTARGETOPTIONSCOMMANDFLAGS_H

#include "llvm37/MC/MCTargetOptions.h"
#include "llvm37/Support/CommandLine.h"
using namespace llvm37;

cl::opt<MCTargetOptions::AsmInstrumentation> AsmInstrumentation(
    "asm-instrumentation", cl::desc("Instrumentation of inline assembly and "
                                    "assembly source files"),
    cl::init(MCTargetOptions::AsmInstrumentationNone),
    cl::values(clEnumValN(MCTargetOptions::AsmInstrumentationNone, "none",
                          "no instrumentation at all"),
               clEnumValN(MCTargetOptions::AsmInstrumentationAddress, "address",
                          "instrument instructions with memory arguments"),
               clEnumValEnd));

cl::opt<bool> RelaxAll("mc-relax-all",
                       cl::desc("When used with filetype=obj, "
                                "relax all fixups in the emitted object file"));

cl::opt<int> DwarfVersion("dwarf-version", cl::desc("Dwarf version"),
                          cl::init(0));

cl::opt<bool> ShowMCInst("asm-show-inst",
                         cl::desc("Emit internal instruction representation to "
                                  "assembly file"));

cl::opt<std::string>
ABIName("target-abi", cl::Hidden,
        cl::desc("The name of the ABI to be targeted from the backend."),
        cl::init(""));

static inline MCTargetOptions InitMCTargetOptionsFromFlags() {
  MCTargetOptions Options;
  Options.SanitizeAddress =
      (AsmInstrumentation == MCTargetOptions::AsmInstrumentationAddress);
  Options.MCRelaxAll = RelaxAll;
  Options.DwarfVersion = DwarfVersion;
  Options.ShowMCInst = ShowMCInst;
  Options.ABIName = ABIName;
  return Options;
}

#endif
