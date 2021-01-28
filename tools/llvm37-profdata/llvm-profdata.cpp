//===- llvm37-profdata.cpp - LLVM37 profile data tool -------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// llvm37-profdata merges .profdata files.
//
//===----------------------------------------------------------------------===//

#include "llvm37/ADT/StringRef.h"
#include "llvm37/IR/LLVMContext.h"
#include "llvm37/ProfileData/InstrProfReader.h"
#include "llvm37/ProfileData/InstrProfWriter.h"
#include "llvm37/ProfileData/SampleProfReader.h"
#include "llvm37/ProfileData/SampleProfWriter.h"
#include "llvm37/Support/CommandLine.h"
#include "llvm37/Support/FileSystem.h"
#include "llvm37/Support/Format.h"
#include "llvm37/Support/ManagedStatic.h"
#include "llvm37/Support/MemoryBuffer.h"
#include "llvm37/Support/Path.h"
#include "llvm37/Support/PrettyStackTrace.h"
#include "llvm37/Support/Signals.h"
#include "llvm37/Support/raw_ostream.h"

using namespace llvm37;

static void exitWithError(const Twine &Message, StringRef Whence = "") {
  errs() << "error: ";
  if (!Whence.empty())
    errs() << Whence << ": ";
  errs() << Message << "\n";
  ::exit(1);
}

namespace {
enum ProfileKinds { instr, sample };
}

static void mergeInstrProfile(const cl::list<std::string> &Inputs,
                              StringRef OutputFilename) {
  if (OutputFilename.compare("-") == 0)
    exitWithError("Cannot write indexed profdata format to stdout.");

  std::error_code EC;
  raw_fd_ostream Output(OutputFilename.data(), EC, sys::fs::F_None);
  if (EC)
    exitWithError(EC.message(), OutputFilename);

  InstrProfWriter Writer;
  for (const auto &Filename : Inputs) {
    auto ReaderOrErr = InstrProfReader::create(Filename);
    if (std::error_code ec = ReaderOrErr.getError())
      exitWithError(ec.message(), Filename);

    auto Reader = std::move(ReaderOrErr.get());
    for (const auto &I : *Reader)
      if (std::error_code EC =
              Writer.addFunctionCounts(I.Name, I.Hash, I.Counts))
        errs() << Filename << ": " << I.Name << ": " << EC.message() << "\n";
    if (Reader->hasError())
      exitWithError(Reader->getError().message(), Filename);
  }
  Writer.write(Output);
}

static void mergeSampleProfile(const cl::list<std::string> &Inputs,
                               StringRef OutputFilename,
                               sampleprof::SampleProfileFormat OutputFormat) {
  using namespace sampleprof;
  auto WriterOrErr = SampleProfileWriter::create(OutputFilename, OutputFormat);
  if (std::error_code EC = WriterOrErr.getError())
    exitWithError(EC.message(), OutputFilename);

  auto Writer = std::move(WriterOrErr.get());
  StringMap<FunctionSamples> ProfileMap;
  for (const auto &Filename : Inputs) {
    auto ReaderOrErr =
        SampleProfileReader::create(Filename, getGlobalContext());
    if (std::error_code EC = ReaderOrErr.getError())
      exitWithError(EC.message(), Filename);

    auto Reader = std::move(ReaderOrErr.get());
    if (std::error_code EC = Reader->read())
      exitWithError(EC.message(), Filename);

    StringMap<FunctionSamples> &Profiles = Reader->getProfiles();
    for (StringMap<FunctionSamples>::iterator I = Profiles.begin(),
                                              E = Profiles.end();
         I != E; ++I) {
      StringRef FName = I->first();
      FunctionSamples &Samples = I->second;
      ProfileMap[FName].merge(Samples);
    }
  }
  Writer->write(ProfileMap);
}

static int merge_main(int argc, const char *argv[]) {
  cl::list<std::string> Inputs(cl::Positional, cl::Required, cl::OneOrMore,
                               cl::desc("<filenames...>"));

  cl::opt<std::string> OutputFilename("output", cl::value_desc("output"),
                                      cl::init("-"), cl::Required,
                                      cl::desc("Output file"));
  cl::alias OutputFilenameA("o", cl::desc("Alias for --output"),
                            cl::aliasopt(OutputFilename));
  cl::opt<ProfileKinds> ProfileKind(
      cl::desc("Profile kind:"), cl::init(instr),
      cl::values(clEnumVal(instr, "Instrumentation profile (default)"),
                 clEnumVal(sample, "Sample profile"), clEnumValEnd));

  cl::opt<sampleprof::SampleProfileFormat> OutputFormat(
      cl::desc("Format of output profile (only meaningful with --sample)"),
      cl::init(sampleprof::SPF_Binary),
      cl::values(clEnumValN(sampleprof::SPF_Binary, "binary",
                            "Binary encoding (default)"),
                 clEnumValN(sampleprof::SPF_Text, "text", "Text encoding"),
                 clEnumValN(sampleprof::SPF_GCC, "gcc", "GCC encoding"),
                 clEnumValEnd));

  cl::ParseCommandLineOptions(argc, argv, "LLVM37 profile data merger\n");

  if (ProfileKind == instr)
    mergeInstrProfile(Inputs, OutputFilename);
  else
    mergeSampleProfile(Inputs, OutputFilename, OutputFormat);

  return 0;
}

static int showInstrProfile(std::string Filename, bool ShowCounts,
                            bool ShowAllFunctions, std::string ShowFunction,
                            raw_fd_ostream &OS) {
  auto ReaderOrErr = InstrProfReader::create(Filename);
  if (std::error_code EC = ReaderOrErr.getError())
    exitWithError(EC.message(), Filename);

  auto Reader = std::move(ReaderOrErr.get());
  uint64_t MaxFunctionCount = 0, MaxBlockCount = 0;
  size_t ShownFunctions = 0, TotalFunctions = 0;
  for (const auto &Func : *Reader) {
    bool Show =
        ShowAllFunctions || (!ShowFunction.empty() &&
                             Func.Name.find(ShowFunction) != Func.Name.npos);

    ++TotalFunctions;
    assert(Func.Counts.size() > 0 && "function missing entry counter");
    if (Func.Counts[0] > MaxFunctionCount)
      MaxFunctionCount = Func.Counts[0];

    if (Show) {
      if (!ShownFunctions)
        OS << "Counters:\n";
      ++ShownFunctions;

      OS << "  " << Func.Name << ":\n"
         << "    Hash: " << format("0x%016" PRIx64, Func.Hash) << "\n"
         << "    Counters: " << Func.Counts.size() << "\n"
         << "    Function count: " << Func.Counts[0] << "\n";
    }

    if (Show && ShowCounts)
      OS << "    Block counts: [";
    for (size_t I = 1, E = Func.Counts.size(); I < E; ++I) {
      if (Func.Counts[I] > MaxBlockCount)
        MaxBlockCount = Func.Counts[I];
      if (Show && ShowCounts)
        OS << (I == 1 ? "" : ", ") << Func.Counts[I];
    }
    if (Show && ShowCounts)
      OS << "]\n";
  }
  if (Reader->hasError())
    exitWithError(Reader->getError().message(), Filename);

  if (ShowAllFunctions || !ShowFunction.empty())
    OS << "Functions shown: " << ShownFunctions << "\n";
  OS << "Total functions: " << TotalFunctions << "\n";
  OS << "Maximum function count: " << MaxFunctionCount << "\n";
  OS << "Maximum internal block count: " << MaxBlockCount << "\n";
  return 0;
}

static int showSampleProfile(std::string Filename, bool ShowCounts,
                             bool ShowAllFunctions, std::string ShowFunction,
                             raw_fd_ostream &OS) {
  using namespace sampleprof;
  auto ReaderOrErr = SampleProfileReader::create(Filename, getGlobalContext());
  if (std::error_code EC = ReaderOrErr.getError())
    exitWithError(EC.message(), Filename);

  auto Reader = std::move(ReaderOrErr.get());
  Reader->read();
  if (ShowAllFunctions || ShowFunction.empty())
    Reader->dump(OS);
  else
    Reader->dumpFunctionProfile(ShowFunction, OS);

  return 0;
}

static int show_main(int argc, const char *argv[]) {
  cl::opt<std::string> Filename(cl::Positional, cl::Required,
                                cl::desc("<profdata-file>"));

  cl::opt<bool> ShowCounts("counts", cl::init(false),
                           cl::desc("Show counter values for shown functions"));
  cl::opt<bool> ShowAllFunctions("all-functions", cl::init(false),
                                 cl::desc("Details for every function"));
  cl::opt<std::string> ShowFunction("function",
                                    cl::desc("Details for matching functions"));

  cl::opt<std::string> OutputFilename("output", cl::value_desc("output"),
                                      cl::init("-"), cl::desc("Output file"));
  cl::alias OutputFilenameA("o", cl::desc("Alias for --output"),
                            cl::aliasopt(OutputFilename));
  cl::opt<ProfileKinds> ProfileKind(
      cl::desc("Profile kind:"), cl::init(instr),
      cl::values(clEnumVal(instr, "Instrumentation profile (default)"),
                 clEnumVal(sample, "Sample profile"), clEnumValEnd));

  cl::ParseCommandLineOptions(argc, argv, "LLVM37 profile data summary\n");

  if (OutputFilename.empty())
    OutputFilename = "-";

  std::error_code EC;
  raw_fd_ostream OS(OutputFilename.data(), EC, sys::fs::F_Text);
  if (EC)
    exitWithError(EC.message(), OutputFilename);

  if (ShowAllFunctions && !ShowFunction.empty())
    errs() << "warning: -function argument ignored: showing all functions\n";

  if (ProfileKind == instr)
    return showInstrProfile(Filename, ShowCounts, ShowAllFunctions,
                            ShowFunction, OS);
  else
    return showSampleProfile(Filename, ShowCounts, ShowAllFunctions,
                             ShowFunction, OS);
}

int main(int argc, const char *argv[]) {
  // Print a stack trace if we signal out.
  sys::PrintStackTraceOnErrorSignal();
  PrettyStackTraceProgram X(argc, argv);
  llvm37_shutdown_obj Y; // Call llvm37_shutdown() on exit.

  StringRef ProgName(sys::path::filename(argv[0]));
  if (argc > 1) {
    int (*func)(int, const char *[]) = nullptr;

    if (strcmp(argv[1], "merge") == 0)
      func = merge_main;
    else if (strcmp(argv[1], "show") == 0)
      func = show_main;

    if (func) {
      std::string Invocation(ProgName.str() + " " + argv[1]);
      argv[1] = Invocation.c_str();
      return func(argc - 1, argv + 1);
    }

    if (strcmp(argv[1], "-h") == 0 ||
        strcmp(argv[1], "-help") == 0 ||
        strcmp(argv[1], "--help") == 0) {

      errs() << "OVERVIEW: LLVM37 profile data tools\n\n"
             << "USAGE: " << ProgName << " <command> [args...]\n"
             << "USAGE: " << ProgName << " <command> -help\n\n"
             << "Available commands: merge, show\n";
      return 0;
    }
  }

  if (argc < 2)
    errs() << ProgName << ": No command specified!\n";
  else
    errs() << ProgName << ": Unknown command!\n";

  errs() << "USAGE: " << ProgName << " <merge|show> [args...]\n";
  return 1;
}
