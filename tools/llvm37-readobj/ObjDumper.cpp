//===-- ObjDumper.cpp - Base dumper class -----------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file implements ObjDumper.
///
//===----------------------------------------------------------------------===//

#include "ObjDumper.h"
#include "Error.h"
#include "StreamWriter.h"
#include "llvm37/ADT/StringRef.h"
#include "llvm37/Object/ObjectFile.h"
#include "llvm37/Support/raw_ostream.h"

namespace llvm37 {

ObjDumper::ObjDumper(StreamWriter& Writer)
  : W(Writer) {
}

ObjDumper::~ObjDumper() {
}

} // namespace llvm37
