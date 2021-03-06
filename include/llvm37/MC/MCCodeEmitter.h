//===-- llvm37/MC/MCCodeEmitter.h - Instruction Encoding ----------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_MC_MCCODEEMITTER_H
#define LLVM37_MC_MCCODEEMITTER_H

#include "llvm37/Support/Compiler.h"

namespace llvm37 {
class MCFixup;
class MCInst;
class MCSubtargetInfo;
class raw_ostream;
template<typename T> class SmallVectorImpl;

/// MCCodeEmitter - Generic instruction encoding interface.
class MCCodeEmitter {
private:
  MCCodeEmitter(const MCCodeEmitter &) = delete;
  void operator=(const MCCodeEmitter &) = delete;

protected: // Can only create subclasses.
  MCCodeEmitter();

public:
  virtual ~MCCodeEmitter();

  /// Lifetime management
  virtual void reset() {}

  /// EncodeInstruction - Encode the given \p Inst to bytes on the output
  /// stream \p OS.
  virtual void encodeInstruction(const MCInst &Inst, raw_ostream &OS,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const = 0;
};

} // End llvm37 namespace

#endif
