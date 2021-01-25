//== IntrusiveRefCntPtr.cpp - Smart Refcounting Pointer ----------*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/ADT/IntrusiveRefCntPtr.h"

using namespace llvm37;

void RefCountedBaseVPTR::anchor() { }
