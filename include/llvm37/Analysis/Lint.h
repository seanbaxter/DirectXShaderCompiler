//===-- llvm37/Analysis/Lint.h - LLVM37 IR Lint ---------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines lint interfaces that can be used for some sanity checking
// of input to the system, and for checking that transformations
// haven't done something bad. In contrast to the Verifier, the Lint checker
// checks for undefined behavior or constructions with likely unintended
// behavior.
//
// To see what specifically is checked, look at Lint.cpp
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_ANALYSIS_LINT_H
#define LLVM37_ANALYSIS_LINT_H

namespace llvm37 {

class FunctionPass;
class Module;
class Function;

/// @brief Create a lint pass.
///
/// Check a module or function.
FunctionPass *createLintPass();

/// @brief Check a module.
///
/// This should only be used for debugging, because it plays games with
/// PassManagers and stuff.
void lintModule(
  const Module &M    ///< The module to be checked
);

// lintFunction - Check a function.
void lintFunction(
  const Function &F  ///< The function to be checked
);

} // End llvm37 namespace

#endif
