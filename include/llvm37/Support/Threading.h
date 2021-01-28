//===-- llvm37/Support/Threading.h - Control multithreading mode --*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares helper functions for running LLVM37 in a multi-threaded
// environment.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_SUPPORT_THREADING_H
#define LLVM37_SUPPORT_THREADING_H

namespace llvm37 {
  /// Returns true if LLVM37 is compiled with support for multi-threading, and
  /// false otherwise.
  bool llvm37_is_multithreaded();

  /// llvm37_execute_on_thread - Execute the given \p UserFn on a separate
  /// thread, passing it the provided \p UserData and waits for thread 
  /// completion.
  ///
  /// This function does not guarantee that the code will actually be executed
  /// on a separate thread or honoring the requested stack size, but tries to do
  /// so where system support is available.
  ///
  /// \param UserFn - The callback to execute.
  /// \param UserData - An argument to pass to the callback function.
  /// \param RequestedStackSize - If non-zero, a requested size (in bytes) for
  /// the thread stack.
  void llvm37_execute_on_thread(void (*UserFn)(void*), void *UserData,
                              unsigned RequestedStackSize = 0);
}

#endif
