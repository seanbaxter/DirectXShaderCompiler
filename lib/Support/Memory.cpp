//===- Memory.cpp - Memory Handling Support ---------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines some helpful functions for allocating memory and dealing
// with memory mapped files
//
//===----------------------------------------------------------------------===//

#include "llvm37/Support/Memory.h"
#include "llvm37/Config/config.h"
#include "llvm37/Support/Valgrind.h"

// Include the platform-specific parts of this class.
#ifdef LLVM37_ON_UNIX
#include "Unix/Memory.inc"
#endif
#ifdef LLVM37_ON_WIN32
#include "Windows/Memory.inc"
#endif
