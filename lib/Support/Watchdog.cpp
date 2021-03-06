//===---- Watchdog.cpp - Implement Watchdog ---------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file implements the Watchdog class.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Support/Watchdog.h"
#include "llvm37/Config/config.h"

// Include the platform-specific parts of this class.
#ifdef LLVM37_ON_UNIX
#include "Unix/Watchdog.inc"
#endif
#ifdef LLVM37_ON_WIN32
#include "Windows/Watchdog.inc"
#endif
