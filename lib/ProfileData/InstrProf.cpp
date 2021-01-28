//=-- InstrProf.cpp - Instrumented profiling format support -----------------=//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains support for clang's instrumentation based PGO and
// coverage.
//
//===----------------------------------------------------------------------===//

#include "llvm37/ProfileData/InstrProf.h"
#include "llvm37/Support/ErrorHandling.h"
#include "llvm37/Support/ManagedStatic.h"

using namespace llvm37;

namespace {
class InstrProfErrorCategoryType : public std::error_category {
  const char *name() const LLVM37_NOEXCEPT override { return "llvm.instrprof"; }
  std::string message(int IE) const override {
    instrprof_error E = static_cast<instrprof_error>(IE);
    switch (E) {
    case instrprof_error::success:
      return "Success";
    case instrprof_error::eof:
      return "End of File";
    case instrprof_error::bad_magic:
      return "Invalid profile data (bad magic)";
    case instrprof_error::bad_header:
      return "Invalid profile data (file header is corrupt)";
    case instrprof_error::unsupported_version:
      return "Unsupported profiling format version";
    case instrprof_error::unsupported_hash_type:
      return "Unsupported profiling hash";
    case instrprof_error::too_large:
      return "Too much profile data";
    case instrprof_error::truncated:
      return "Truncated profile data";
    case instrprof_error::malformed:
      return "Malformed profile data";
    case instrprof_error::unknown_function:
      return "No profile data available for function";
    case instrprof_error::hash_mismatch:
      return "Function hash mismatch";
    case instrprof_error::count_mismatch:
      return "Function count mismatch";
    case instrprof_error::counter_overflow:
      return "Counter overflow";
    }
    llvm37_unreachable("A value of instrprof_error has no message.");
  }
};
}

static ManagedStatic<InstrProfErrorCategoryType> ErrorCategory;

const std::error_category &llvm37::instrprof_category() {
  return *ErrorCategory;
}
