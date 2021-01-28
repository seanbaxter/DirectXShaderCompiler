//===-- FrontendAction.h - Pluggable Frontend Action Interface --*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_FRONTEND_FRONTENDPLUGINREGISTRY_H
#define LLVM37_CLANG_FRONTEND_FRONTENDPLUGINREGISTRY_H

#include "clang/Frontend/FrontendAction.h"
#include "llvm37/Support/Registry.h"

// Instantiated in FrontendAction.cpp.
extern template class llvm37::Registry<clang::PluginASTAction>;

namespace clang {

/// The frontend plugin registry.
typedef llvm37::Registry<PluginASTAction> FrontendPluginRegistry;

} // end namespace clang

#endif
