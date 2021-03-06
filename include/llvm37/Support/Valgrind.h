//===- llvm37/Support/Valgrind.h - Communication with Valgrind -----*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Methods for communicating with a valgrind instance this program is running
// under.  These are all no-ops unless LLVM37 was configured on a system with the
// valgrind headers installed and valgrind is controlling this process.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_SUPPORT_VALGRIND_H
#define LLVM37_SUPPORT_VALGRIND_H

#include "llvm37/Config/llvm-config.h"
#include "llvm37/Support/Compiler.h"
#include <stddef.h>

#if LLVM37_ENABLE_THREADS != 0 && !defined(NDEBUG)
// tsan (Thread Sanitizer) is a valgrind-based tool that detects these exact
// functions by name.
extern "C" {
void AnnotateHappensAfter(const char *file, int line, const volatile void *cv);
void AnnotateHappensBefore(const char *file, int line, const volatile void *cv);
void AnnotateIgnoreWritesBegin(const char *file, int line);
void AnnotateIgnoreWritesEnd(const char *file, int line);
}
#endif

namespace llvm37 {
namespace sys {
  // True if Valgrind is controlling this process.
  bool RunningOnValgrind();

  // Discard valgrind's translation of code in the range [Addr .. Addr + Len).
  // Otherwise valgrind may continue to execute the old version of the code.
  void ValgrindDiscardTranslations(const void *Addr, size_t Len);

#if LLVM37_ENABLE_THREADS != 0 && !defined(NDEBUG)
  // Thread Sanitizer is a valgrind tool that finds races in code.
  // See http://code.google.com/p/data-race-test/wiki/DynamicAnnotations .

  // This marker is used to define a happens-before arc. The race detector will
  // infer an arc from the begin to the end when they share the same pointer
  // argument.
  #define LLVM37_TsanHappensBefore(cv) \
    AnnotateHappensBefore(__FILE__, __LINE__, cv)

  // This marker defines the destination of a happens-before arc.
  #define LLVM37_TsanHappensAfter(cv) \
    AnnotateHappensAfter(__FILE__, __LINE__, cv)

  // Ignore any races on writes between here and the next LLVM37_TsanIgnoreWritesEnd.
  #define LLVM37_TsanIgnoreWritesBegin() \
    AnnotateIgnoreWritesBegin(__FILE__, __LINE__)

  // Resume checking for racy writes.
  #define LLVM37_TsanIgnoreWritesEnd() \
    AnnotateIgnoreWritesEnd(__FILE__, __LINE__)
#else
  #define LLVM37_TsanHappensBefore(cv)
  #define LLVM37_TsanHappensAfter(cv)
  #define LLVM37_TsanIgnoreWritesBegin()
  #define LLVM37_TsanIgnoreWritesEnd()
#endif
}
}

#endif
