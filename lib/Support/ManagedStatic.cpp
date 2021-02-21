//===-- ManagedStatic.cpp - Static Global wrapper -------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the ManagedStatic class and llvm37_shutdown().
//
//===----------------------------------------------------------------------===//

#include "llvm37/Support/ManagedStatic.h"
#include "llvm37/Config/config.h"
#include "llvm37/Support/Atomic.h"
#include "llvm37/Support/Mutex.h"
#include "llvm37/Support/MutexGuard.h"
#include <cassert>
using namespace llvm37;

static const ManagedStaticBase *StaticList = nullptr;

static sys::Mutex& getManagedStaticMutex() {
  // We need to use a function local static here, since this can get called
  // during a static constructor and we need to guarantee that it's initialized
  // correctly.
  static sys::Mutex ManagedStaticMutex;
  return ManagedStaticMutex;
}

void ManagedStaticBase::RegisterManagedStatic(void *(*Creator)(),
                                              void (*Deleter)(void*)) const {
  assert(Creator);
  if (llvm37_is_multithreaded()) {
    MutexGuard Lock(getManagedStaticMutex());

    if (!Ptr) {
      void* tmp = Creator();

      LLVM37_TsanHappensBefore(this);
      sys::MemoryFence();

      // This write is racy against the first read in the ManagedStatic
      // accessors. The race is benign because it does a second read after a
      // memory fence, at which point it isn't possible to get a partial value.
      LLVM37_TsanIgnoreWritesBegin();
      Ptr = tmp;
      LLVM37_TsanIgnoreWritesEnd();
      DeleterFn = Deleter;
      
      // Add to list of managed statics.
      Next = StaticList;
      StaticList = this;
    }
  } else {
    assert(!Ptr && !DeleterFn && !Next &&
           "Partially initialized ManagedStatic!?");
    Ptr = Creator();
    DeleterFn = Deleter;
  
    // Add to list of managed statics.
    Next = StaticList;
    StaticList = this;
  }
}

void ManagedStaticBase::destroy() const {
  assert(DeleterFn && "ManagedStatic not initialized correctly!");
  assert(StaticList == this &&
         "Not destroyed in reverse order of construction?");
  // Unlink from list.
  StaticList = Next;
  Next = nullptr;

  // Destroy memory.
  DeleterFn(Ptr);
  
  // Cleanup.
  Ptr = nullptr;
  DeleterFn = nullptr;
}

/// llvm37_shutdown - Deallocate and destroy all ManagedStatic variables.
void llvm37::llvm37_shutdown() {
  MutexGuard Lock(getManagedStaticMutex());

  while (StaticList)
    StaticList->destroy();
}
