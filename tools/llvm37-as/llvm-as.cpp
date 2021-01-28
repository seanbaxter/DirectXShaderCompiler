//===--- llvm37-as.cpp - The low-level LLVM37 assembler -----------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This utility may be invoked in the following manner:
//   llvm37-as --help         - Output information about command line switches
//   llvm37-as [options]      - Read LLVM37 asm from stdin, write bitcode to stdout
//   llvm37-as [options] x.ll - Read LLVM37 asm from the x.ll file, write bitcode
//                            to the x.bc file.
//
//===----------------------------------------------------------------------===//

#include "llvm37/IR/LLVMContext.h"
#include "llvm37/AsmParser/Parser.h"
#include "llvm37/Bitcode/ReaderWriter.h"
#include "llvm37/IR/Module.h"
#include "llvm37/IR/Verifier.h"
#include "llvm37/Support/CommandLine.h"
#include "llvm37/Support/FileSystem.h"
#include "llvm37/Support/ManagedStatic.h"
#include "llvm37/Support/PrettyStackTrace.h"
#include "llvm37/Support/Signals.h"
#include "llvm37/Support/SourceMgr.h"
#include "llvm37/Support/SystemUtils.h"
#include "llvm37/Support/ToolOutputFile.h"
#include <memory>
using namespace llvm37;

static cl::opt<std::string>
InputFilename(cl::Positional, cl::desc("<input .llvm37 file>"), cl::init("-"));

static cl::opt<std::string>
OutputFilename("o", cl::desc("Override output filename"),
               cl::value_desc("filename"));

static cl::opt<bool>
Force("f", cl::desc("Enable binary output on terminals"));

static cl::opt<bool>
DisableOutput("disable-output", cl::desc("Disable output"), cl::init(false));

static cl::opt<bool>
DumpAsm("d", cl::desc("Print assembly as parsed"), cl::Hidden);

static cl::opt<bool>
DisableVerify("disable-verify", cl::Hidden,
              cl::desc("Do not run verifier on input LLVM37 (dangerous!)"));

static cl::opt<bool> PreserveBitcodeUseListOrder(
    "preserve-bc-uselistorder",
    cl::desc("Preserve use-list order when writing LLVM37 bitcode."),
    cl::init(true), cl::Hidden);

static void WriteOutputFile(const Module *M) {
  // Infer the output filename if needed.
  if (OutputFilename.empty()) {
    if (InputFilename == "-") {
      OutputFilename = "-";
    } else {
      StringRef IFN = InputFilename;
      OutputFilename = (IFN.endswith(".ll") ? IFN.drop_back(3) : IFN).str();
      OutputFilename += ".bc";
    }
  }

  std::error_code EC;
  std::unique_ptr<tool_output_file> Out(
      new tool_output_file(OutputFilename, EC, sys::fs::F_None));
  if (EC) {
    errs() << EC.message() << '\n';
    exit(1);
  }

  if (Force || !CheckBitcodeOutputToConsole(Out->os(), true))
    WriteBitcodeToFile(M, Out->os(), PreserveBitcodeUseListOrder);

  // Declare success.
  Out->keep();
}

// HLSL Change Starts
#define NOMINMAX
#include <windows.h>
#include "llvm37/Support/FileSystem.h"
#include "llvm37/Support/MSFileSystem.h"
// HLSL Change Ends

// HLSL Change: changed calling convention to __cdecl
int __cdecl main(int argc, char **argv) {
  // Print a stack trace if we signal out.
  // sys::PrintStackTraceOnErrorSignal(); // HLSL Change - disable this
  // PrettyStackTraceProgram X(argc, argv); // HLSL Change - disable this
  // HLSL Change Starts
  llvm37::sys::fs::MSFileSystem* msfPtr;
  HRESULT hr;
  if (!SUCCEEDED(hr = CreateMSFileSystemForDisk(&msfPtr)))
    return 1;
  std::unique_ptr<llvm37::sys::fs::MSFileSystem> msf(msfPtr);
  llvm37::sys::fs::AutoPerThreadSystem pts(msf.get());
  llvm37::STDStreamCloser stdStreamCloser;
  // HLSL Change Ends

  LLVM37Context &Context = getGlobalContext();
  llvm37_shutdown_obj Y;  // Call llvm37_shutdown() on exit.
  cl::ParseCommandLineOptions(argc, argv, "llvm37 .ll -> .bc assembler\n");

  // Parse the file now...
  SMDiagnostic Err;
  std::unique_ptr<Module> M = parseAssemblyFile(InputFilename, Err, Context);
  if (!M.get()) {
    Err.print(argv[0], errs());
    return 1;
  }

  if (!DisableVerify) {
    std::string ErrorStr;
    raw_string_ostream OS(ErrorStr);
    if (verifyModule(*M.get(), &OS)) {
      errs() << argv[0]
             << ": assembly parsed, but does not verify as correct!\n";
      errs() << OS.str();
      return 1;
    }
  }

  if (DumpAsm) errs() << "Here's the assembly:\n" << *M.get();

  if (!DisableOutput)
    WriteOutputFile(M.get());

  return 0;
}
