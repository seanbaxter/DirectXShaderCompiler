//===- TreeView.cpp - diagtool tool for printing warning flags ------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "DiagTool.h"
#include "DiagnosticNames.h"
#include "clang/AST/ASTDiagnostic.h"
#include "clang/Basic/AllDiagnostics.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "llvm37/ADT/DenseSet.h"
#include "llvm37/ADT/StringMap.h"
#include "llvm37/Support/Format.h"
#include "llvm37/Support/Process.h"

DEF_DIAGTOOL("tree", "Show warning flags in a tree view", TreeView)

using namespace clang;
using namespace diagtool;

static bool hasColors(const llvm37::raw_ostream &out) {
  if (&out != &llvm37::errs() && &out != &llvm37::outs())
    return false;
  return llvm37::errs().is_displayed() && llvm37::outs().is_displayed();
}

class TreePrinter {
public:
  llvm37::raw_ostream &out;
  const bool ShowColors;
  bool FlagsOnly;

  TreePrinter(llvm37::raw_ostream &out)
      : out(out), ShowColors(hasColors(out)), FlagsOnly(false) {}

  void setColor(llvm37::raw_ostream::Colors Color) {
    if (ShowColors)
      out << llvm37::sys::Process::OutputColor(Color, false, false);
  }

  void resetColor() {
    if (ShowColors)
      out << llvm37::sys::Process::ResetColor();
  }

  static bool isIgnored(unsigned DiagID) {
    // FIXME: This feels like a hack.
    static clang::DiagnosticsEngine Diags(new DiagnosticIDs,
                                          new DiagnosticOptions);
    return Diags.isIgnored(DiagID, SourceLocation());
  }

  void printGroup(const GroupRecord &Group, unsigned Indent = 0) {
    out.indent(Indent * 2);

    setColor(llvm37::raw_ostream::YELLOW);
    out << "-W" << Group.getName() << "\n";
    resetColor();

    ++Indent;
    for (GroupRecord::subgroup_iterator I = Group.subgroup_begin(),
                                        E = Group.subgroup_end();
         I != E; ++I) {
      printGroup(*I, Indent);
    }

    if (!FlagsOnly) {
      for (GroupRecord::diagnostics_iterator I = Group.diagnostics_begin(),
                                             E = Group.diagnostics_end();
           I != E; ++I) {
        if (ShowColors && !isIgnored(I->DiagID))
          setColor(llvm37::raw_ostream::GREEN);
        out.indent(Indent * 2);
        out << I->getName();
        resetColor();
        out << "\n";
      }
    }
  }

  int showGroup(StringRef RootGroup) {
    ArrayRef<GroupRecord> AllGroups = getDiagnosticGroups();

    if (RootGroup.size() > UINT16_MAX) {
      llvm37::errs() << "No such diagnostic group exists\n";
      return 1;
    }

    const GroupRecord *Found =
        std::lower_bound(AllGroups.begin(), AllGroups.end(), RootGroup);

    if (Found == AllGroups.end() || Found->getName() != RootGroup) {
      llvm37::errs() << "No such diagnostic group exists\n";
      return 1;
    }

    printGroup(*Found);

    return 0;
  }

  int showAll() {
    ArrayRef<GroupRecord> AllGroups = getDiagnosticGroups();
    llvm37::DenseSet<unsigned> NonRootGroupIDs;

    for (ArrayRef<GroupRecord>::iterator I = AllGroups.begin(),
                                         E = AllGroups.end();
         I != E; ++I) {
      for (GroupRecord::subgroup_iterator SI = I->subgroup_begin(),
                                          SE = I->subgroup_end();
           SI != SE; ++SI) {
        NonRootGroupIDs.insert((unsigned)SI.getID());
      }
    }

    assert(NonRootGroupIDs.size() < AllGroups.size());

    for (unsigned i = 0, e = AllGroups.size(); i != e; ++i) {
      if (!NonRootGroupIDs.count(i))
        printGroup(AllGroups[i]);
    }

    return 0;
  }

  void showKey() {
    if (ShowColors) {
      out << '\n';
      setColor(llvm37::raw_ostream::GREEN);
      out << "GREEN";
      resetColor();
      out << " = enabled by default\n\n";
    }
  }
};

static void printUsage() {
  llvm37::errs() << "Usage: diagtool tree [--flags-only] [<diagnostic-group>]\n";
}

int TreeView::run(unsigned int argc, char **argv, llvm37::raw_ostream &out) {
  // First check our one flag (--flags-only).
  bool FlagsOnly = false;
  if (argc > 0) {
    StringRef FirstArg(*argv);
    if (FirstArg.equals("--flags-only")) {
      FlagsOnly = true;
      --argc;
      ++argv;
    }
  }

  bool ShowAll = false;
  StringRef RootGroup;

  switch (argc) {
  case 0:
    ShowAll = true;
    break;
  case 1:
    RootGroup = argv[0];
    if (RootGroup.startswith("-W"))
      RootGroup = RootGroup.substr(2);
    if (RootGroup == "everything")
      ShowAll = true;
    // FIXME: Handle other special warning flags, like -pedantic.
    break;
  default:
    printUsage();
    return -1;
  }

  TreePrinter TP(out);
  TP.FlagsOnly = FlagsOnly;
  TP.showKey();
  return ShowAll ? TP.showAll() : TP.showGroup(RootGroup);
}
