///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// HLUtil.h                                                                  //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// HL helper functions.                                                      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "llvm37/ADT/SetVector.h"

namespace llvm37 {
class Function;
class Value;
class MemCpyInst;
} // namespace llvm37

namespace hlsl {
class DxilTypeSystem;

namespace hlutil {

struct PointerStatus {
  /// Keep track of what stores to the pointer look like.
  enum class StoredType {
    /// There is no store to this pointer.  It can thus be marked constant.
    NotStored,

    /// This ptr is a global, and is stored to, but the only thing stored is the
    /// constant it
    /// was initialized with. This is only tracked for scalar globals.
    InitializerStored,

    /// This ptr is stored to, but only its initializer and one other value
    /// is ever stored to it.  If this global isStoredOnce, we track the value
    /// stored to it in StoredOnceValue below.  This is only tracked for scalar
    /// globals.
    StoredOnce,

    /// This ptr is only assigned by a memcpy.
    MemcopyDestOnce,

    /// This ptr is stored to by multiple values or something else that we
    /// cannot track.
    Stored
  } storedType;
  /// Keep track of what loaded from the pointer look like.
  enum class LoadedType {
    /// There is no load to this pointer.  It can thus be marked constant.
    NotLoaded,

    /// This ptr is only used by a memcpy.
    MemcopySrcOnce,

    /// This ptr is loaded to by multiple instructions or something else that we
    /// cannot track.
    Loaded
  } loadedType;
  /// If only one value (besides the initializer constant) is ever stored to
  /// this global, keep track of what value it is.
  llvm37::Value *StoredOnceValue;
  /// Memcpy which this ptr is used.
  llvm37::SetVector<llvm37::MemCpyInst *> memcpySet;
  /// Memcpy which use this ptr as dest.
  llvm37::MemCpyInst *StoringMemcpy;
  /// Memcpy which use this ptr as src.
  llvm37::MemCpyInst *LoadingMemcpy;
  /// These start out null/false.  When the first accessing function is noticed,
  /// it is recorded. When a second different accessing function is noticed,
  /// HasMultipleAccessingFunctions is set to true.
  const llvm37::Function *AccessingFunction;
  bool HasMultipleAccessingFunctions;
  /// Size of the ptr.
  unsigned Size;
  llvm37::Value *Ptr;
  // Just check load store.
  bool bLoadStoreOnly;

  void analyze(DxilTypeSystem &typeSys, bool bStructElt);

  PointerStatus(llvm37::Value *ptr, unsigned size, bool bLdStOnly);
  void MarkAsStored();
  void MarkAsLoaded();
  bool HasStored();
  bool HasLoaded();
};

} // namespace hlutil

} // namespace hlsl