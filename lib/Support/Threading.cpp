//===-- llvm37/Support/Threading.cpp- Control multithreading mode --*- C++ -*-==//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines helper functions for running LLVM37 in a multi-threaded
// environment.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Support/Threading.h"
#include "llvm37/Config/config.h"
#include "llvm37/Support/Atomic.h"
#include "llvm37/Support/Mutex.h"
#include <cassert>

using namespace llvm37;

bool llvm37::llvm37_is_multithreaded() {
#if LLVM37_ENABLE_THREADS != 0
  return true;
#else
  return false;
#endif
}

#if LLVM37_ENABLE_THREADS != 0 && defined(HAVE_PTHREAD_H)
#include <pthread.h>

struct ThreadInfo {
  void (*UserFn)(void *);
  void *UserData;
};
static void *ExecuteOnThread_Dispatch(void *Arg) {
  ThreadInfo *TI = reinterpret_cast<ThreadInfo*>(Arg);
  TI->UserFn(TI->UserData);
  return nullptr;
}

void llvm37::llvm37_execute_on_thread(void (*Fn)(void*), void *UserData,
                                  unsigned RequestedStackSize) {
  ThreadInfo Info = { Fn, UserData };
  pthread_attr_t Attr;
  pthread_t Thread;

  // Construct the attributes object.
  if (::pthread_attr_init(&Attr) != 0)
    return;

  // Set the requested stack size, if given.
  if (RequestedStackSize != 0) {
    if (::pthread_attr_setstacksize(&Attr, RequestedStackSize) != 0)
      goto error;
  }

  // Construct and execute the thread.
  if (::pthread_create(&Thread, &Attr, ExecuteOnThread_Dispatch, &Info) != 0)
    goto error;

  // Wait for the thread and clean up.
  ::pthread_join(Thread, nullptr);

 error:
  ::pthread_attr_destroy(&Attr);
}
#elif LLVM37_ENABLE_THREADS!=0 && defined(LLVM37_ON_WIN32)
#include "Windows/WindowsSupport.h"
#include <process.h>

struct ThreadInfo {
  void (*func)(void*);
  void *param;
};

static unsigned __stdcall ThreadCallback(void *param) {
  struct ThreadInfo *info = reinterpret_cast<struct ThreadInfo *>(param);
  info->func(info->param);

  return 0;
}

void llvm37::llvm37_execute_on_thread(void (*Fn)(void*), void *UserData,
                                  unsigned RequestedStackSize) {
  struct ThreadInfo param = { Fn, UserData };

  HANDLE hThread = (HANDLE)::_beginthreadex(NULL,
                                            RequestedStackSize, ThreadCallback,
                                            &param, 0, NULL);

  if (hThread) {
    // We actually don't care whether the wait succeeds or fails, in
    // the same way we don't care whether the pthread_join call succeeds
    // or fails.  There's not much we could do if this were to fail. But
    // on success, this call will wait until the thread finishes executing
    // before returning.
    (void)::WaitForSingleObject(hThread, INFINITE);
    ::CloseHandle(hThread);
  }
}
#else
// Support for non-Win32, non-pthread implementation.
void llvm37::llvm37_execute_on_thread(void (*Fn)(void*), void *UserData,
                                  unsigned RequestedStackSize) {
  (void) RequestedStackSize;
  Fn(UserData);
}

#endif
