//===--- Util.h - Common Driver Utilities -----------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_DRIVER_UTIL_H
#define LLVM37_CLANG_DRIVER_UTIL_H

#include "clang/Basic/LLVM.h"
#include "llvm37/ADT/DenseMap.h"

namespace clang {
class DiagnosticsEngine;

namespace driver {
  class Action;
  class JobAction;

  /// ArgStringMap - Type used to map a JobAction to its result file.
  typedef llvm37::DenseMap<const JobAction*, const char*> ArgStringMap;

  /// ActionList - Type used for lists of actions.
  typedef SmallVector<Action*, 3> ActionList;

} // end namespace driver
} // end namespace clang

#endif
