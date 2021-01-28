//===- Object.cpp - C bindings to the object file library--------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the C bindings to the file-format-independent object
// library.
//
//===----------------------------------------------------------------------===//

#include "llvm37/ADT/SmallVector.h"
#include "llvm37-c/Object.h"
#include "llvm37/Object/ObjectFile.h"

using namespace llvm37;
using namespace object;

inline OwningBinary<ObjectFile> *unwrap(LLVM37ObjectFileRef OF) {
  return reinterpret_cast<OwningBinary<ObjectFile> *>(OF);
}

inline LLVM37ObjectFileRef wrap(const OwningBinary<ObjectFile> *OF) {
  return reinterpret_cast<LLVM37ObjectFileRef>(
      const_cast<OwningBinary<ObjectFile> *>(OF));
}

inline section_iterator *unwrap(LLVM37SectionIteratorRef SI) {
  return reinterpret_cast<section_iterator*>(SI);
}

inline LLVM37SectionIteratorRef
wrap(const section_iterator *SI) {
  return reinterpret_cast<LLVM37SectionIteratorRef>
    (const_cast<section_iterator*>(SI));
}

inline symbol_iterator *unwrap(LLVM37SymbolIteratorRef SI) {
  return reinterpret_cast<symbol_iterator*>(SI);
}

inline LLVM37SymbolIteratorRef
wrap(const symbol_iterator *SI) {
  return reinterpret_cast<LLVM37SymbolIteratorRef>
    (const_cast<symbol_iterator*>(SI));
}

inline relocation_iterator *unwrap(LLVM37RelocationIteratorRef SI) {
  return reinterpret_cast<relocation_iterator*>(SI);
}

inline LLVM37RelocationIteratorRef
wrap(const relocation_iterator *SI) {
  return reinterpret_cast<LLVM37RelocationIteratorRef>
    (const_cast<relocation_iterator*>(SI));
}

// ObjectFile creation
LLVM37ObjectFileRef LLVM37CreateObjectFile(LLVM37MemoryBufferRef MemBuf) {
  std::unique_ptr<MemoryBuffer> Buf(unwrap(MemBuf));
  ErrorOr<std::unique_ptr<ObjectFile>> ObjOrErr(
      ObjectFile::createObjectFile(Buf->getMemBufferRef()));
  std::unique_ptr<ObjectFile> Obj;
  if (!ObjOrErr)
    return nullptr;

  auto *Ret = new OwningBinary<ObjectFile>(std::move(ObjOrErr.get()), std::move(Buf));
  return wrap(Ret);
}

void LLVM37DisposeObjectFile(LLVM37ObjectFileRef ObjectFile) {
  delete unwrap(ObjectFile);
}

// ObjectFile Section iterators
LLVM37SectionIteratorRef LLVM37GetSections(LLVM37ObjectFileRef OF) {
  OwningBinary<ObjectFile> *OB = unwrap(OF);
  section_iterator SI = OB->getBinary()->section_begin();
  return wrap(new section_iterator(SI));
}

void LLVM37DisposeSectionIterator(LLVM37SectionIteratorRef SI) {
  delete unwrap(SI);
}

LLVM37Bool LLVM37IsSectionIteratorAtEnd(LLVM37ObjectFileRef OF,
                                    LLVM37SectionIteratorRef SI) {
  OwningBinary<ObjectFile> *OB = unwrap(OF);
  return (*unwrap(SI) == OB->getBinary()->section_end()) ? 1 : 0;
}

void LLVM37MoveToNextSection(LLVM37SectionIteratorRef SI) {
  ++(*unwrap(SI));
}

void LLVM37MoveToContainingSection(LLVM37SectionIteratorRef Sect,
                                 LLVM37SymbolIteratorRef Sym) {
  if (std::error_code ec = (*unwrap(Sym))->getSection(*unwrap(Sect)))
    report_fatal_error(ec.message());
}

// ObjectFile Symbol iterators
LLVM37SymbolIteratorRef LLVM37GetSymbols(LLVM37ObjectFileRef OF) {
  OwningBinary<ObjectFile> *OB = unwrap(OF);
  symbol_iterator SI = OB->getBinary()->symbol_begin();
  return wrap(new symbol_iterator(SI));
}

void LLVM37DisposeSymbolIterator(LLVM37SymbolIteratorRef SI) {
  delete unwrap(SI);
}

LLVM37Bool LLVM37IsSymbolIteratorAtEnd(LLVM37ObjectFileRef OF,
                                   LLVM37SymbolIteratorRef SI) {
  OwningBinary<ObjectFile> *OB = unwrap(OF);
  return (*unwrap(SI) == OB->getBinary()->symbol_end()) ? 1 : 0;
}

void LLVM37MoveToNextSymbol(LLVM37SymbolIteratorRef SI) {
  ++(*unwrap(SI));
}

// SectionRef accessors
const char *LLVM37GetSectionName(LLVM37SectionIteratorRef SI) {
  StringRef ret;
  if (std::error_code ec = (*unwrap(SI))->getName(ret))
   report_fatal_error(ec.message());
  return ret.data();
}

uint64_t LLVM37GetSectionSize(LLVM37SectionIteratorRef SI) {
  return (*unwrap(SI))->getSize();
}

const char *LLVM37GetSectionContents(LLVM37SectionIteratorRef SI) {
  StringRef ret;
  if (std::error_code ec = (*unwrap(SI))->getContents(ret))
    report_fatal_error(ec.message());
  return ret.data();
}

uint64_t LLVM37GetSectionAddress(LLVM37SectionIteratorRef SI) {
  return (*unwrap(SI))->getAddress();
}

LLVM37Bool LLVM37GetSectionContainsSymbol(LLVM37SectionIteratorRef SI,
                                 LLVM37SymbolIteratorRef Sym) {
  return (*unwrap(SI))->containsSymbol(**unwrap(Sym));
}

// Section Relocation iterators
LLVM37RelocationIteratorRef LLVM37GetRelocations(LLVM37SectionIteratorRef Section) {
  relocation_iterator SI = (*unwrap(Section))->relocation_begin();
  return wrap(new relocation_iterator(SI));
}

void LLVM37DisposeRelocationIterator(LLVM37RelocationIteratorRef SI) {
  delete unwrap(SI);
}

LLVM37Bool LLVM37IsRelocationIteratorAtEnd(LLVM37SectionIteratorRef Section,
                                       LLVM37RelocationIteratorRef SI) {
  return (*unwrap(SI) == (*unwrap(Section))->relocation_end()) ? 1 : 0;
}

void LLVM37MoveToNextRelocation(LLVM37RelocationIteratorRef SI) {
  ++(*unwrap(SI));
}


// SymbolRef accessors
const char *LLVM37GetSymbolName(LLVM37SymbolIteratorRef SI) {
  ErrorOr<StringRef> Ret = (*unwrap(SI))->getName();
  if (std::error_code EC = Ret.getError())
    report_fatal_error(EC.message());
  return Ret->data();
}

uint64_t LLVM37GetSymbolAddress(LLVM37SymbolIteratorRef SI) {
  ErrorOr<uint64_t> Ret = (*unwrap(SI))->getAddress();
  if (std::error_code EC = Ret.getError())
    report_fatal_error(EC.message());
  return *Ret;
}

uint64_t LLVM37GetSymbolSize(LLVM37SymbolIteratorRef SI) {
  return (*unwrap(SI))->getCommonSize();
}

// RelocationRef accessors
uint64_t LLVM37GetRelocationOffset(LLVM37RelocationIteratorRef RI) {
  return (*unwrap(RI))->getOffset();
}

LLVM37SymbolIteratorRef LLVM37GetRelocationSymbol(LLVM37RelocationIteratorRef RI) {
  symbol_iterator ret = (*unwrap(RI))->getSymbol();
  return wrap(new symbol_iterator(ret));
}

uint64_t LLVM37GetRelocationType(LLVM37RelocationIteratorRef RI) {
  return (*unwrap(RI))->getType();
}

// NOTE: Caller takes ownership of returned string.
const char *LLVM37GetRelocationTypeName(LLVM37RelocationIteratorRef RI) {
  SmallVector<char, 0> ret;
  (*unwrap(RI))->getTypeName(ret);
  char *str = static_cast<char*>(malloc(ret.size()));
  std::copy(ret.begin(), ret.end(), str);
  return str;
}

// NOTE: Caller takes ownership of returned string.
const char *LLVM37GetRelocationValueString(LLVM37RelocationIteratorRef RI) {
  return strdup("");
}

