//===- Error.h - system_error extensions for llvm37-readobj -------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This declares a new error_category for the llvm37-readobj tool.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_TOOLS_LLVM37_READOBJ_ERROR_H
#define LLVM37_TOOLS_LLVM37_READOBJ_ERROR_H

#include <system_error>

namespace llvm37 {
const std::error_category &readobj_category();

enum class readobj_error {
  success = 0,
  file_not_found,
  unsupported_file_format,
  unrecognized_file_format,
  unsupported_obj_file_format,
  unknown_symbol
};

inline std::error_code make_error_code(readobj_error e) {
  return std::error_code(static_cast<int>(e), readobj_category());
}

} // namespace llvm37

namespace std {
template <> struct is_error_code_enum<llvm37::readobj_error> : std::true_type {};
}

#endif
