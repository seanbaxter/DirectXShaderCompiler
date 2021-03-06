//===- TestingSupport.cpp - Convert objects files into test files --------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Object/ObjectFile.h"
#include "llvm37/Support/CommandLine.h"
#include "llvm37/Support/LEB128.h"
#include "llvm37/Support/ManagedStatic.h"
#include "llvm37/Support/PrettyStackTrace.h"
#include "llvm37/Support/Signals.h"
#include "llvm37/Support/raw_ostream.h"
#include <functional>
#include <system_error>

using namespace llvm37;
using namespace object;

int convertForTestingMain(int argc, const char *argv[]) {
  sys::PrintStackTraceOnErrorSignal();
  PrettyStackTraceProgram X(argc, argv);
  llvm37_shutdown_obj Y; // Call llvm37_shutdown() on exit.

  cl::opt<std::string> InputSourceFile(cl::Positional, cl::Required,
                                       cl::desc("<Source file>"));

  cl::opt<std::string> OutputFilename(
      "o", cl::Required,
      cl::desc(
          "File with the profile data obtained after an instrumented run"));

  cl::ParseCommandLineOptions(argc, argv, "LLVM37 code coverage tool\n");

  auto ObjErr = llvm37::object::ObjectFile::createObjectFile(InputSourceFile);
  if (auto Err = ObjErr.getError()) {
    errs() << "error: " << Err.message() << "\n";
    return 1;
  }
  ObjectFile *OF = ObjErr.get().getBinary();
  auto BytesInAddress = OF->getBytesInAddress();
  if (BytesInAddress != 8) {
    errs() << "error: 64 bit binary expected\n";
    return 1;
  }

  // Look for the sections that we are interested in.
  int FoundSectionCount = 0;
  SectionRef ProfileNames, CoverageMapping;
  for (const auto &Section : OF->sections()) {
    StringRef Name;
    if (Section.getName(Name))
      return 1;
    if (Name == "__llvm37_prf_names") {
      ProfileNames = Section;
    } else if (Name == "__llvm37_covmap") {
      CoverageMapping = Section;
    } else
      continue;
    ++FoundSectionCount;
  }
  if (FoundSectionCount != 2)
    return 1;

  // Get the contents of the given sections.
  uint64_t ProfileNamesAddress = ProfileNames.getAddress();
  StringRef CoverageMappingData;
  StringRef ProfileNamesData;
  if (CoverageMapping.getContents(CoverageMappingData) ||
      ProfileNames.getContents(ProfileNamesData))
    return 1;

  int FD;
  if (auto Err =
          sys::fs::openFileForWrite(OutputFilename, FD, sys::fs::F_None)) {
    errs() << "error: " << Err.message() << "\n";
    return 1;
  }

  raw_fd_ostream OS(FD, true);
  OS << "llvm37covmtestdata";
  encodeULEB128(ProfileNamesData.size(), OS);
  encodeULEB128(ProfileNamesAddress, OS);
  OS << ProfileNamesData << CoverageMappingData;

  return 0;
}
