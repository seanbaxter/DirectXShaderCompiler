//===- ThreadLocal.cpp - Thread Local Data ----------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the llvm37::sys::ThreadLocal class.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Config/config.h"
#include "llvm37/Support/Compiler.h"
#include "llvm37/Support/ThreadLocal.h"

//===----------------------------------------------------------------------===//
//=== WARNING: Implementation here must contain only TRULY operating system
//===          independent code.
//===----------------------------------------------------------------------===//

#if !defined(LLVM37_ENABLE_THREADS) || LLVM37_ENABLE_THREADS == 0
// Define all methods as no-ops if threading is explicitly disabled
namespace llvm37 {
using namespace sys;
ThreadLocalImpl::ThreadLocalImpl() : data() { }
ThreadLocalImpl::~ThreadLocalImpl() { }
void ThreadLocalImpl::setInstance(const void* d) {
  static_assert(sizeof(d) <= sizeof(data), "size too big");
  void **pd = reinterpret_cast<void**>(&data);
  *pd = const_cast<void*>(d);
}
void *ThreadLocalImpl::getInstance() {
  void **pd = reinterpret_cast<void**>(&data);
  return *pd;
}
void ThreadLocalImpl::removeInstance() {
  setInstance(nullptr);
}
}
#elif defined(LLVM37_ON_UNIX)
#include "Unix/ThreadLocal.inc"
#elif defined( LLVM37_ON_WIN32)
#include "Windows/ThreadLocal.inc"
#else
#warning Neither LLVM37_ON_UNIX nor LLVM37_ON_WIN32 set in Support/ThreadLocal.cpp
#endif
