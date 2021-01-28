//===-- cc1_main.cpp - Clang CC1 Compiler Frontend ------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is the entry point to the clang -cc1 functionality, which implements the
// core compiler functionality along with a number of additional tools for
// demonstration and testing purposes.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Option/Arg.h"
#include "clang/CodeGen/ObjectFilePCHContainerOperations.h"
#include "clang/Driver/DriverDiagnostic.h"
#include "clang/Driver/Options.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/FrontendDiagnostic.h"
#include "clang/Frontend/TextDiagnosticBuffer.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Frontend/Utils.h"
#include "clang/FrontendTool/Utils.h"
#include "llvm37/ADT/Statistic.h"
#include "llvm37/LinkAllPasses.h"
#include "llvm37/Option/ArgList.h"
#include "llvm37/Option/OptTable.h"
#include "llvm37/Support/ErrorHandling.h"
#include "llvm37/Support/ManagedStatic.h"
#include "llvm37/Support/Signals.h"
#include "llvm37/Support/TargetSelect.h"
#include "llvm37/Support/Timer.h"
#include "llvm37/Support/raw_ostream.h"
#include <cstdio>
using namespace clang;
using namespace llvm37::opt;

//===----------------------------------------------------------------------===//
// Main driver
//===----------------------------------------------------------------------===//

static void LLVM37ErrorHandler(void *UserData, const std::string &Message,
                             bool GenCrashDiag) {
  DiagnosticsEngine &Diags = *static_cast<DiagnosticsEngine*>(UserData);

  Diags.Report(diag::err_fe_error_backend) << Message;

  // Run the interrupt handlers to make sure any special cleanups get done, in
  // particular that we remove files registered with RemoveFileOnSignal.
  llvm37::sys::RunInterruptHandlers();

  // We cannot recover from llvm37 errors.  When reporting a fatal error, exit
  // with status 70 to generate crash diagnostics.  For BSD systems this is
  // defined as an internal software error.  Otherwise, exit with status 1.
  exit(GenCrashDiag ? 70 : 1);
}

#ifdef LINK_POLLY_INTO_TOOLS
namespace polly {
void initializePollyPasses(llvm37::PassRegistry &Registry);
}
#endif

int cc1_main(ArrayRef<const char *> Argv, const char *Argv0, void *MainAddr) {
  std::unique_ptr<CompilerInstance> Clang(new CompilerInstance());
  IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());

  // Register the support for object-file-wrapped Clang modules.
  auto PCHOps = Clang->getPCHContainerOperations();
  PCHOps->registerWriter(llvm37::make_unique<ObjectFilePCHContainerWriter>());
  PCHOps->registerReader(llvm37::make_unique<ObjectFilePCHContainerReader>());

  // Initialize targets first, so that --version shows registered targets.
  llvm37::InitializeAllTargets();
  llvm37::InitializeAllTargetMCs();
  llvm37::InitializeAllAsmPrinters();
  llvm37::InitializeAllAsmParsers();

#ifdef LINK_POLLY_INTO_TOOLS
  llvm37::PassRegistry &Registry = *llvm37::PassRegistry::getPassRegistry();
  polly::initializePollyPasses(Registry);
#endif

  // Buffer diagnostics from argument parsing so that we can output them using a
  // well formed diagnostic object.
  IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts = new DiagnosticOptions();
  TextDiagnosticBuffer *DiagsBuffer = new TextDiagnosticBuffer;
  DiagnosticsEngine Diags(DiagID, &*DiagOpts, DiagsBuffer);
  bool Success = CompilerInvocation::CreateFromArgs(
      Clang->getInvocation(), Argv.begin(), Argv.end(), Diags);

  // Infer the builtin include path if unspecified.
  if (Clang->getHeaderSearchOpts().UseBuiltinIncludes &&
      Clang->getHeaderSearchOpts().ResourceDir.empty())
    Clang->getHeaderSearchOpts().ResourceDir =
      CompilerInvocation::GetResourcesPath(Argv0, MainAddr);

  // Create the actual diagnostics engine.
  Clang->createDiagnostics();
  if (!Clang->hasDiagnostics())
    return 1;

  // Set an error handler, so that any LLVM37 backend diagnostics go through our
  // error handler.
  llvm37::install_fatal_error_handler(LLVM37ErrorHandler,
                                  static_cast<void*>(&Clang->getDiagnostics()));

  DiagsBuffer->FlushDiagnostics(Clang->getDiagnostics());
  if (!Success)
    return 1;

  // Execute the frontend actions.
  Success = ExecuteCompilerInvocation(Clang.get());

  // If any timers were active but haven't been destroyed yet, print their
  // results now.  This happens in -disable-free mode.
  llvm37::TimerGroup::printAll(llvm37::errs());

  // Our error handler depends on the Diagnostics object, which we're
  // potentially about to delete. Uninstall the handler now so that any
  // later errors use the default handling behavior instead.
  llvm37::remove_fatal_error_handler();

  // When running with -disable-free, don't do any destruction or shutdown.
  if (Clang->getFrontendOpts().DisableFree) {
    if (llvm37::AreStatisticsEnabled() || Clang->getFrontendOpts().ShowStats)
      llvm37::PrintStatistics();
    BuryPointer(std::move(Clang));
    return !Success;
  }

  // Managed static deconstruction. Useful for making things like
  // -time-passes usable.
  llvm37::llvm37_shutdown();

  return !Success;
}
