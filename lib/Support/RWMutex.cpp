//===- RWMutex.cpp - Reader/Writer Mutual Exclusion Lock --------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the llvm37::sys::RWMutex class.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Config/config.h"
#include "llvm37/Support/RWMutex.h"
#include <cstring>

//===----------------------------------------------------------------------===//
//=== WARNING: Implementation here must contain only TRULY operating system
//===          independent code.
//===----------------------------------------------------------------------===//

#if !defined(LLVM37_ENABLE_THREADS) || LLVM37_ENABLE_THREADS == 0
// Define all methods as no-ops if threading is explicitly disabled
namespace llvm37 {
using namespace sys;
RWMutexImpl::RWMutexImpl() { }
RWMutexImpl::~RWMutexImpl() { }
bool RWMutexImpl::reader_acquire() { return true; }
bool RWMutexImpl::reader_release() { return true; }
bool RWMutexImpl::writer_acquire() { return true; }
bool RWMutexImpl::writer_release() { return true; }
}
#else

#if defined(HAVE_PTHREAD_H) && defined(HAVE_PTHREAD_RWLOCK_INIT)

#include <cassert>
#include <pthread.h>
#include <stdlib.h>

namespace llvm37 {
using namespace sys;

// Construct a RWMutex using pthread calls
RWMutexImpl::RWMutexImpl()
  : data_(nullptr)
{
  // Declare the pthread_rwlock data structures
  pthread_rwlock_t* rwlock =
    static_cast<pthread_rwlock_t*>(malloc(sizeof(pthread_rwlock_t)));

#ifdef __APPLE__
  // Workaround a bug/mis-feature in Darwin's pthread_rwlock_init.
  bzero(rwlock, sizeof(pthread_rwlock_t));
#endif

  // Initialize the rwlock
  int errorcode = pthread_rwlock_init(rwlock, nullptr);
  (void)errorcode;
  assert(errorcode == 0);

  // Assign the data member
  data_ = rwlock;
}

// Destruct a RWMutex
RWMutexImpl::~RWMutexImpl()
{
  pthread_rwlock_t* rwlock = static_cast<pthread_rwlock_t*>(data_);
  assert(rwlock != nullptr);
  pthread_rwlock_destroy(rwlock);
  free(rwlock);
}

bool
RWMutexImpl::reader_acquire()
{
  pthread_rwlock_t* rwlock = static_cast<pthread_rwlock_t*>(data_);
  assert(rwlock != nullptr);

  int errorcode = pthread_rwlock_rdlock(rwlock);
  return errorcode == 0;
}

bool
RWMutexImpl::reader_release()
{
  pthread_rwlock_t* rwlock = static_cast<pthread_rwlock_t*>(data_);
  assert(rwlock != nullptr);

  int errorcode = pthread_rwlock_unlock(rwlock);
  return errorcode == 0;
}

bool
RWMutexImpl::writer_acquire()
{
  pthread_rwlock_t* rwlock = static_cast<pthread_rwlock_t*>(data_);
  assert(rwlock != nullptr);

  int errorcode = pthread_rwlock_wrlock(rwlock);
  return errorcode == 0;
}

bool
RWMutexImpl::writer_release()
{
  pthread_rwlock_t* rwlock = static_cast<pthread_rwlock_t*>(data_);
  assert(rwlock != nullptr);

  int errorcode = pthread_rwlock_unlock(rwlock);
  return errorcode == 0;
}

}

#elif defined(LLVM37_ON_UNIX)
#include "Unix/RWMutex.inc"
#elif defined( LLVM37_ON_WIN32)
#include "Windows/RWMutex.inc"
#else
#warning Neither LLVM37_ON_UNIX nor LLVM37_ON_WIN32 was set in Support/Mutex.cpp
#endif
#endif
