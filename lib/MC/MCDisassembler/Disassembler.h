//===------------- Disassembler.h - LLVM37 Disassembler -----------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
// 
//===----------------------------------------------------------------------===//
//
// This file defines the interface for the Disassembly library's disassembler 
// context.  The disassembler is responsible for producing strings for
// individual instructions according to a given architecture and disassembly
// syntax.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_LIB_MC_MCDISASSEMBLER_DISASSEMBLER_H
#define LLVM37_LIB_MC_MCDISASSEMBLER_DISASSEMBLER_H

#include "llvm37-c/Disassembler.h"
#include "llvm37/ADT/SmallString.h"
#include "llvm37/Support/raw_ostream.h"
#include <string>

namespace llvm37 {
class MCContext;
class MCAsmInfo;
class MCDisassembler;
class MCInstPrinter; 
class MCInstrInfo;
class MCRegisterInfo;
class MCSubtargetInfo;
class Target;

//
// This is the disassembler context returned by LLVM37CreateDisasm().
//
class LLVM37DisasmContext {
private:
  //
  // The passed parameters when the disassembler context is created.
  //
  // The TripleName for this disassembler.
  std::string TripleName;
  // The pointer to the caller's block of symbolic information.
  void *DisInfo;
  // The Triple specific symbolic information type returned by GetOpInfo.
  int TagType;
  // The function to get the symbolic information for operands.
  LLVM37OpInfoCallback GetOpInfo;
  // The function to look up a symbol name.
  LLVM37SymbolLookupCallback SymbolLookUp;
  //
  // The objects created and saved by LLVM37CreateDisasm() then used by
  // LLVM37DisasmInstruction().
  //
  // The LLVM37 target corresponding to the disassembler.
  // FIXME: using std::unique_ptr<const llvm37::Target> causes a malloc error
  //        when this LLVM37DisasmContext is deleted.
  const Target *TheTarget;
  // The assembly information for the target architecture.
  std::unique_ptr<const llvm37::MCAsmInfo> MAI;
  // The register information for the target architecture.
  std::unique_ptr<const llvm37::MCRegisterInfo> MRI;
  // The subtarget information for the target architecture.
  std::unique_ptr<const llvm37::MCSubtargetInfo> MSI;
  // The instruction information for the target architecture.
  std::unique_ptr<const llvm37::MCInstrInfo> MII;
  // The assembly context for creating symbols and MCExprs.
  std::unique_ptr<const llvm37::MCContext> Ctx;
  // The disassembler for the target architecture.
  std::unique_ptr<const llvm37::MCDisassembler> DisAsm;
  // The instruction printer for the target architecture.
  std::unique_ptr<llvm37::MCInstPrinter> IP;
  // The options used to set up the disassembler.
  uint64_t Options;
  // The CPU string.
  std::string CPU;

public:
  // Comment stream and backing vector.
  SmallString<128> CommentsToEmit;
  raw_svector_ostream CommentStream;

  LLVM37DisasmContext(std::string tripleName, void *disInfo, int tagType,
                    LLVM37OpInfoCallback getOpInfo,
                    LLVM37SymbolLookupCallback symbolLookUp,
                    const Target *theTarget, const MCAsmInfo *mAI,
                    const MCRegisterInfo *mRI,
                    const MCSubtargetInfo *mSI,
                    const MCInstrInfo *mII,
                    llvm37::MCContext *ctx, const MCDisassembler *disAsm,
                    MCInstPrinter *iP) : TripleName(tripleName),
                    DisInfo(disInfo), TagType(tagType), GetOpInfo(getOpInfo),
                    SymbolLookUp(symbolLookUp), TheTarget(theTarget),
                    Options(0),
                    CommentStream(CommentsToEmit) {
    MAI.reset(mAI);
    MRI.reset(mRI);
    MSI.reset(mSI);
    MII.reset(mII);
    Ctx.reset(ctx);
    DisAsm.reset(disAsm);
    IP.reset(iP);
  }
  const std::string &getTripleName() const { return TripleName; }
  void *getDisInfo() const { return DisInfo; }
  int getTagType() const { return TagType; }
  LLVM37OpInfoCallback getGetOpInfo() const { return GetOpInfo; }
  LLVM37SymbolLookupCallback getSymbolLookupCallback() const {
    return SymbolLookUp;
  }
  const Target *getTarget() const { return TheTarget; }
  const MCDisassembler *getDisAsm() const { return DisAsm.get(); }
  const MCAsmInfo *getAsmInfo() const { return MAI.get(); }
  const MCInstrInfo *getInstrInfo() const { return MII.get(); }
  const MCRegisterInfo *getRegisterInfo() const { return MRI.get(); }
  const MCSubtargetInfo *getSubtargetInfo() const { return MSI.get(); }
  MCInstPrinter *getIP() { return IP.get(); }
  void setIP(MCInstPrinter *NewIP) { IP.reset(NewIP); }
  uint64_t getOptions() const { return Options; }
  void addOptions(uint64_t Options) { this->Options |= Options; }
  StringRef getCPU() const { return CPU; }
  void setCPU(const char *CPU) { this->CPU = CPU; }
};

} // namespace llvm37

#endif
