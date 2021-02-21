//===-- llvm37/Support/ManagedStatic.h - Static Global wrapper ----*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the ManagedStatic class and the llvm37_shutdown() function.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_SUPPORT_MANAGEDSTATIC_H
#define LLVM37_SUPPORT_MANAGEDSTATIC_H

#include "llvm37/Support/Atomic.h"
#include "llvm37/Support/Threading.h"
#include "llvm37/Support/Valgrind.h"

namespace llvm37 {

/// object_creator - Helper method for ManagedStatic.
template<class C>
void* object_creator() {
  return new C();
}

/// object_deleter - Helper method for ManagedStatic.
///
template<typename T> struct object_deleter {
  static void call(void * Ptr) { delete (T*)Ptr; }
};
template<typename T, size_t N> struct object_deleter<T[N]> {
  static void call(void * Ptr) { delete[] (T*)Ptr; }
};

/// ManagedStaticBase - Common base class for ManagedStatic instances.
class ManagedStaticBase {
protected:
  // This should only be used as a static variable, which guarantees that this
  // will be zero initialized.
  mutable void *Ptr;
  mutable void (*DeleterFn)(void*);
  mutable const ManagedStaticBase *Next;

  void RegisterManagedStatic(void *(*creator)(), void (*deleter)(void*)) const;
public:
  /// isConstructed - Return true if this object has not been created yet.
  bool isConstructed() const { return Ptr != nullptr; }

  void destroy() const;
};

/// ManagedStatic - This transparently changes the behavior of global statics to
/// be lazily constructed on demand (good for reducing startup times of dynamic
/// libraries that link in LLVM37 components) and for making destruction be
/// explicit through the llvm37_shutdown() function call.
///
template<class C>
class ManagedStatic : public ManagedStaticBase {
public:

  // Accessors.
  C &operator*() {
    void* tmp = Ptr;
    if (llvm37_is_multithreaded()) sys::MemoryFence();
    if (!tmp) RegisterManagedStatic(object_creator<C>, object_deleter<C>::call);
    LLVM37_TsanHappensAfter(this);

    return *static_cast<C*>(Ptr);
  }
  C *operator->() {
    void* tmp = Ptr;
    if (llvm37_is_multithreaded()) sys::MemoryFence();
    if (!tmp) RegisterManagedStatic(object_creator<C>, object_deleter<C>::call);
    LLVM37_TsanHappensAfter(this);

    return static_cast<C*>(Ptr);
  }
  const C &operator*() const {
    void* tmp = Ptr;
    if (llvm37_is_multithreaded()) sys::MemoryFence();
    if (!tmp) RegisterManagedStatic(object_creator<C>, object_deleter<C>::call);
    LLVM37_TsanHappensAfter(this);

    return *static_cast<C*>(Ptr);
  }
  const C *operator->() const {
    void* tmp = Ptr;
    if (llvm37_is_multithreaded()) sys::MemoryFence();
    if (!tmp) RegisterManagedStatic(object_creator<C>, object_deleter<C>::call);
    LLVM37_TsanHappensAfter(this);

    return static_cast<C*>(Ptr);
  }
};

/// llvm37_shutdown - Deallocate and destroy all ManagedStatic variables.
void llvm37_shutdown();

/// llvm37_shutdown_obj - This is a simple helper class that calls
/// llvm37_shutdown() when it is destroyed.
struct llvm37_shutdown_obj {
  llvm37_shutdown_obj() { }
  ~llvm37_shutdown_obj() { llvm37_shutdown(); }
};

}

#endif
