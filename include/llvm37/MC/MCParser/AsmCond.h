//===- AsmCond.h - Assembly file conditional assembly  ----------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_MC_MCPARSER_ASMCOND_H
#define LLVM37_MC_MCPARSER_ASMCOND_H

namespace llvm37 {

/// AsmCond - Class to support conditional assembly
///
/// The conditional assembly feature (.if, .else, .elseif and .endif) is
/// implemented with AsmCond that tells us what we are in the middle of 
/// processing.  Ignore can be either true or false.  When true we are ignoring
/// the block of code in the middle of a conditional.

class AsmCond {
public:
  enum ConditionalAssemblyType {
    NoCond,     // no conditional is being processed
    IfCond,     // inside if conditional
    ElseIfCond, // inside elseif conditional
    ElseCond    // inside else conditional
  };

  ConditionalAssemblyType TheCond;
  bool CondMet;
  bool Ignore;

  AsmCond() : TheCond(NoCond), CondMet(false), Ignore(false) {}
};

} // end namespace llvm37

#endif
