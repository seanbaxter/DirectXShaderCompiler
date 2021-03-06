//===-- DynamicLibrary.cpp - Runtime link/load libraries --------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file implements the operating system DynamicLibrary concept.
//
// FIXME: This file leaks ExplicitSymbols and OpenedHandles!
//
//===----------------------------------------------------------------------===//

#include "llvm37/Support/DynamicLibrary.h"
#include "llvm37-c/Support.h"
#include "llvm37/ADT/DenseSet.h"
#include "llvm37/ADT/StringMap.h"
#include "llvm37/Config/config.h"
#include "llvm37/Support/ManagedStatic.h"
#include "llvm37/Support/Mutex.h"
#include <cstdio>
#include <cstring>

// Collection of symbol name/value pairs to be searched prior to any libraries.
static llvm37::ManagedStatic<llvm37::StringMap<void *> > ExplicitSymbols;
static llvm37::ManagedStatic<llvm37::sys::SmartMutex<true> > SymbolsMutex;

void llvm37::sys::DynamicLibrary::AddSymbol(StringRef symbolName,
                                          void *symbolValue) {
  SmartScopedLock<true> lock(*SymbolsMutex);
  (*ExplicitSymbols)[symbolName] = symbolValue;
}

char llvm37::sys::DynamicLibrary::Invalid = 0;

#ifdef LLVM37_ON_WIN32

#include "Windows/DynamicLibrary.inc"

#else

#if HAVE_DLFCN_H
#include <dlfcn.h>
using namespace llvm37;
using namespace llvm37::sys;

//===----------------------------------------------------------------------===//
//=== WARNING: Implementation here must contain only TRULY operating system
//===          independent code.
//===----------------------------------------------------------------------===//

static DenseSet<void *> *OpenedHandles = nullptr;

DynamicLibrary DynamicLibrary::getPermanentLibrary(const char *filename,
                                                   std::string *errMsg) {
  SmartScopedLock<true> lock(*SymbolsMutex);

  void *handle = dlopen(filename, RTLD_LAZY|RTLD_GLOBAL);
  if (!handle) {
    if (errMsg) *errMsg = dlerror();
    return DynamicLibrary();
  }

#ifdef __CYGWIN__
  // Cygwin searches symbols only in the main
  // with the handle of dlopen(NULL, RTLD_GLOBAL).
  if (!filename)
    handle = RTLD_DEFAULT;
#endif

  if (!OpenedHandles)
    OpenedHandles = new DenseSet<void *>();

  // If we've already loaded this library, dlclose() the handle in order to
  // keep the internal refcount at +1.
  if (!OpenedHandles->insert(handle).second)
    dlclose(handle);

  return DynamicLibrary(handle);
}

void *DynamicLibrary::getAddressOfSymbol(const char *symbolName) {
  if (!isValid())
    return nullptr;
  return dlsym(Data, symbolName);
}

#else

using namespace llvm37;
using namespace llvm37::sys;

DynamicLibrary DynamicLibrary::getPermanentLibrary(const char *filename,
                                                   std::string *errMsg) {
  if (errMsg) *errMsg = "dlopen() not supported on this platform";
  return DynamicLibrary();
}

void *DynamicLibrary::getAddressOfSymbol(const char *symbolName) {
  return NULL;
}

#endif

namespace llvm37 {
void *SearchForAddressOfSpecialSymbol(const char* symbolName);
}

void* DynamicLibrary::SearchForAddressOfSymbol(const char *symbolName) {
  SmartScopedLock<true> Lock(*SymbolsMutex);

  // First check symbols added via AddSymbol().
  if (ExplicitSymbols.isConstructed()) {
    StringMap<void *>::iterator i = ExplicitSymbols->find(symbolName);

    if (i != ExplicitSymbols->end())
      return i->second;
  }

#if HAVE_DLFCN_H
  // Now search the libraries.
  if (OpenedHandles) {
    for (DenseSet<void *>::iterator I = OpenedHandles->begin(),
         E = OpenedHandles->end(); I != E; ++I) {
      //lt_ptr ptr = lt_dlsym(*I, symbolName);
      void *ptr = dlsym(*I, symbolName);
      if (ptr) {
        return ptr;
      }
    }
  }
#endif

  if (void *Result = llvm37::SearchForAddressOfSpecialSymbol(symbolName))
    return Result;

// This macro returns the address of a well-known, explicit symbol
#define EXPLICIT_SYMBOL(SYM)                                                   \
  if (!strcmp(symbolName, #SYM))                                               \
    return (void *)&SYM

// On linux we have a weird situation. The stderr/out/in symbols are both
// macros and global variables because of standards requirements. So, we
// boldly use the EXPLICIT_SYMBOL macro without checking for a #define first.
#if defined(__linux__) and !defined(__ANDROID__)
  {
    EXPLICIT_SYMBOL(stderr);
    EXPLICIT_SYMBOL(stdout);
    EXPLICIT_SYMBOL(stdin);
  }
#else
  // For everything else, we want to check to make sure the symbol isn't defined
  // as a macro before using EXPLICIT_SYMBOL.
  {
#ifndef stdin
    EXPLICIT_SYMBOL(stdin);
#endif
#ifndef stdout
    EXPLICIT_SYMBOL(stdout);
#endif
#ifndef stderr
    EXPLICIT_SYMBOL(stderr);
#endif
  }
#endif
#undef EXPLICIT_SYMBOL

  return nullptr;
}

#endif // LLVM37_ON_WIN32

//===----------------------------------------------------------------------===//
// C API.
//===----------------------------------------------------------------------===//

LLVM37Bool LLVM37LoadLibraryPermanently(const char* Filename) {
  return llvm37::sys::DynamicLibrary::LoadLibraryPermanently(Filename);
}

void *LLVM37SearchForAddressOfSymbol(const char *symbolName) {
  return llvm37::sys::DynamicLibrary::SearchForAddressOfSymbol(symbolName);
}

void LLVM37AddSymbol(const char *symbolName, void *symbolValue) {
  return llvm37::sys::DynamicLibrary::AddSymbol(symbolName, symbolValue);
}

