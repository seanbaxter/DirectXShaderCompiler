//===--- SanitizerMetadata.cpp - Blacklist for sanitizers -----------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Class which emits metadata consumed by sanitizer instrumentation passes.
//
//===----------------------------------------------------------------------===//

#include "SanitizerMetadata.h"
#include "CodeGenModule.h"
#include "clang/AST/Type.h"
#include "llvm37/ADT/StringRef.h"
#include "llvm37/IR/Constants.h"
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
using namespace clang;
using namespace CodeGen;

SanitizerMetadata::SanitizerMetadata(CodeGenModule &CGM) : CGM(CGM) {}

void SanitizerMetadata::reportGlobalToASan(llvm37::GlobalVariable *GV,
                                           SourceLocation Loc, StringRef Name,
                                           QualType Ty, bool IsDynInit,
                                           bool IsBlacklisted) {
  if (!CGM.getLangOpts().Sanitize.hasOneOf(SanitizerKind::Address |
                                           SanitizerKind::KernelAddress))
    return;
  IsDynInit &= !CGM.isInSanitizerBlacklist(GV, Loc, Ty, "init");
  IsBlacklisted |= CGM.isInSanitizerBlacklist(GV, Loc, Ty);

  llvm37::Metadata *LocDescr = nullptr;
  llvm37::Metadata *GlobalName = nullptr;
  llvm37::LLVM37Context &VMContext = CGM.getLLVM37Context();
  if (!IsBlacklisted) {
    // Don't generate source location and global name if it is blacklisted -
    // it won't be instrumented anyway.
    LocDescr = getLocationMetadata(Loc);
    if (!Name.empty())
      GlobalName = llvm37::MDString::get(VMContext, Name);
  }

  llvm37::Metadata *GlobalMetadata[] = {
      llvm37::ConstantAsMetadata::get(GV), LocDescr, GlobalName,
      llvm37::ConstantAsMetadata::get(
          llvm37::ConstantInt::get(llvm37::Type::getInt1Ty(VMContext), IsDynInit)),
      llvm37::ConstantAsMetadata::get(llvm37::ConstantInt::get(
          llvm37::Type::getInt1Ty(VMContext), IsBlacklisted))};

  llvm37::MDNode *ThisGlobal = llvm37::MDNode::get(VMContext, GlobalMetadata);
  llvm37::NamedMDNode *AsanGlobals =
      CGM.getModule().getOrInsertNamedMetadata("llvm.asan.globals");
  AsanGlobals->addOperand(ThisGlobal);
}

void SanitizerMetadata::reportGlobalToASan(llvm37::GlobalVariable *GV,
                                           const VarDecl &D, bool IsDynInit) {
  if (!CGM.getLangOpts().Sanitize.hasOneOf(SanitizerKind::Address |
                                           SanitizerKind::KernelAddress))
    return;
  std::string QualName;
  llvm37::raw_string_ostream OS(QualName);
  D.printQualifiedName(OS);
  reportGlobalToASan(GV, D.getLocation(), OS.str(), D.getType(), IsDynInit);
}

void SanitizerMetadata::disableSanitizerForGlobal(llvm37::GlobalVariable *GV) {
  // For now, just make sure the global is not modified by the ASan
  // instrumentation.
  if (CGM.getLangOpts().Sanitize.hasOneOf(SanitizerKind::Address |
                                          SanitizerKind::KernelAddress))
    reportGlobalToASan(GV, SourceLocation(), "", QualType(), false, true);
}

void SanitizerMetadata::disableSanitizerForInstruction(llvm37::Instruction *I) {
  I->setMetadata(CGM.getModule().getMDKindID("nosanitize"),
                 llvm37::MDNode::get(CGM.getLLVM37Context(), None));
}

llvm37::MDNode *SanitizerMetadata::getLocationMetadata(SourceLocation Loc) {
  PresumedLoc PLoc = CGM.getContext().getSourceManager().getPresumedLoc(Loc);
  if (!PLoc.isValid())
    return nullptr;
  llvm37::LLVM37Context &VMContext = CGM.getLLVM37Context();
  llvm37::Metadata *LocMetadata[] = {
      llvm37::MDString::get(VMContext, PLoc.getFilename()),
      llvm37::ConstantAsMetadata::get(llvm37::ConstantInt::get(
          llvm37::Type::getInt32Ty(VMContext), PLoc.getLine())),
      llvm37::ConstantAsMetadata::get(llvm37::ConstantInt::get(
          llvm37::Type::getInt32Ty(VMContext), PLoc.getColumn())),
  };
  return llvm37::MDNode::get(VMContext, LocMetadata);
}
