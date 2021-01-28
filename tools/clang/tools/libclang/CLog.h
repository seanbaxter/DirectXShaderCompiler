//===- CLog.h - Logging Interface -------------------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_TOOLS_LIBCLANG_CLOG_H
#define LLVM37_CLANG_TOOLS_LIBCLANG_CLOG_H

#include "clang-c/Index.h"
#include "clang/Basic/LLVM.h"
#include "llvm37/ADT/IntrusiveRefCntPtr.h"
#include "llvm37/ADT/SmallString.h"
#include "llvm37/ADT/StringRef.h"
#include "llvm37/Support/Compiler.h"
#include "llvm37/Support/raw_ostream.h"
#include <string>

namespace llvm37 {
class format_object_base;
}

namespace clang {
  class FileEntry;

namespace cxindex {

class Logger;
typedef IntrusiveRefCntPtr<Logger> LogRef;

/// \brief Collects logging output and writes it to stderr when it's destructed.
/// Common use case:
/// \code
///   if (LogRef Log = Logger::make(__func__)) {
///     *Log << "stuff";
///   }
/// \endcode
class Logger : public RefCountedBase<Logger> {
  std::string Name;
  bool Trace;
  SmallString<64> Msg;
  llvm37::raw_svector_ostream LogOS;
public:
  static const char *getEnvVar() {
    static const char *sCachedVar = ::getenv("LIBCLANG_LOGGING");
    return sCachedVar;
  }
  static bool isLoggingEnabled() { return getEnvVar() != nullptr; }
  static bool isStackTracingEnabled() {
    if (const char *EnvOpt = Logger::getEnvVar())
      return llvm37::StringRef(EnvOpt) == "2";
    return false;
  }
  static LogRef make(llvm37::StringRef name,
                     bool trace = isStackTracingEnabled()) {
    if (isLoggingEnabled())
      return new Logger(name, trace);
    return nullptr;
  }

  explicit Logger(llvm37::StringRef name, bool trace)
    : Name(name), Trace(trace), LogOS(Msg) { }
  ~Logger();

  Logger &operator<<(CXTranslationUnit);
  Logger &operator<<(const FileEntry *FE);
  Logger &operator<<(CXCursor cursor);
  Logger &operator<<(CXSourceLocation);
  Logger &operator<<(CXSourceRange);
  Logger &operator<<(CXString);
  Logger &operator<<(llvm37::StringRef Str) { LogOS << Str; return *this; }
  Logger &operator<<(const char *Str) {
    if (Str)
      LogOS << Str;
    return *this;
  }
  Logger &operator<<(unsigned long N) { LogOS << N; return *this; }
  Logger &operator<<(long N) { LogOS << N ; return *this; }
  Logger &operator<<(unsigned int N) { LogOS << N; return *this; }
  Logger &operator<<(int N) { LogOS << N; return *this; }
  Logger &operator<<(char C) { LogOS << C; return *this; }
  Logger &operator<<(unsigned char C) { LogOS << C; return *this; }
  Logger &operator<<(signed char C) { LogOS << C; return *this; }
  Logger &operator<<(const llvm37::format_object_base &Fmt);
};

}
}

/// \brief Macros to automate common uses of Logger. Like this:
/// \code
///   LOG_FUNC_SECTION {
///     *Log << "blah";
///   }
/// \endcode
#define LOG_SECTION(NAME) \
    if (clang::cxindex::LogRef Log = clang::cxindex::Logger::make(NAME))
#define LOG_FUNC_SECTION LOG_SECTION(LLVM37_FUNCTION_NAME)

#endif
