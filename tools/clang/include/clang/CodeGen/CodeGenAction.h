//===--- CodeGenAction.h - LLVM37 Code Generation Frontend Action -*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_CODEGEN_CODEGENACTION_H
#define LLVM37_CLANG_CODEGEN_CODEGENACTION_H

#include "clang/Frontend/FrontendAction.h"
#include <memory>

namespace llvm37 {
  class LLVM37Context;
  class Module;
}

namespace clang {
class BackendConsumer;

class CodeGenAction : public ASTFrontendAction {
private:
  unsigned Act;
  std::unique_ptr<llvm37::Module> TheModule;
  llvm37::Module *LinkModule;
  llvm37::LLVM37Context *VMContext;
  bool OwnsVMContext;

protected:
  /// Create a new code generation action.  If the optional \p _VMContext
  /// parameter is supplied, the action uses it without taking ownership,
  /// otherwise it creates a fresh LLVM37 context and takes ownership.
  CodeGenAction(unsigned _Act, llvm37::LLVM37Context *_VMContext = nullptr);

  bool hasIRSupport() const override;

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef InFile) override;

  void ExecuteAction() override;

  void EndSourceFileAction() override;

public:
  ~CodeGenAction() override;

  /// setLinkModule - Set the link module to be used by this action.  If a link
  /// module is not provided, and CodeGenOptions::LinkBitcodeFile is non-empty,
  /// the action will load it from the specified file.
  void setLinkModule(llvm37::Module *Mod) { LinkModule = Mod; }

  /// Take the generated LLVM37 module, for use after the action has been run.
  /// The result may be null on failure.
  std::unique_ptr<llvm37::Module> takeModule();

  /// Take the LLVM37 context used by this action.
  llvm37::LLVM37Context *takeLLVM37Context();

  BackendConsumer *BEConsumer;
};

class EmitAssemblyAction : public CodeGenAction {
  virtual void anchor();
public:
  EmitAssemblyAction(llvm37::LLVM37Context *_VMContext = nullptr);
};

class EmitBCAction : public CodeGenAction {
  virtual void anchor();
public:
  EmitBCAction(llvm37::LLVM37Context *_VMContext = nullptr);
};

class EmitLLVM37Action : public CodeGenAction {
  virtual void anchor();
public:
  EmitLLVM37Action(llvm37::LLVM37Context *_VMContext = nullptr);
};

class EmitLLVM37OnlyAction : public CodeGenAction {
  virtual void anchor();
public:
  EmitLLVM37OnlyAction(llvm37::LLVM37Context *_VMContext = nullptr);
};

class EmitCodeGenOnlyAction : public CodeGenAction {
  virtual void anchor();
public:
  EmitCodeGenOnlyAction(llvm37::LLVM37Context *_VMContext = nullptr);
};

class EmitObjAction : public CodeGenAction {
  virtual void anchor();
public:
  EmitObjAction(llvm37::LLVM37Context *_VMContext = nullptr);
};

// HLSL Change Starts
class EmitOptDumpAction : public CodeGenAction {
  virtual void anchor();
public:
  EmitOptDumpAction(llvm37::LLVM37Context *_VMContext = nullptr);
};
// HLSL Change Ends

}

#endif
