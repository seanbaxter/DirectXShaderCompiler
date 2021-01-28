//===- llvm37/PassRegistry.h - Pass Information Registry ----------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines PassRegistry, a class that is used in the initialization
// and registration of passes.  At application startup, passes are registered
// with the PassRegistry, which is later provided to the PassManager for 
// dependency resolution and similar tasks.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_PASSREGISTRY_H
#define LLVM37_PASSREGISTRY_H

#include "llvm37-c/Core.h"
#include "llvm37/ADT/DenseMap.h"
#include "llvm37/ADT/SmallPtrSet.h"
#include "llvm37/ADT/StringMap.h"
#include "llvm37/ADT/StringRef.h"
#include "llvm37/PassInfo.h"
#include "llvm37/Support/CBindingWrapping.h"
#include "llvm37/Support/RWMutex.h"
#include <vector>

namespace llvm37 {

class PassInfo;
struct PassRegistrationListener;

/// PassRegistry - This class manages the registration and intitialization of
/// the pass subsystem as application startup, and assists the PassManager
/// in resolving pass dependencies.
/// NOTE: PassRegistry is NOT thread-safe.  If you want to use LLVM37 on multiple
/// threads simultaneously, you will need to use a separate PassRegistry on
/// each thread.
class PassRegistry {
  #ifndef LLVM37_ON_WIN32
  // HLSL Change - no lock needed for Windows, as it will use its own mechanism defined in PassRegistry.cpp.
  mutable sys::SmartRWMutex<true> Lock;
  #endif

  /// PassInfoMap - Keep track of the PassInfo object for each registered pass.
  typedef DenseMap<const void *, const PassInfo *> MapType;
  MapType PassInfoMap;

  typedef StringMap<const PassInfo *> StringMapType;
  StringMapType PassInfoStringMap;

  std::vector<std::unique_ptr<const PassInfo>> ToFree;
  std::vector<PassRegistrationListener *> Listeners;

public:
  PassRegistry() {}
  ~PassRegistry();

  /// getPassRegistry - Access the global registry object, which is
  /// automatically initialized at application launch and destroyed by
  /// llvm37_shutdown.
  static PassRegistry *getPassRegistry();

  /// getPassInfo - Look up a pass' corresponding PassInfo, indexed by the pass'
  /// type identifier (&MyPass::ID).
  const PassInfo *getPassInfo(const void *TI) const;

  /// getPassInfo - Look up a pass' corresponding PassInfo, indexed by the pass'
  /// argument string.
  const PassInfo *getPassInfo(StringRef Arg) const;

  /// registerPass - Register a pass (by means of its PassInfo) with the
  /// registry.  Required in order to use the pass with a PassManager.
  void registerPass(const PassInfo &PI, bool ShouldFree = false);

  /// registerAnalysisGroup - Register an analysis group (or a pass implementing
  // an analysis group) with the registry.  Like registerPass, this is required
  // in order for a PassManager to be able to use this group/pass.
  void registerAnalysisGroup(const void *InterfaceID, const void *PassID,
                             PassInfo &Registeree, bool isDefault,
                             bool ShouldFree = false);

  /// enumerateWith - Enumerate the registered passes, calling the provided
  /// PassRegistrationListener's passEnumerate() callback on each of them.
  void enumerateWith(PassRegistrationListener *L);

  /// addRegistrationListener - Register the given PassRegistrationListener
  /// to receive passRegistered() callbacks whenever a new pass is registered.
  void addRegistrationListener(PassRegistrationListener *L);

  /// removeRegistrationListener - Unregister a PassRegistrationListener so that
  /// it no longer receives passRegistered() callbacks.
  void removeRegistrationListener(PassRegistrationListener *L);
};

// Create wrappers for C Binding types (see CBindingWrapping.h).
DEFINE_STDCXX_CONVERSION_FUNCTIONS(PassRegistry, LLVM37PassRegistryRef)

}

#endif
