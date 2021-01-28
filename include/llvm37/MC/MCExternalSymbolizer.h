//===-- llvm37/MC/MCExternalSymbolizer.h - ------------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the MCExternalSymbolizer class, which
// enables library users to provide callbacks (through the C API) to do the
// symbolization externally.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_MC_MCEXTERNALSYMBOLIZER_H
#define LLVM37_MC_MCEXTERNALSYMBOLIZER_H

#include "llvm37-c/Disassembler.h"
#include "llvm37/MC/MCSymbolizer.h"
#include <memory>

namespace llvm37 {

/// \brief Symbolize using user-provided, C API, callbacks.
///
/// See llvm37-c/Disassembler.h.
class MCExternalSymbolizer : public MCSymbolizer {
protected:
  /// \name Hooks for symbolic disassembly via the public 'C' interface.
  /// @{
  /// The function to get the symbolic information for operands.
  LLVM37OpInfoCallback GetOpInfo;
  /// The function to lookup a symbol name.
  LLVM37SymbolLookupCallback SymbolLookUp;
  /// The pointer to the block of symbolic information for above call back.
  void *DisInfo;
  /// @}

public:
  MCExternalSymbolizer(MCContext &Ctx,
                       std::unique_ptr<MCRelocationInfo> RelInfo,
                       LLVM37OpInfoCallback getOpInfo,
                       LLVM37SymbolLookupCallback symbolLookUp, void *disInfo)
    : MCSymbolizer(Ctx, std::move(RelInfo)), GetOpInfo(getOpInfo),
      SymbolLookUp(symbolLookUp), DisInfo(disInfo) {}

  bool tryAddingSymbolicOperand(MCInst &MI, raw_ostream &CommentStream,
                                int64_t Value, uint64_t Address, bool IsBranch,
                                uint64_t Offset, uint64_t InstSize) override;
  void tryAddingPcLoadReferenceComment(raw_ostream &CommentStream,
                                       int64_t Value,
                                       uint64_t Address) override;
};

}

#endif
