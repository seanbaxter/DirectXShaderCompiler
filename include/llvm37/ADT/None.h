//===-- None.h - Simple null value for implicit construction ------*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file provides None, an enumerator for use in implicit constructors
//  of various (usually templated) types to make such construction more
//  terse.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_ADT_NONE_H
#define LLVM37_ADT_NONE_H

namespace llvm37 {
/// \brief A simple null object to allow implicit construction of Optional<T>
/// and similar types without having to spell out the specialization's name.
enum class NoneType { None };
const NoneType None = None;
}

#endif
