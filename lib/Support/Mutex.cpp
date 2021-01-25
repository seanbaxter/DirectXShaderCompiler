//===- Mutex.cpp - Mutual Exclusion Lock ------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the llvm37::sys::Mutex class.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Config/config.h"
#include "llvm37/Support/Mutex.h"

//===----------------------------------------------------------------------===//
//=== WARNING: Implementation here must contain only TRULY operating system
//===          independent code.
//===----------------------------------------------------------------------===//

#if !defined(LLVM37_ENABLE_THREADS) || LLVM37_ENABLE_THREADS == 0
// Define all methods as no-ops if threading is explicitly disabled
namespace llvm37 {
using namespace sys;
MutexImpl::MutexImpl( bool recursive) { }
MutexImpl::~MutexImpl() { }
bool MutexImpl::acquire() { return true; }
bool MutexImpl::release() { return true; }
bool MutexImpl::tryacquire() { return true; }
}
#else

#if defined(HAVE_PTHREAD_H) && defined(HAVE_PTHREAD_MUTEX_LOCK)

#include <cassert>
#include <pthread.h>
#include <stdlib.h>

namespace llvm37 {
using namespace sys;

// Construct a Mutex using pthread calls
MutexImpl::MutexImpl( bool recursive)
  : data_(nullptr)
{
  // Declare the pthread_mutex data structures
  pthread_mutex_t* mutex =
    static_cast<pthread_mutex_t*>(malloc(sizeof(pthread_mutex_t)));
  pthread_mutexattr_t attr;

  // Initialize the mutex attributes
  int errorcode = pthread_mutexattr_init(&attr);
  assert(errorcode == 0); (void)errorcode;

  // Initialize the mutex as a recursive mutex, if requested, or normal
  // otherwise.
  int kind = ( recursive  ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_NORMAL );
  errorcode = pthread_mutexattr_settype(&attr, kind);
  assert(errorcode == 0);

  // Initialize the mutex
  errorcode = pthread_mutex_init(mutex, &attr);
  assert(errorcode == 0);

  // Destroy the attributes
  errorcode = pthread_mutexattr_destroy(&attr);
  assert(errorcode == 0);

  // Assign the data member
  data_ = mutex;
}

// Destruct a Mutex
MutexImpl::~MutexImpl()
{
  pthread_mutex_t* mutex = static_cast<pthread_mutex_t*>(data_);
  assert(mutex != nullptr);
  pthread_mutex_destroy(mutex);
  free(mutex);
}

bool
MutexImpl::acquire()
{
  pthread_mutex_t* mutex = static_cast<pthread_mutex_t*>(data_);
  assert(mutex != nullptr);

  int errorcode = pthread_mutex_lock(mutex);
  return errorcode == 0;
}

bool
MutexImpl::release()
{
  pthread_mutex_t* mutex = static_cast<pthread_mutex_t*>(data_);
  assert(mutex != nullptr);

  int errorcode = pthread_mutex_unlock(mutex);
  return errorcode == 0;
}

bool
MutexImpl::tryacquire()
{
  pthread_mutex_t* mutex = static_cast<pthread_mutex_t*>(data_);
  assert(mutex != nullptr);

  int errorcode = pthread_mutex_trylock(mutex);
  return errorcode == 0;
}

}

#elif defined(LLVM37_ON_UNIX)
#include "Unix/Mutex.inc"
#elif defined( LLVM37_ON_WIN32)
#include "Windows/Mutex.inc"
#else
#warning Neither LLVM37_ON_UNIX nor LLVM37_ON_WIN32 was set in Support/Mutex.cpp
#endif
#endif
