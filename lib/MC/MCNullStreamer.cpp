//===- lib/MC/MCNullStreamer.cpp - Dummy Streamer Implementation ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/MC/MCStreamer.h"
#include "llvm37/MC/MCContext.h"
#include "llvm37/MC/MCInst.h"
#include "llvm37/MC/MCSectionMachO.h"
#include "llvm37/MC/MCSymbol.h"

using namespace llvm37;

namespace {

  class MCNullStreamer : public MCStreamer {
  public:
    MCNullStreamer(MCContext &Context) : MCStreamer(Context) {}

    /// @name MCStreamer Interface
    /// @{

    bool EmitSymbolAttribute(MCSymbol *Symbol,
                             MCSymbolAttr Attribute) override {
      return true;
    }

    void EmitCommonSymbol(MCSymbol *Symbol, uint64_t Size,
                          unsigned ByteAlignment) override {}
    void EmitZerofill(MCSection *Section, MCSymbol *Symbol = nullptr,
                      uint64_t Size = 0, unsigned ByteAlignment = 0) override {}
    void EmitGPRel32Value(const MCExpr *Value) override {}
  };

}

MCStreamer *llvm37::createNullStreamer(MCContext &Context) {
  return new MCNullStreamer(Context);
}
