//===- MIRPrinter.h - MIR serialization format printer --------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the functions that print out the LLVM37 IR and the machine
// functions using the MIR serialization format.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_LIB_CODEGEN_MIRPRINTER_H
#define LLVM37_LIB_CODEGEN_MIRPRINTER_H

namespace llvm37 {

class MachineFunction;
class Module;
class raw_ostream;

/// Print LLVM37 IR using the MIR serialization format to the given output stream.
void printMIR(raw_ostream &OS, const Module &M);

/// Print a machine function using the MIR serialization format to the given
/// output stream.
void printMIR(raw_ostream &OS, const MachineFunction &MF);

} // end namespace llvm37

#endif
