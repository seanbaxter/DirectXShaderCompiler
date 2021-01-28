//===-- GVMaterializer.cpp - Base implementation for GV materializers -----===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Minimal implementation of the abstract interface for materializing
// GlobalValues.
//
//===----------------------------------------------------------------------===//

#include "llvm37/IR/GVMaterializer.h"
using namespace llvm37;

GVMaterializer::~GVMaterializer() {}
