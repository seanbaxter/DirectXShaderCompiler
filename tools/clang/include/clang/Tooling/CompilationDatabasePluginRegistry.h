//===--- CompilationDatabasePluginRegistry.h - ------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_TOOLING_COMPILATIONDATABASEPLUGINREGISTRY_H
#define LLVM37_CLANG_TOOLING_COMPILATIONDATABASEPLUGINREGISTRY_H

#include "clang/Tooling/CompilationDatabase.h"
#include "llvm37/Support/Registry.h"

namespace clang {
namespace tooling {

class CompilationDatabasePlugin;

typedef llvm37::Registry<CompilationDatabasePlugin>
    CompilationDatabasePluginRegistry;

} // end namespace tooling
} // end namespace clang

#endif
