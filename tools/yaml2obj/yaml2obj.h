//===--- yaml2obj.h - -------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
/// \file
/// \brief Common declarations for yaml2obj
//===----------------------------------------------------------------------===//
#ifndef LLVM37_TOOLS_YAML2OBJ_YAML2OBJ_H
#define LLVM37_TOOLS_YAML2OBJ_YAML2OBJ_H

namespace llvm37 {
class raw_ostream;
namespace yaml {
class Input;
}
}
int yaml2coff(llvm37::yaml::Input &YIn, llvm37::raw_ostream &Out);
int yaml2elf(llvm37::yaml::Input &YIn, llvm37::raw_ostream &Out);

#endif
