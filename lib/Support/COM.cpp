//===-- COM.cpp - Implement COM utility classes -----------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file implements utility classes related to COM.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Support/COM.h"

#include "llvm37/Config/config.h"

// Include the platform-specific parts of this class.
#ifdef LLVM37_ON_UNIX
#include "Unix/COM.inc"
#elif LLVM37_ON_WIN32
#include "Windows/COM.inc"
#endif
