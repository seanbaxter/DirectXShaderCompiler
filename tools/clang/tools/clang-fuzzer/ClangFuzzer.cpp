//===-- ClangFuzzer.cpp - Fuzz Clang --------------------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file implements a function that runs Clang on a single
///  input. This function is then linked into the Fuzzer library.
///
//===----------------------------------------------------------------------===//

#include "clang/Tooling/Tooling.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm37/Option/Option.h"

using namespace clang;

extern "C" void LLVM37FuzzerTestOneInput(uint8_t *data, size_t size) {
  std::string s((const char *)data, size);
  llvm37::opt::ArgStringList CC1Args;
  CC1Args.push_back("-cc1");
  CC1Args.push_back("./test.cc");
  llvm37::IntrusiveRefCntPtr<FileManager> Files(
      new FileManager(FileSystemOptions()));
  IgnoringDiagConsumer Diags;
  IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts = new DiagnosticOptions();
  DiagnosticsEngine Diagnostics(
      IntrusiveRefCntPtr<clang::DiagnosticIDs>(new DiagnosticIDs()), &*DiagOpts,
      &Diags, false);
  std::unique_ptr<clang::CompilerInvocation> Invocation(
      tooling::newInvocation(&Diagnostics, CC1Args));
  std::unique_ptr<llvm37::MemoryBuffer> Input =
      llvm37::MemoryBuffer::getMemBuffer(s);
  Invocation->getPreprocessorOpts().addRemappedFile("./test.cc", Input.release());
  std::unique_ptr<tooling::ToolAction> action(
      tooling::newFrontendActionFactory<clang::SyntaxOnlyAction>());
  std::shared_ptr<PCHContainerOperations> PCHContainerOps =
      std::make_shared<RawPCHContainerOperations>();
  action->runInvocation(Invocation.release(), Files.get(), PCHContainerOps,
                        &Diags);
}
