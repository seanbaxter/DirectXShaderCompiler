//===- CIndex.cpp - Clang-C Source Indexing Library -----------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Clang-C Source Indexing library.
//
//===----------------------------------------------------------------------===//

#include "CIndexer.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclVisitor.h"
#include "clang/AST/StmtVisitor.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/Version.h"
#include "clang/Sema/CodeCompleteConsumer.h"
#include "llvm37/ADT/StringExtras.h"
#include "llvm37/Config/llvm-config.h"
#include "llvm37/Support/Compiler.h"
#include "llvm37/Support/MemoryBuffer.h"
#include "llvm37/Support/Program.h"
#include "llvm37/Support/raw_ostream.h"
#include <cstdio>
#include <sstream>
#include <vector>

#ifdef __CYGWIN__
#include <cygwin/version.h>
#include <sys/cygwin.h>
#define LLVM37_ON_WIN32 1
#endif

#ifdef LLVM37_ON_WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

using namespace clang;

const std::string &CIndexer::getClangResourcesPath() {
  // HLSL Change Starts - never have a DLL-relative path
#if 1
  return ResourcesPath;
#else
  // Did we already compute the path?
  if (!ResourcesPath.empty())
    return ResourcesPath;

  SmallString<128> LibClangPath;

  // Find the location where this library lives (libclang.dylib).
#ifdef LLVM37_ON_WIN32
  MEMORY_BASIC_INFORMATION mbi;
  char path[MAX_PATH];
  VirtualQuery((void *)(uintptr_t)clang_createTranslationUnit, &mbi,
               sizeof(mbi));
  GetModuleFileNameA((HINSTANCE)mbi.AllocationBase, path, MAX_PATH);

#ifdef __CYGWIN__
  char w32path[MAX_PATH];
  strcpy(w32path, path);
#if CYGWIN_VERSION_API_MAJOR > 0 || CYGWIN_VERSION_API_MINOR >= 181
  cygwin_conv_path(CCP_WIN_A_TO_POSIX, w32path, path, MAX_PATH);
#else
  cygwin_conv_to_full_posix_path(w32path, path);
#endif
#endif

  LibClangPath += llvm37::sys::path::parent_path(path);
#else
  // This silly cast below avoids a C++ warning.
  Dl_info info;
  if (dladdr((void *)(uintptr_t)clang_createTranslationUnit, &info) == 0)
    llvm37_unreachable("Call to dladdr() failed");

  // We now have the CIndex directory, locate clang relative to it.
  LibClangPath += llvm37::sys::path::parent_path(info.dli_fname);
#endif

  llvm37::sys::path::append(LibClangPath, "clang", CLANG_VERSION_STRING);

  // Cache our result.
  ResourcesPath = LibClangPath.str();
  return ResourcesPath;
#endif // HLSL Change Ends
}
