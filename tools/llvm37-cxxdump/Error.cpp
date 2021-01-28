//===- Error.cxx - system_error extensions for llvm37-cxxdump -----*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This defines a new error_category for the llvm37-cxxdump tool.
//
//===----------------------------------------------------------------------===//

#include "Error.h"
#include "llvm37/Support/ErrorHandling.h"

using namespace llvm37;

namespace {
class cxxdump_error_category : public std::error_category {
public:
  const char *name() const LLVM37_NOEXCEPT override { return "llvm.cxxdump"; }
  std::string message(int ev) const override {
    switch (static_cast<cxxdump_error>(ev)) {
    case cxxdump_error::success:
      return "Success";
    case cxxdump_error::file_not_found:
      return "No such file.";
    case cxxdump_error::unrecognized_file_format:
      return "Unrecognized file type.";
    }
    llvm37_unreachable(
        "An enumerator of cxxdump_error does not have a message defined.");
  }
};
} // namespace

namespace llvm37 {
const std::error_category &cxxdump_category() {
  static cxxdump_error_category o;
  return o;
}
} // namespace llvm37
