//===-- cc1as_main.cpp - Clang Assembler  ---------------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is the entry point to the clang -cc1as functionality, which implements
// the direct interface to the LLVM37 MC based assembler.
//
//===----------------------------------------------------------------------===//

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Driver/DriverDiagnostic.h"
#include "clang/Driver/Options.h"
#include "clang/Frontend/FrontendDiagnostic.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Frontend/Utils.h"
#include "llvm37/ADT/STLExtras.h"
#include "llvm37/ADT/StringSwitch.h"
#include "llvm37/ADT/Triple.h"
#include "llvm37/IR/DataLayout.h"
#include "llvm37/MC/MCAsmBackend.h"
#include "llvm37/MC/MCAsmInfo.h"
#include "llvm37/MC/MCCodeEmitter.h"
#include "llvm37/MC/MCContext.h"
#include "llvm37/MC/MCInstrInfo.h"
#include "llvm37/MC/MCObjectFileInfo.h"
#include "llvm37/MC/MCParser/MCAsmParser.h"
#include "llvm37/MC/MCRegisterInfo.h"
#include "llvm37/MC/MCStreamer.h"
#include "llvm37/MC/MCSubtargetInfo.h"
#include "llvm37/MC/MCTargetAsmParser.h"
#include "llvm37/MC/MCTargetOptions.h"
#include "llvm37/Option/Arg.h"
#include "llvm37/Option/ArgList.h"
#include "llvm37/Option/OptTable.h"
#include "llvm37/Support/CommandLine.h"
#include "llvm37/Support/ErrorHandling.h"
#include "llvm37/Support/FileSystem.h"
#include "llvm37/Support/FormattedStream.h"
#include "llvm37/Support/Host.h"
#include "llvm37/Support/ManagedStatic.h"
#include "llvm37/Support/MemoryBuffer.h"
#include "llvm37/Support/Path.h"
#include "llvm37/Support/PrettyStackTrace.h"
#include "llvm37/Support/Signals.h"
#include "llvm37/Support/SourceMgr.h"
#include "llvm37/Support/TargetRegistry.h"
#include "llvm37/Support/TargetSelect.h"
#include "llvm37/Support/Timer.h"
#include "llvm37/Support/raw_ostream.h"
#include <memory>
#include <system_error>
using namespace clang;
using namespace clang::driver;
using namespace clang::driver::options;
using namespace llvm37;
using namespace llvm37::opt;

namespace {

/// \brief Helper class for representing a single invocation of the assembler.
struct AssemblerInvocation {
  /// @name Target Options
  /// @{

  /// The name of the target triple to assemble for.
  std::string Triple;

  /// If given, the name of the target CPU to determine which instructions
  /// are legal.
  std::string CPU;

  /// The list of target specific features to enable or disable -- this should
  /// be a list of strings starting with '+' or '-'.
  std::vector<std::string> Features;

  /// @}
  /// @name Language Options
  /// @{

  std::vector<std::string> IncludePaths;
  unsigned NoInitialTextSection : 1;
  unsigned SaveTemporaryLabels : 1;
  unsigned GenDwarfForAssembly : 1;
  unsigned CompressDebugSections : 1;
  unsigned DwarfVersion;
  std::string DwarfDebugFlags;
  std::string DwarfDebugProducer;
  std::string DebugCompilationDir;
  std::string MainFileName;

  /// @}
  /// @name Frontend Options
  /// @{

  std::string InputFile;
  std::vector<std::string> LLVM37Args;
  std::string OutputPath;
  enum FileType {
    FT_Asm,  ///< Assembly (.s) output, transliterate mode.
    FT_Null, ///< No output, for timing purposes.
    FT_Obj   ///< Object file output.
  };
  FileType OutputType;
  unsigned ShowHelp : 1;
  unsigned ShowVersion : 1;

  /// @}
  /// @name Transliterate Options
  /// @{

  unsigned OutputAsmVariant;
  unsigned ShowEncoding : 1;
  unsigned ShowInst : 1;

  /// @}
  /// @name Assembler Options
  /// @{

  unsigned RelaxAll : 1;
  unsigned NoExecStack : 1;
  unsigned FatalWarnings : 1;

  /// @}

public:
  AssemblerInvocation() {
    Triple = "";
    NoInitialTextSection = 0;
    InputFile = "-";
    OutputPath = "-";
    OutputType = FT_Asm;
    OutputAsmVariant = 0;
    ShowInst = 0;
    ShowEncoding = 0;
    RelaxAll = 0;
    NoExecStack = 0;
    FatalWarnings = 0;
    DwarfVersion = 3;
  }

  static bool CreateFromArgs(AssemblerInvocation &Res,
                             ArrayRef<const char *> Argv,
                             DiagnosticsEngine &Diags);
};

}

bool AssemblerInvocation::CreateFromArgs(AssemblerInvocation &Opts,
                                         ArrayRef<const char *> Argv,
                                         DiagnosticsEngine &Diags) {
  bool Success = true;

  // Parse the arguments.
  std::unique_ptr<OptTable> OptTbl(createDriverOptTable());

  const unsigned IncludedFlagsBitmask = options::CC1AsOption;
  unsigned MissingArgIndex, MissingArgCount;
  InputArgList Args = OptTbl->ParseArgs(Argv, MissingArgIndex, MissingArgCount,
                                        IncludedFlagsBitmask);

  // Check for missing argument error.
  if (MissingArgCount) {
    Diags.Report(diag::err_drv_missing_argument)
        << Args.getArgString(MissingArgIndex) << MissingArgCount;
    Success = false;
  }

  // Issue errors on unknown arguments.
  for (const Arg *A : Args.filtered(OPT_UNKNOWN)) {
    Diags.Report(diag::err_drv_unknown_argument) << A->getAsString(Args);
    Success = false;
  }

  // Construct the invocation.

  // Target Options
  Opts.Triple = llvm37::Triple::normalize(Args.getLastArgValue(OPT_triple));
  Opts.CPU = Args.getLastArgValue(OPT_target_cpu);
  Opts.Features = Args.getAllArgValues(OPT_target_feature);

  // Use the default target triple if unspecified.
  if (Opts.Triple.empty())
    Opts.Triple = llvm37::sys::getDefaultTargetTriple();

  // Language Options
  Opts.IncludePaths = Args.getAllArgValues(OPT_I);
  Opts.NoInitialTextSection = Args.hasArg(OPT_n);
  Opts.SaveTemporaryLabels = Args.hasArg(OPT_msave_temp_labels);
  Opts.GenDwarfForAssembly = Args.hasArg(OPT_g_Flag);
  Opts.CompressDebugSections = Args.hasArg(OPT_compress_debug_sections);
  if (Args.hasArg(OPT_gdwarf_2))
    Opts.DwarfVersion = 2;
  if (Args.hasArg(OPT_gdwarf_3))
    Opts.DwarfVersion = 3;
  if (Args.hasArg(OPT_gdwarf_4))
    Opts.DwarfVersion = 4;
  Opts.DwarfDebugFlags = Args.getLastArgValue(OPT_dwarf_debug_flags);
  Opts.DwarfDebugProducer = Args.getLastArgValue(OPT_dwarf_debug_producer);
  Opts.DebugCompilationDir = Args.getLastArgValue(OPT_fdebug_compilation_dir);
  Opts.MainFileName = Args.getLastArgValue(OPT_main_file_name);

  // Frontend Options
  if (Args.hasArg(OPT_INPUT)) {
    bool First = true;
    for (arg_iterator it = Args.filtered_begin(OPT_INPUT),
                      ie = Args.filtered_end();
         it != ie; ++it, First = false) {
      const Arg *A = it;
      if (First)
        Opts.InputFile = A->getValue();
      else {
        Diags.Report(diag::err_drv_unknown_argument) << A->getAsString(Args);
        Success = false;
      }
    }
  }
  Opts.LLVM37Args = Args.getAllArgValues(OPT_mllvm37);
  Opts.OutputPath = Args.getLastArgValue(OPT_o);
  if (Arg *A = Args.getLastArg(OPT_filetype)) {
    StringRef Name = A->getValue();
    unsigned OutputType = StringSwitch<unsigned>(Name)
      .Case("asm", FT_Asm)
      .Case("null", FT_Null)
      .Case("obj", FT_Obj)
      .Default(~0U);
    if (OutputType == ~0U) {
      Diags.Report(diag::err_drv_invalid_value) << A->getAsString(Args) << Name;
      Success = false;
    } else
      Opts.OutputType = FileType(OutputType);
  }
  Opts.ShowHelp = Args.hasArg(OPT_help);
  Opts.ShowVersion = Args.hasArg(OPT_version);

  // Transliterate Options
  Opts.OutputAsmVariant =
      getLastArgIntValue(Args, OPT_output_asm_variant, 0, Diags);
  Opts.ShowEncoding = Args.hasArg(OPT_show_encoding);
  Opts.ShowInst = Args.hasArg(OPT_show_inst);

  // Assemble Options
  Opts.RelaxAll = Args.hasArg(OPT_mrelax_all);
  Opts.NoExecStack = Args.hasArg(OPT_mno_exec_stack);
  Opts.FatalWarnings = Args.hasArg(OPT_massembler_fatal_warnings);

  return Success;
}

static std::unique_ptr<raw_fd_ostream>
getOutputStream(AssemblerInvocation &Opts, DiagnosticsEngine &Diags,
                bool Binary) {
  if (Opts.OutputPath.empty())
    Opts.OutputPath = "-";

  // Make sure that the Out file gets unlinked from the disk if we get a
  // SIGINT.
  if (Opts.OutputPath != "-")
    sys::RemoveFileOnSignal(Opts.OutputPath);

  std::error_code EC;
  auto Out = llvm37::make_unique<raw_fd_ostream>(
      Opts.OutputPath, EC, (Binary ? sys::fs::F_None : sys::fs::F_Text));
  if (EC) {
    Diags.Report(diag::err_fe_unable_to_open_output) << Opts.OutputPath
                                                     << EC.message();
    return nullptr;
  }

  return Out;
}

static bool ExecuteAssembler(AssemblerInvocation &Opts,
                             DiagnosticsEngine &Diags) {
  // Get the target specific parser.
  std::string Error;
  const Target *TheTarget = TargetRegistry::lookupTarget(Opts.Triple, Error);
  if (!TheTarget)
    return Diags.Report(diag::err_target_unknown_triple) << Opts.Triple;

  ErrorOr<std::unique_ptr<MemoryBuffer>> Buffer =
      MemoryBuffer::getFileOrSTDIN(Opts.InputFile);

  if (std::error_code EC = Buffer.getError()) {
    Error = EC.message();
    return Diags.Report(diag::err_fe_error_reading) << Opts.InputFile;
  }

  SourceMgr SrcMgr;

  // Tell SrcMgr about this buffer, which is what the parser will pick up.
  SrcMgr.AddNewSourceBuffer(std::move(*Buffer), SMLoc());

  // Record the location of the include directories so that the lexer can find
  // it later.
  SrcMgr.setIncludeDirs(Opts.IncludePaths);

  std::unique_ptr<MCRegisterInfo> MRI(TheTarget->createMCRegInfo(Opts.Triple));
  assert(MRI && "Unable to create target register info!");

  std::unique_ptr<MCAsmInfo> MAI(TheTarget->createMCAsmInfo(*MRI, Opts.Triple));
  assert(MAI && "Unable to create target asm info!");

  // Ensure MCAsmInfo initialization occurs before any use, otherwise sections
  // may be created with a combination of default and explicit settings.
  if (Opts.CompressDebugSections)
    MAI->setCompressDebugSections(true);

  bool IsBinary = Opts.OutputType == AssemblerInvocation::FT_Obj;
  std::unique_ptr<raw_fd_ostream> FDOS = getOutputStream(Opts, Diags, IsBinary);
  if (!FDOS)
    return true;

  // FIXME: This is not pretty. MCContext has a ptr to MCObjectFileInfo and
  // MCObjectFileInfo needs a MCContext reference in order to initialize itself.
  std::unique_ptr<MCObjectFileInfo> MOFI(new MCObjectFileInfo());

  MCContext Ctx(MAI.get(), MRI.get(), MOFI.get(), &SrcMgr);
  // FIXME: Assembler behavior can change with -static.
  MOFI->InitMCObjectFileInfo(Triple(Opts.Triple), Reloc::Default,
                             CodeModel::Default, Ctx);
  if (Opts.SaveTemporaryLabels)
    Ctx.setAllowTemporaryLabels(false);
  if (Opts.GenDwarfForAssembly)
    Ctx.setGenDwarfForAssembly(true);
  if (!Opts.DwarfDebugFlags.empty())
    Ctx.setDwarfDebugFlags(StringRef(Opts.DwarfDebugFlags));
  if (!Opts.DwarfDebugProducer.empty())
    Ctx.setDwarfDebugProducer(StringRef(Opts.DwarfDebugProducer));
  if (!Opts.DebugCompilationDir.empty())
    Ctx.setCompilationDir(Opts.DebugCompilationDir);
  if (!Opts.MainFileName.empty())
    Ctx.setMainFileName(StringRef(Opts.MainFileName));
  Ctx.setDwarfVersion(Opts.DwarfVersion);

  // Build up the feature string from the target feature list.
  std::string FS;
  if (!Opts.Features.empty()) {
    FS = Opts.Features[0];
    for (unsigned i = 1, e = Opts.Features.size(); i != e; ++i)
      FS += "," + Opts.Features[i];
  }

  std::unique_ptr<MCStreamer> Str;

  std::unique_ptr<MCInstrInfo> MCII(TheTarget->createMCInstrInfo());
  std::unique_ptr<MCSubtargetInfo> STI(
      TheTarget->createMCSubtargetInfo(Opts.Triple, Opts.CPU, FS));

  raw_pwrite_stream *Out = FDOS.get();
  std::unique_ptr<buffer_ostream> BOS;

  // FIXME: There is a bit of code duplication with addPassesToEmitFile.
  if (Opts.OutputType == AssemblerInvocation::FT_Asm) {
    MCInstPrinter *IP = TheTarget->createMCInstPrinter(
        llvm37::Triple(Opts.Triple), Opts.OutputAsmVariant, *MAI, *MCII, *MRI);
    MCCodeEmitter *CE = nullptr;
    MCAsmBackend *MAB = nullptr;
    if (Opts.ShowEncoding) {
      CE = TheTarget->createMCCodeEmitter(*MCII, *MRI, Ctx);
      MAB = TheTarget->createMCAsmBackend(*MRI, Opts.Triple, Opts.CPU);
    }
    auto FOut = llvm37::make_unique<formatted_raw_ostream>(*Out);
    Str.reset(TheTarget->createAsmStreamer(
        Ctx, std::move(FOut), /*asmverbose*/ true,
        /*useDwarfDirectory*/ true, IP, CE, MAB, Opts.ShowInst));
  } else if (Opts.OutputType == AssemblerInvocation::FT_Null) {
    Str.reset(createNullStreamer(Ctx));
  } else {
    assert(Opts.OutputType == AssemblerInvocation::FT_Obj &&
           "Invalid file type!");
    if (!FDOS->supportsSeeking()) {
      BOS = make_unique<buffer_ostream>(*FDOS);
      Out = BOS.get();
    }

    MCCodeEmitter *CE = TheTarget->createMCCodeEmitter(*MCII, *MRI, Ctx);
    MCAsmBackend *MAB = TheTarget->createMCAsmBackend(*MRI, Opts.Triple,
                                                      Opts.CPU);
    Triple T(Opts.Triple);
    Str.reset(TheTarget->createMCObjectStreamer(T, Ctx, *MAB, *Out, CE, *STI,
                                                Opts.RelaxAll,
                                                /*DWARFMustBeAtTheEnd*/ true));
    Str.get()->InitSections(Opts.NoExecStack);
  }

  bool Failed = false;

  std::unique_ptr<MCAsmParser> Parser(
      createMCAsmParser(SrcMgr, Ctx, *Str.get(), *MAI));

  // FIXME: init MCTargetOptions from sanitizer flags here.
  MCTargetOptions Options;
  std::unique_ptr<MCTargetAsmParser> TAP(
      TheTarget->createMCAsmParser(*STI, *Parser, *MCII, Options));
  if (!TAP)
    Failed = Diags.Report(diag::err_target_unknown_triple) << Opts.Triple;

  if (!Failed) {
    Parser->setTargetParser(*TAP.get());
    Failed = Parser->Run(Opts.NoInitialTextSection);
  }

  // Close Streamer first.
  // It might have a reference to the output stream.
  Str.reset();
  // Close the output stream early.
  BOS.reset();
  FDOS.reset();

  // Delete output file if there were errors.
  if (Failed && Opts.OutputPath != "-")
    sys::fs::remove(Opts.OutputPath);

  return Failed;
}

static void LLVM37ErrorHandler(void *UserData, const std::string &Message,
                             bool GenCrashDiag) {
  DiagnosticsEngine &Diags = *static_cast<DiagnosticsEngine*>(UserData);

  Diags.Report(diag::err_fe_error_backend) << Message;

  // We cannot recover from llvm37 errors.
  exit(1);
}

int cc1as_main(ArrayRef<const char *> Argv, const char *Argv0, void *MainAddr) {
  // Print a stack trace if we signal out.
  sys::PrintStackTraceOnErrorSignal();
  PrettyStackTraceProgram X(Argv.size(), Argv.data());
  llvm37_shutdown_obj Y;  // Call llvm37_shutdown() on exit.

  // Initialize targets and assembly printers/parsers.
  InitializeAllTargetInfos();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();

  // Construct our diagnostic client.
  IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts = new DiagnosticOptions();
  TextDiagnosticPrinter *DiagClient
    = new TextDiagnosticPrinter(errs(), &*DiagOpts);
  DiagClient->setPrefix("clang -cc1as");
  IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());
  DiagnosticsEngine Diags(DiagID, &*DiagOpts, DiagClient);

  // Set an error handler, so that any LLVM37 backend diagnostics go through our
  // error handler.
  ScopedFatalErrorHandler FatalErrorHandler
    (LLVM37ErrorHandler, static_cast<void*>(&Diags));

  // Parse the arguments.
  AssemblerInvocation Asm;
  if (!AssemblerInvocation::CreateFromArgs(Asm, Argv, Diags))
    return 1;

  if (Asm.ShowHelp) {
    std::unique_ptr<OptTable> Opts(driver::createDriverOptTable());
    Opts->PrintHelp(llvm37::outs(), "clang -cc1as", "Clang Integrated Assembler",
                    /*Include=*/driver::options::CC1AsOption, /*Exclude=*/0);
    return 0;
  }

  // Honor -version.
  //
  // FIXME: Use a better -version message?
  if (Asm.ShowVersion) {
    llvm37::cl::PrintVersionMessage();
    return 0;
  }

  // Honor -mllvm37.
  //
  // FIXME: Remove this, one day.
  if (!Asm.LLVM37Args.empty()) {
    unsigned NumArgs = Asm.LLVM37Args.size();
    const char **Args = new const char*[NumArgs + 2];
    Args[0] = "clang (LLVM37 option parsing)";
    for (unsigned i = 0; i != NumArgs; ++i)
      Args[i + 1] = Asm.LLVM37Args[i].c_str();
    Args[NumArgs + 1] = nullptr;
    llvm37::cl::ParseCommandLineOptions(NumArgs + 1, Args);
  }

  // Execute the invocation, unless there were parsing errors.
  bool Failed = Diags.hasErrorOccurred() || ExecuteAssembler(Asm, Diags);

  // If any timers were active but haven't been destroyed yet, print their
  // results now.
  TimerGroup::printAll(errs());

  return !!Failed;
}

