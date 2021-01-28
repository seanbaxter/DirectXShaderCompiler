//===- Error.cpp - system_error extensions for llvm37-readobj -----*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This defines a new error_category for the llvm37-readobj tool.
//
//===----------------------------------------------------------------------===//

#include "Error.h"
#include "llvm37/Support/ErrorHandling.h"

using namespace llvm37;

namespace {
class _readobj_error_category : public std::error_category {
public:
  const char* name() const LLVM37_NOEXCEPT override;
  std::string message(int ev) const override;
};
} // namespace

const char *_readobj_error_category::name() const LLVM37_NOEXCEPT {
  return "llvm.readobj";
}

std::string _readobj_error_category::message(int EV) const {
  switch (static_cast<readobj_error>(EV)) {
  case readobj_error::success: return "Success";
  case readobj_error::file_not_found:
    return "No such file.";
  case readobj_error::unsupported_file_format:
    return "The file was not recognized as a valid object file.";
  case readobj_error::unrecognized_file_format:
    return "Unrecognized file type.";
  case readobj_error::unsupported_obj_file_format:
    return "Unsupported object file format.";
  case readobj_error::unknown_symbol:
    return "Unknown symbol.";
  }
  llvm37_unreachable("An enumerator of readobj_error does not have a message "
                   "defined.");
}

namespace llvm37 {
const std::error_category &readobj_category() {
  static _readobj_error_category o;
  return o;
}
} // namespace llvm37
