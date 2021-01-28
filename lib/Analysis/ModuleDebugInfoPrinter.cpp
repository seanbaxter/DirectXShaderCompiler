//===-- ModuleDebugInfoPrinter.cpp - Prints module debug info metadata ----===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This pass decodes the debug info metadata in a module and prints in a
// (sufficiently-prepared-) human-readable form.
//
// For example, run this pass from opt along with the -analyze option, and
// it'll print to standard output.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Analysis/Passes.h"
#include "llvm37/ADT/Statistic.h"
#include "llvm37/IR/DebugInfo.h"
#include "llvm37/IR/Function.h"
#include "llvm37/Pass.h"
#include "llvm37/Support/ErrorHandling.h"
#include "llvm37/Support/raw_ostream.h"
using namespace llvm37;

namespace {
  class ModuleDebugInfoPrinter : public ModulePass {
    DebugInfoFinder Finder;
  public:
    static char ID; // Pass identification, replacement for typeid
    ModuleDebugInfoPrinter() : ModulePass(ID) {
      initializeModuleDebugInfoPrinterPass(*PassRegistry::getPassRegistry());
    }

    bool runOnModule(Module &M) override;

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
    }
    void print(raw_ostream &O, const Module *M) const override;
  };
}

char ModuleDebugInfoPrinter::ID = 0;
INITIALIZE_PASS(ModuleDebugInfoPrinter, "module-debuginfo",
                "Decodes module-level debug info", false, true)

ModulePass *llvm37::createModuleDebugInfoPrinterPass() {
  return new ModuleDebugInfoPrinter();
}

bool ModuleDebugInfoPrinter::runOnModule(Module &M) {
  Finder.processModule(M);
  return false;
}

static void printFile(raw_ostream &O, StringRef Filename, StringRef Directory,
                      unsigned Line = 0) {
  if (Filename.empty())
    return;

  O << " from ";
  if (!Directory.empty())
    O << Directory << "/";
  O << Filename;
  if (Line)
    O << ":" << Line;
}

void ModuleDebugInfoPrinter::print(raw_ostream &O, const Module *M) const {
  // Printing the nodes directly isn't particularly helpful (since they
  // reference other nodes that won't be printed, particularly for the
  // filenames), so just print a few useful things.
  for (DICompileUnit *CU : Finder.compile_units()) {
    O << "Compile unit: ";
    if (const char *Lang = dwarf::LanguageString(CU->getSourceLanguage()))
      O << Lang;
    else
      O << "unknown-language(" << CU->getSourceLanguage() << ")";
    printFile(O, CU->getFilename(), CU->getDirectory());
    O << '\n';
  }

  for (DISubprogram *S : Finder.subprograms()) {
    O << "Subprogram: " << S->getName();
    printFile(O, S->getFilename(), S->getDirectory(), S->getLine());
    if (!S->getLinkageName().empty())
      O << " ('" << S->getLinkageName() << "')";
    O << '\n';
  }

  for (const DIGlobalVariable *GV : Finder.global_variables()) {
    O << "Global variable: " << GV->getName();
    printFile(O, GV->getFilename(), GV->getDirectory(), GV->getLine());
    if (!GV->getLinkageName().empty())
      O << " ('" << GV->getLinkageName() << "')";
    O << '\n';
  }

  for (const DIType *T : Finder.types()) {
    O << "Type:";
    if (!T->getName().empty())
      O << ' ' << T->getName();
    printFile(O, T->getFilename(), T->getDirectory(), T->getLine());
    if (auto *BT = dyn_cast<DIBasicType>(T)) {
      O << " ";
      if (const char *Encoding =
              dwarf::AttributeEncodingString(BT->getEncoding()))
        O << Encoding;
      else
        O << "unknown-encoding(" << BT->getEncoding() << ')';
    } else {
      O << ' ';
      if (const char *Tag = dwarf::TagString(T->getTag()))
        O << Tag;
      else
        O << "unknown-tag(" << T->getTag() << ")";
    }
    if (auto *CT = dyn_cast<DICompositeType>(T)) {
      if (auto *S = CT->getRawIdentifier())
        O << " (identifier: '" << S->getString() << "')";
    }
    O << '\n';
  }
}
