//===--- CodeGen/ModuleBuilder.h - Build LLVM37 from ASTs ---------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the ModuleBuilder interface.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_CODEGEN_MODULEBUILDER_H
#define LLVM37_CLANG_CODEGEN_MODULEBUILDER_H

#include "clang/AST/ASTConsumer.h"
#include <string>

namespace llvm37 {
  class LLVM37Context;
  class Module;
}

namespace clang {
  class DiagnosticsEngine;
  class CoverageSourceInfo;
  class LangOptions;
  class HeaderSearchOptions;
  class PreprocessorOptions;
  class CodeGenOptions;
  class Decl;

  class CodeGenerator : public ASTConsumer {
    virtual void anchor();
  public:
    virtual llvm37::Module* GetModule() = 0;
    virtual llvm37::Module* ReleaseModule() = 0;
    virtual const Decl *GetDeclForMangledName(llvm37::StringRef MangledName) = 0;
  };

  /// CreateLLVM37CodeGen - Create a CodeGenerator instance.
  /// It is the responsibility of the caller to call delete on
  /// the allocated CodeGenerator instance.
  CodeGenerator *CreateLLVM37CodeGen(DiagnosticsEngine &Diags,
                                   const std::string &ModuleName,
                                   const HeaderSearchOptions &HeaderSearchOpts,
                                   const PreprocessorOptions &PreprocessorOpts,
                                   const CodeGenOptions &CGO,
                                   llvm37::LLVM37Context& C,
                                   CoverageSourceInfo *CoverageInfo = nullptr);
}

#endif
