//===- ListWarnings.h - diagtool tool for printing warning flags ----------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides a diagtool tool that displays warning flags for
// diagnostics.
//
//===----------------------------------------------------------------------===//

#include "DiagTool.h"
#include "DiagnosticNames.h"
#include "clang/AST/ASTDiagnostic.h"
#include "clang/Basic/AllDiagnostics.h"
#include "clang/Basic/Diagnostic.h"
#include "llvm37/ADT/StringMap.h"
#include "llvm37/Support/Format.h"

DEF_DIAGTOOL("list-warnings",
             "List warnings and their corresponding flags",
             ListWarnings)
  
using namespace clang;
using namespace diagtool;

namespace {
struct Entry {
  llvm37::StringRef DiagName;
  llvm37::StringRef Flag;
  
  Entry(llvm37::StringRef diagN, llvm37::StringRef flag)
    : DiagName(diagN), Flag(flag) {}
  
  bool operator<(const Entry &x) const { return DiagName < x.DiagName; }
};
}

static void printEntries(std::vector<Entry> &entries, llvm37::raw_ostream &out) {
  for (std::vector<Entry>::iterator it = entries.begin(), ei = entries.end();
       it != ei; ++it) {
    out << "  " << it->DiagName;
    if (!it->Flag.empty())
      out << " [-W" << it->Flag << "]";
    out << '\n';
  }
}

int ListWarnings::run(unsigned int argc, char **argv, llvm37::raw_ostream &out) {
  std::vector<Entry> Flagged, Unflagged;
  llvm37::StringMap<std::vector<unsigned> > flagHistogram;
  
  ArrayRef<DiagnosticRecord> AllDiagnostics = getBuiltinDiagnosticsByName();

  for (ArrayRef<DiagnosticRecord>::iterator di = AllDiagnostics.begin(),
                                            de = AllDiagnostics.end();
       di != de; ++di) {
    unsigned diagID = di->DiagID;
    
    if (DiagnosticIDs::isBuiltinNote(diagID))
      continue;
        
    if (!DiagnosticIDs::isBuiltinWarningOrExtension(diagID))
      continue;
  
    Entry entry(di->getName(),
                DiagnosticIDs::getWarningOptionForDiag(diagID));
    
    if (entry.Flag.empty())
      Unflagged.push_back(entry);
    else {
      Flagged.push_back(entry);
      flagHistogram[entry.Flag].push_back(diagID);
    }
  }
  
  out << "Warnings with flags (" << Flagged.size() << "):\n";
  printEntries(Flagged, out);
  
  out << "Warnings without flags (" << Unflagged.size() << "):\n";
  printEntries(Unflagged, out);

  out << "\nSTATISTICS:\n\n";

  double percentFlagged = ((double) Flagged.size()) 
    / (Flagged.size() + Unflagged.size()) * 100.0;
  
  out << "  Percentage of warnings with flags: " 
      << llvm37::format("%.4g",percentFlagged) << "%\n";
  
  out << "  Number of unique flags: "
      << flagHistogram.size() << '\n';
  
  double avgDiagsPerFlag = (double) Flagged.size() / flagHistogram.size();
  out << "  Average number of diagnostics per flag: "
      << llvm37::format("%.4g", avgDiagsPerFlag) << '\n';

  out << "  Number in -Wpedantic (not covered by other -W flags): "
      << flagHistogram["pedantic"].size() << '\n';

  out << '\n';
  
  return 0;
}

