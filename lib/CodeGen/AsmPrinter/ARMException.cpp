//===-- CodeGen/AsmPrinter/ARMException.cpp - ARM EHABI Exception Impl ----===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains support for writing DWARF exception info into asm files.
//
//===----------------------------------------------------------------------===//

#include "DwarfException.h"
#include "llvm37/ADT/SmallString.h"
#include "llvm37/ADT/StringExtras.h"
#include "llvm37/ADT/Twine.h"
#include "llvm37/CodeGen/AsmPrinter.h"
#include "llvm37/CodeGen/MachineFrameInfo.h"
#include "llvm37/CodeGen/MachineFunction.h"
#include "llvm37/CodeGen/MachineModuleInfo.h"
#include "llvm37/IR/DataLayout.h"
#include "llvm37/IR/Mangler.h"
#include "llvm37/IR/Module.h"
#include "llvm37/MC/MCAsmInfo.h"
#include "llvm37/MC/MCContext.h"
#include "llvm37/MC/MCExpr.h"
#include "llvm37/MC/MCSection.h"
#include "llvm37/MC/MCStreamer.h"
#include "llvm37/MC/MCSymbol.h"
#include "llvm37/Support/CommandLine.h"
#include "llvm37/Support/Dwarf.h"
#include "llvm37/Support/FormattedStream.h"
#include "llvm37/Target/TargetFrameLowering.h"
#include "llvm37/Target/TargetOptions.h"
#include "llvm37/Target/TargetRegisterInfo.h"
using namespace llvm37;

ARMException::ARMException(AsmPrinter *A) : DwarfCFIExceptionBase(A) {}

ARMException::~ARMException() {}

ARMTargetStreamer &ARMException::getTargetStreamer() {
  MCTargetStreamer &TS = *Asm->OutStreamer->getTargetStreamer();
  return static_cast<ARMTargetStreamer &>(TS);
}

/// endModule - Emit all exception information that should come after the
/// content.
void ARMException::endModule() {
  if (shouldEmitCFI)
    Asm->OutStreamer->EmitCFISections(false, true);
}

void ARMException::beginFunction(const MachineFunction *MF) {
  if (Asm->MAI->getExceptionHandlingType() == ExceptionHandling::ARM)
    getTargetStreamer().emitFnStart();
  // See if we need call frame info.
  AsmPrinter::CFIMoveType MoveType = Asm->needsCFIMoves();
  assert(MoveType != AsmPrinter::CFI_M_EH &&
         "non-EH CFI not yet supported in prologue with EHABI lowering");
  if (MoveType == AsmPrinter::CFI_M_Debug) {
    shouldEmitCFI = true;
    Asm->OutStreamer->EmitCFIStartProc(false);
  }
}

/// endFunction - Gather and emit post-function exception information.
///
void ARMException::endFunction(const MachineFunction *MF) {
  ARMTargetStreamer &ATS = getTargetStreamer();
  const Function *F = MF->getFunction();
  const Function *Per = nullptr;
  if (F->hasPersonalityFn())
    Per = dyn_cast<Function>(F->getPersonalityFn()->stripPointerCasts());
  assert(!MMI->getPersonality() || Per == MMI->getPersonality());
  bool forceEmitPersonality =
    F->hasPersonalityFn() && !isNoOpWithoutInvoke(classifyEHPersonality(Per)) &&
    F->needsUnwindTableEntry();
  bool shouldEmitPersonality = forceEmitPersonality ||
    !MMI->getLandingPads().empty();
  if (!Asm->MF->getFunction()->needsUnwindTableEntry() &&
      !shouldEmitPersonality)
    ATS.emitCantUnwind();
  else if (shouldEmitPersonality) {
    // Emit references to personality.
    if (Per) {
      MCSymbol *PerSym = Asm->getSymbol(Per);
      Asm->OutStreamer->EmitSymbolAttribute(PerSym, MCSA_Global);
      ATS.emitPersonality(PerSym);
    }

    // Emit .handlerdata directive.
    ATS.emitHandlerData();

    // Emit actual exception table
    emitExceptionTable();
  }

  if (Asm->MAI->getExceptionHandlingType() == ExceptionHandling::ARM)
    ATS.emitFnEnd();
}

void ARMException::emitTypeInfos(unsigned TTypeEncoding) {
  const std::vector<const GlobalValue *> &TypeInfos = MMI->getTypeInfos();
  const std::vector<unsigned> &FilterIds = MMI->getFilterIds();

  bool VerboseAsm = Asm->OutStreamer->isVerboseAsm();

  int Entry = 0;
  // Emit the Catch TypeInfos.
  if (VerboseAsm && !TypeInfos.empty()) {
    Asm->OutStreamer->AddComment(">> Catch TypeInfos <<");
    Asm->OutStreamer->AddBlankLine();
    Entry = TypeInfos.size();
  }

  for (std::vector<const GlobalValue *>::const_reverse_iterator
         I = TypeInfos.rbegin(), E = TypeInfos.rend(); I != E; ++I) {
    const GlobalValue *GV = *I;
    if (VerboseAsm)
      Asm->OutStreamer->AddComment("TypeInfo " + Twine(Entry--));
    Asm->EmitTTypeReference(GV, TTypeEncoding);
  }

  // Emit the Exception Specifications.
  if (VerboseAsm && !FilterIds.empty()) {
    Asm->OutStreamer->AddComment(">> Filter TypeInfos <<");
    Asm->OutStreamer->AddBlankLine();
    Entry = 0;
  }
  for (std::vector<unsigned>::const_iterator
         I = FilterIds.begin(), E = FilterIds.end(); I < E; ++I) {
    unsigned TypeID = *I;
    if (VerboseAsm) {
      --Entry;
      if (TypeID != 0)
        Asm->OutStreamer->AddComment("FilterInfo " + Twine(Entry));
    }

    Asm->EmitTTypeReference((TypeID == 0 ? nullptr : TypeInfos[TypeID - 1]),
                            TTypeEncoding);
  }
}
