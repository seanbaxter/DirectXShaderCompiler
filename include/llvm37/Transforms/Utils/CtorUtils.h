//===- CtorUtils.h - Helpers for working with global_ctors ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines functions that are used to process llvm.global_ctors.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_TRANSFORMS_UTILS_CTORUTILS_H
#define LLVM37_TRANSFORMS_UTILS_CTORUTILS_H

#include "llvm37/ADT/STLExtras.h"

namespace llvm37 {

class GlobalVariable;
class Function;
class Module;

/// Call "ShouldRemove" for every entry in M's global_ctor list and remove the
/// entries for which it returns true.  Return true if anything changed.
bool optimizeGlobalCtorsList(Module &M,
                             function_ref<bool(Function *)> ShouldRemove);

} // End llvm namespace

#endif
