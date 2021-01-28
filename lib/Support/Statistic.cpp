//===-- Statistic.cpp - Easy way to expose stats information --------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the 'Statistic' class, which is designed to be an easy
// way to expose various success metrics from passes.  These statistics are
// printed at the end of a run, when the -stats command line option is enabled
// on the command line.
//
// This is useful for reporting information like the number of instructions
// simplified, optimized or removed by various transformations, like this:
//
// static Statistic NumInstEliminated("GCSE", "Number of instructions killed");
//
// Later, in the code: ++NumInstEliminated;
//
//===----------------------------------------------------------------------===//

#include "llvm37/ADT/Statistic.h"
#include "llvm37/ADT/StringExtras.h"
#include "llvm37/Support/CommandLine.h"
#include "llvm37/Support/Debug.h"
#include "llvm37/Support/Format.h"
#include "llvm37/Support/ManagedStatic.h"
#include "llvm37/Support/Mutex.h"
#include "llvm37/Support/raw_ostream.h"
#include <algorithm>
#include <cstring>
using namespace llvm37;

// CreateInfoOutputFile - Return a file stream to print our output on.
namespace llvm37 { extern raw_ostream *CreateInfoOutputFile(); }

/// -stats - Command line option to cause transformations to emit stats about
/// what they did.
///
#if 0 // HLSL Change Starts - option pending
static cl::opt<bool>
Enabled(
    "stats",
    cl::desc("Enable statistics output from program (available with Asserts)"));
#else
static const bool Enabled = false;
#endif // HLSL Change Ends

namespace {
/// StatisticInfo - This class is used in a ManagedStatic so that it is created
/// on demand (when the first statistic is bumped) and destroyed only when
/// llvm37_shutdown is called.  We print statistics from the destructor.
class StatisticInfo {
  std::vector<const Statistic*> Stats;
  friend void llvm37::PrintStatistics();
  friend void llvm37::PrintStatistics(raw_ostream &OS);
public:
  ~StatisticInfo();

  void addStatistic(const Statistic *S) {
    Stats.push_back(S);
  }
};
}

static ManagedStatic<StatisticInfo> StatInfo;
static ManagedStatic<sys::SmartMutex<true> > StatLock;

/// RegisterStatistic - The first time a statistic is bumped, this method is
/// called.
void Statistic::RegisterStatistic() {
  // If stats are enabled, inform StatInfo that this statistic should be
  // printed.
  sys::SmartScopedLock<true> Writer(*StatLock);
  if (!Initialized) {
    if (Enabled)
      StatInfo->addStatistic(this);

    TsanHappensBefore(this);
    sys::MemoryFence();
    // Remember we have been registered.
    TsanIgnoreWritesBegin();
    Initialized = true;
    TsanIgnoreWritesEnd();
  }
}

// Print information when destroyed, iff command line option is specified.
StatisticInfo::~StatisticInfo() {
  llvm37::PrintStatistics();
}

void llvm37::EnableStatistics() {
  //Enabled.setValue(true); // HLSL Change
}

bool llvm37::AreStatisticsEnabled() {
  return Enabled;
}

void llvm37::PrintStatistics(raw_ostream &OS) {
  StatisticInfo &Stats = *StatInfo;

  // Figure out how long the biggest Value and Name fields are.
  unsigned MaxNameLen = 0, MaxValLen = 0;
  for (size_t i = 0, e = Stats.Stats.size(); i != e; ++i) {
    MaxValLen = std::max(MaxValLen,
                         (unsigned)utostr(Stats.Stats[i]->getValue()).size());
    MaxNameLen = std::max(MaxNameLen,
                          (unsigned)std::strlen(Stats.Stats[i]->getName()));
  }

  // Sort the fields by name.
  std::stable_sort(Stats.Stats.begin(), Stats.Stats.end(),
                   [](const Statistic *LHS, const Statistic *RHS) {
    if (int Cmp = std::strcmp(LHS->getName(), RHS->getName()))
      return Cmp < 0;

    // Secondary key is the description.
    return std::strcmp(LHS->getDesc(), RHS->getDesc()) < 0;
  });

  // Print out the statistics header...
  OS << "===" << std::string(73, '-') << "===\n"
     << "                          ... Statistics Collected ...\n"
     << "===" << std::string(73, '-') << "===\n\n";

  // Print all of the statistics.
  for (size_t i = 0, e = Stats.Stats.size(); i != e; ++i)
    OS << format("%*u %-*s - %s\n",
                 MaxValLen, Stats.Stats[i]->getValue(),
                 MaxNameLen, Stats.Stats[i]->getName(),
                 Stats.Stats[i]->getDesc());

  OS << '\n';  // Flush the output stream.
  OS.flush();

}

void llvm37::PrintStatistics() {
#if !defined(NDEBUG) || defined(LLVM37_ENABLE_STATS)
  StatisticInfo &Stats = *StatInfo;

  // Statistics not enabled?
  if (Stats.Stats.empty()) return;

  // Get the stream to write to.
  raw_ostream &OutStream = *CreateInfoOutputFile();
  PrintStatistics(OutStream);
  delete &OutStream;   // Close the file.
#else
  // Check if the -stats option is set instead of checking
  // !Stats.Stats.empty().  In release builds, Statistics operators
  // do nothing, so stats are never Registered.
  if (Enabled) {
    // Get the stream to write to.
    raw_ostream &OutStream = *CreateInfoOutputFile();
    OutStream << "Statistics are disabled.  "
            << "Build with asserts or with -DLLVM37_ENABLE_STATS\n";
    OutStream.flush();
    delete &OutStream;   // Close the file.
  }
#endif
}
