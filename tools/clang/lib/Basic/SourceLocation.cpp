//==--- SourceLocation.cpp - Compact identifier for Source Files -*- C++ -*-==//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines accessor methods for the FullSourceLoc class.
//
//===----------------------------------------------------------------------===//

#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/PrettyStackTrace.h"
#include "clang/Basic/SourceManager.h"
#include "llvm37/Support/MemoryBuffer.h"
#include "llvm37/Support/raw_ostream.h"
#include <cstdio>
using namespace clang;

//===----------------------------------------------------------------------===//
// PrettyStackTraceLoc
//===----------------------------------------------------------------------===//

void PrettyStackTraceLoc::print(raw_ostream &OS) const {
  if (Loc.isValid()) {
    Loc.print(OS, SM);
    OS << ": ";
  }
  OS << Message << '\n';
}

//===----------------------------------------------------------------------===//
// SourceLocation
//===----------------------------------------------------------------------===//

void SourceLocation::print(raw_ostream &OS, const SourceManager &SM)const{
  if (!isValid()) {
    OS << "<invalid loc>";
    return;
  }

  if (isFileID()) {
    PresumedLoc PLoc = SM.getPresumedLoc(*this);
    
    if (PLoc.isInvalid()) {
      OS << "<invalid>";
      return;
    }
    // The macro expansion and spelling pos is identical for file locs.
    OS << PLoc.getFilename() << ':' << PLoc.getLine()
       << ':' << PLoc.getColumn();
    return;
  }

  SM.getExpansionLoc(*this).print(OS, SM);

  OS << " <Spelling=";
  SM.getSpellingLoc(*this).print(OS, SM);
  OS << '>';
}

LLVM37_DUMP_METHOD std::string
SourceLocation::printToString(const SourceManager &SM) const {
  std::string S;
  llvm37::raw_string_ostream OS(S);
  print(OS, SM);
  return OS.str();
}

LLVM37_DUMP_METHOD void SourceLocation::dump(const SourceManager &SM) const {
  print(llvm37::errs(), SM);
}

//===----------------------------------------------------------------------===//
// FullSourceLoc
//===----------------------------------------------------------------------===//

FileID FullSourceLoc::getFileID() const {
  assert(isValid());
  return SrcMgr->getFileID(*this);
}


FullSourceLoc FullSourceLoc::getExpansionLoc() const {
  assert(isValid());
  return FullSourceLoc(SrcMgr->getExpansionLoc(*this), *SrcMgr);
}

FullSourceLoc FullSourceLoc::getSpellingLoc() const {
  assert(isValid());
  return FullSourceLoc(SrcMgr->getSpellingLoc(*this), *SrcMgr);
}

unsigned FullSourceLoc::getExpansionLineNumber(bool *Invalid) const {
  assert(isValid());
  return SrcMgr->getExpansionLineNumber(*this, Invalid);
}

unsigned FullSourceLoc::getExpansionColumnNumber(bool *Invalid) const {
  assert(isValid());
  return SrcMgr->getExpansionColumnNumber(*this, Invalid);
}

unsigned FullSourceLoc::getSpellingLineNumber(bool *Invalid) const {
  assert(isValid());
  return SrcMgr->getSpellingLineNumber(*this, Invalid);
}

unsigned FullSourceLoc::getSpellingColumnNumber(bool *Invalid) const {
  assert(isValid());
  return SrcMgr->getSpellingColumnNumber(*this, Invalid);
}

bool FullSourceLoc::isInSystemHeader() const {
  assert(isValid());
  return SrcMgr->isInSystemHeader(*this);
}

bool FullSourceLoc::isBeforeInTranslationUnitThan(SourceLocation Loc) const {
  assert(isValid());
  return SrcMgr->isBeforeInTranslationUnit(*this, Loc);
}

LLVM37_DUMP_METHOD void FullSourceLoc::dump() const {
  SourceLocation::dump(*SrcMgr);
}

const char *FullSourceLoc::getCharacterData(bool *Invalid) const {
  assert(isValid());
  return SrcMgr->getCharacterData(*this, Invalid);
}

StringRef FullSourceLoc::getBufferData(bool *Invalid) const {
  assert(isValid());
  return SrcMgr->getBuffer(SrcMgr->getFileID(*this), Invalid)->getBuffer();
}

std::pair<FileID, unsigned> FullSourceLoc::getDecomposedLoc() const {
  return SrcMgr->getDecomposedLoc(*this);
}
