//===- llvm/LibDriver/LibDriver.h - lib.exe-compatible driver ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Defines an interface to a lib.exe-compatible driver that also understands
// bitcode files. Used by llvm-lib and lld-link2 /lib.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_LIBDRIVER_LIBDRIVER_H
#define LLVM37_LIBDRIVER_LIBDRIVER_H

#include "llvm37/ADT/ArrayRef.h"

namespace llvm37 {

int libDriverMain(llvm37::ArrayRef<const char*> ARgs);

}

#endif
