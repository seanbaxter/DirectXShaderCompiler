//===- MCWinCOFFStreamer.h - COFF Object File Interface ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_MC_MCWINCOFFSTREAMER_H
#define LLVM37_MC_MCWINCOFFSTREAMER_H

#include "llvm37/MC/MCDirectives.h"
#include "llvm37/MC/MCObjectStreamer.h"

namespace llvm37 {
class MCAsmBackend;
class MCContext;
class MCCodeEmitter;
class MCExpr;
class MCInst;
class MCSection;
class MCSubtargetInfo;
class MCSymbol;
class StringRef;
class raw_ostream;
class raw_pwrite_stream;

class MCWinCOFFStreamer : public MCObjectStreamer {
public:
  MCWinCOFFStreamer(MCContext &Context, MCAsmBackend &MAB, MCCodeEmitter &CE,
                    raw_pwrite_stream &OS);

  /// state management
  void reset() override {
    CurSymbol = nullptr;
    MCObjectStreamer::reset();
  }

  /// \name MCStreamer interface
  /// \{

  void InitSections(bool NoExecStack) override;
  void EmitLabel(MCSymbol *Symbol) override;
  void EmitAssemblerFlag(MCAssemblerFlag Flag) override;
  void EmitThumbFunc(MCSymbol *Func) override;
  bool EmitSymbolAttribute(MCSymbol *Symbol, MCSymbolAttr Attribute) override;
  void EmitSymbolDesc(MCSymbol *Symbol, unsigned DescValue) override;
  void BeginCOFFSymbolDef(MCSymbol const *Symbol) override;
  void EmitCOFFSymbolStorageClass(int StorageClass) override;
  void EmitCOFFSymbolType(int Type) override;
  void EndCOFFSymbolDef() override;
  void EmitCOFFSafeSEH(MCSymbol const *Symbol) override;
  void EmitCOFFSectionIndex(MCSymbol const *Symbol) override;
  void EmitCOFFSecRel32(MCSymbol const *Symbol) override;
  void EmitCommonSymbol(MCSymbol *Symbol, uint64_t Size,
                        unsigned ByteAlignment) override;
  void EmitLocalCommonSymbol(MCSymbol *Symbol, uint64_t Size,
                             unsigned ByteAlignment) override;
  void EmitZerofill(MCSection *Section, MCSymbol *Symbol, uint64_t Size,
                    unsigned ByteAlignment) override;
  void EmitTBSSSymbol(MCSection *Section, MCSymbol *Symbol, uint64_t Size,
                      unsigned ByteAlignment) override;
  void EmitFileDirective(StringRef Filename) override;
  void EmitIdent(StringRef IdentString) override;
  void EmitWinEHHandlerData() override;
  void FinishImpl() override;

  /// \}

protected:
  const MCSymbol *CurSymbol;
  void EmitInstToData(const MCInst &Inst, const MCSubtargetInfo &STI) override;

private:
  LLVM37_ATTRIBUTE_NORETURN void FatalError(const Twine &Msg) const;
};
}

#endif

