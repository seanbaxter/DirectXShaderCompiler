//===- fpcmp.cpp - A fuzzy "cmp" that permits floating point noise --------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// fpcmp is a tool that basically works like the 'cmp' tool, except that it can
// tolerate errors due to floating point noise, with the -r and -a options.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Support/CommandLine.h"
#include "llvm37/Support/FileUtilities.h"
#include "llvm37/Support/raw_ostream.h"
using namespace llvm37;

namespace {
  cl::opt<std::string>
  File1(cl::Positional, cl::desc("<input file #1>"), cl::Required);
  cl::opt<std::string>
  File2(cl::Positional, cl::desc("<input file #2>"), cl::Required);

  cl::opt<double>
  RelTolerance("r", cl::desc("Relative error tolerated"), cl::init(0));
  cl::opt<double>
  AbsTolerance("a", cl::desc("Absolute error tolerated"), cl::init(0));
}

int main(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv);

  std::string ErrorMsg;
  int DF = DiffFilesWithTolerance(File1, File2, AbsTolerance, RelTolerance,
                                  &ErrorMsg);
  if (!ErrorMsg.empty())
    errs() << argv[0] << ": " << ErrorMsg << "\n";
  return DF;
}

