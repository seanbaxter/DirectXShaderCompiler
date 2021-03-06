//===- Error.h - system_error extensions for llvm37-cxxdump -------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This declares a new error_category for the llvm37-cxxdump tool.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_TOOLS_LLVM37_CXXDUMP_ERROR_H
#define LLVM37_TOOLS_LLVM37_CXXDUMP_ERROR_H

#include <system_error>

namespace llvm37 {
const std::error_category &cxxdump_category();

enum class cxxdump_error {
  success = 0,
  file_not_found,
  unrecognized_file_format,
};

inline std::error_code make_error_code(cxxdump_error e) {
  return std::error_code(static_cast<int>(e), cxxdump_category());
}

} // namespace llvm37

namespace std {
template <>
struct is_error_code_enum<llvm37::cxxdump_error> : std::true_type {};
}

#endif
