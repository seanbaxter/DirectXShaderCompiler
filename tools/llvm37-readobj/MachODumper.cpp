//===-- MachODump.cpp - Object file dumping utility for llvm37 --------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the MachO-specific dumper for llvm37-readobj.
//
//===----------------------------------------------------------------------===//

#include "llvm37-readobj.h"
#include "Error.h"
#include "ObjDumper.h"
#include "StackMapPrinter.h"
#include "StreamWriter.h"
#include "llvm37/ADT/SmallString.h"
#include "llvm37/ADT/StringExtras.h"
#include "llvm37/Object/MachO.h"
#include "llvm37/Support/Casting.h"

using namespace llvm37;
using namespace object;

namespace {

class MachODumper : public ObjDumper {
public:
  MachODumper(const MachOObjectFile *Obj, StreamWriter& Writer)
    : ObjDumper(Writer)
    , Obj(Obj) { }

  void printFileHeaders() override;
  void printSections() override;
  void printRelocations() override;
  void printSymbols() override;
  void printDynamicSymbols() override;
  void printUnwindInfo() override;
  void printStackMap() const override;

private:
  template<class MachHeader>
  void printFileHeaders(const MachHeader &Header);

  void printSymbol(const SymbolRef &Symbol);

  void printRelocation(const RelocationRef &Reloc);

  void printRelocation(const MachOObjectFile *Obj, const RelocationRef &Reloc);

  void printSections(const MachOObjectFile *Obj);

  const MachOObjectFile *Obj;
};

} // namespace


namespace llvm37 {

std::error_code createMachODumper(const object::ObjectFile *Obj,
                                  StreamWriter &Writer,
                                  std::unique_ptr<ObjDumper> &Result) {
  const MachOObjectFile *MachOObj = dyn_cast<MachOObjectFile>(Obj);
  if (!MachOObj)
    return readobj_error::unsupported_obj_file_format;

  Result.reset(new MachODumper(MachOObj, Writer));
  return readobj_error::success;
}

} // namespace llvm37

static const EnumEntry<uint32_t> MachOMagics[] = {
  { "Magic",      MachO::MH_MAGIC    },
  { "Cigam",      MachO::MH_CIGAM    },
  { "Magic64",    MachO::MH_MAGIC_64 },
  { "Cigam64",    MachO::MH_CIGAM_64 },
  { "FatMagic",   MachO::FAT_MAGIC   },
  { "FatCigam",   MachO::FAT_CIGAM   },
};

static const EnumEntry<uint32_t> MachOHeaderFileTypes[] = {
  { "Relocatable",          MachO::MH_OBJECT      },
  { "Executable",           MachO::MH_EXECUTE     },
  { "FixedVMLibrary",       MachO::MH_FVMLIB      },
  { "Core",                 MachO::MH_CORE        },
  { "PreloadedExecutable",  MachO::MH_PRELOAD     },
  { "DynamicLibrary",       MachO::MH_DYLIB       },
  { "DynamicLinker",        MachO::MH_DYLINKER    },
  { "Bundle",               MachO::MH_BUNDLE      },
  { "DynamicLibraryStub",   MachO::MH_DYLIB_STUB  },
  { "DWARFSymbol",          MachO::MH_DSYM        },
  { "KextBundle",           MachO::MH_KEXT_BUNDLE },
};

static const EnumEntry<uint32_t> MachOHeaderCpuTypes[] = {
  { "Any"       , static_cast<uint32_t>(MachO::CPU_TYPE_ANY) },
  { "X86"       , MachO::CPU_TYPE_X86       },
  { "X86-64"    , MachO::CPU_TYPE_X86_64    },
  { "Mc98000"   , MachO::CPU_TYPE_MC98000   },
  { "Arm"       , MachO::CPU_TYPE_ARM       },
  { "Arm64"     , MachO::CPU_TYPE_ARM64     },
  { "Sparc"     , MachO::CPU_TYPE_SPARC     },
  { "PowerPC"   , MachO::CPU_TYPE_POWERPC   },
  { "PowerPC64" , MachO::CPU_TYPE_POWERPC64 },
};

static const EnumEntry<uint32_t> MachOHeaderCpuSubtypesX86[] = {
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_I386_ALL),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_386),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_486),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_486SX),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_586),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_PENTPRO),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_PENTII_M3),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_PENTII_M5),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_CELERON),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_CELERON_MOBILE),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_PENTIUM_3),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_PENTIUM_3_M),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_PENTIUM_3_XEON),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_PENTIUM_M),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_PENTIUM_4),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_PENTIUM_4_M),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_ITANIUM),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_ITANIUM_2),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_XEON),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_XEON_MP),
};

static const EnumEntry<uint32_t> MachOHeaderCpuSubtypesX64[] = {
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_X86_64_ALL),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_X86_ARCH1),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_X86_64_H),
};

static const EnumEntry<uint32_t> MachOHeaderCpuSubtypesARM[] = {
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_ARM_ALL),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_ARM_V4T),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_ARM_V6),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_ARM_V5),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_ARM_V5TEJ),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_ARM_XSCALE),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_ARM_V7),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_ARM_V7S),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_ARM_V7K),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_ARM_V6M),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_ARM_V7M),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_ARM_V7EM),
};

static const EnumEntry<uint32_t> MachOHeaderCpuSubtypesARM64[] = {
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_ARM64_ALL),
};

static const EnumEntry<uint32_t> MachOHeaderCpuSubtypesSPARC[] = {
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_SPARC_ALL),
};

static const EnumEntry<uint32_t> MachOHeaderCpuSubtypesPPC[] = {
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_POWERPC_ALL),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_POWERPC_601),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_POWERPC_602),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_POWERPC_603),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_POWERPC_603e),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_POWERPC_603ev),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_POWERPC_604),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_POWERPC_604e),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_POWERPC_620),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_POWERPC_750),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_POWERPC_7400),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_POWERPC_7450),
  LLVM37_READOBJ_ENUM_ENT(MachO, CPU_SUBTYPE_POWERPC_970),
};

static const EnumEntry<uint32_t> MachOHeaderFlags[] = {
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_NOUNDEFS),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_INCRLINK),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_DYLDLINK),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_BINDATLOAD),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_PREBOUND),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_SPLIT_SEGS),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_LAZY_INIT),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_TWOLEVEL),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_FORCE_FLAT),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_NOMULTIDEFS),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_NOFIXPREBINDING),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_PREBINDABLE),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_ALLMODSBOUND),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_SUBSECTIONS_VIA_SYMBOLS),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_CANONICAL),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_WEAK_DEFINES),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_BINDS_TO_WEAK),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_ALLOW_STACK_EXECUTION),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_ROOT_SAFE),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_SETUID_SAFE),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_NO_REEXPORTED_DYLIBS),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_PIE),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_DEAD_STRIPPABLE_DYLIB),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_HAS_TLV_DESCRIPTORS),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_NO_HEAP_EXECUTION),
  LLVM37_READOBJ_ENUM_ENT(MachO, MH_APP_EXTENSION_SAFE),
};

static const EnumEntry<unsigned> MachOSectionAttributes[] = {
  { "LocReloc"         , 1 <<  0 /*S_ATTR_LOC_RELOC          */ },
  { "ExtReloc"         , 1 <<  1 /*S_ATTR_EXT_RELOC          */ },
  { "SomeInstructions" , 1 <<  2 /*S_ATTR_SOME_INSTRUCTIONS  */ },
  { "Debug"            , 1 << 17 /*S_ATTR_DEBUG              */ },
  { "SelfModifyingCode", 1 << 18 /*S_ATTR_SELF_MODIFYING_CODE*/ },
  { "LiveSupport"      , 1 << 19 /*S_ATTR_LIVE_SUPPORT       */ },
  { "NoDeadStrip"      , 1 << 20 /*S_ATTR_NO_DEAD_STRIP      */ },
  { "StripStaticSyms"  , 1 << 21 /*S_ATTR_STRIP_STATIC_SYMS  */ },
  { "NoTOC"            , 1 << 22 /*S_ATTR_NO_TOC             */ },
  { "PureInstructions" , 1 << 23 /*S_ATTR_PURE_INSTRUCTIONS  */ },
};

static const EnumEntry<unsigned> MachOSymbolRefTypes[] = {
  { "UndefinedNonLazy",                     0 },
  { "ReferenceFlagUndefinedLazy",           1 },
  { "ReferenceFlagDefined",                 2 },
  { "ReferenceFlagPrivateDefined",          3 },
  { "ReferenceFlagPrivateUndefinedNonLazy", 4 },
  { "ReferenceFlagPrivateUndefinedLazy",    5 }
};

static const EnumEntry<unsigned> MachOSymbolFlags[] = {
  { "ReferencedDynamically", 0x10 },
  { "NoDeadStrip",           0x20 },
  { "WeakRef",               0x40 },
  { "WeakDef",               0x80 }
};

static const EnumEntry<unsigned> MachOSymbolTypes[] = {
  { "Undef",           0x0 },
  { "Abs",             0x2 },
  { "Indirect",        0xA },
  { "PreboundUndef",   0xC },
  { "Section",         0xE }
};

namespace {
  struct MachOSection {
    ArrayRef<char> Name;
    ArrayRef<char> SegmentName;
    uint64_t Address;
    uint64_t Size;
    uint32_t Offset;
    uint32_t Alignment;
    uint32_t RelocationTableOffset;
    uint32_t NumRelocationTableEntries;
    uint32_t Flags;
    uint32_t Reserved1;
    uint32_t Reserved2;
  };

  struct MachOSymbol {
    uint32_t StringIndex;
    uint8_t Type;
    uint8_t SectionIndex;
    uint16_t Flags;
    uint64_t Value;
  };
}

static void getSection(const MachOObjectFile *Obj,
                       DataRefImpl Sec,
                       MachOSection &Section) {
  if (!Obj->is64Bit()) {
    MachO::section Sect = Obj->getSection(Sec);
    Section.Address     = Sect.addr;
    Section.Size        = Sect.size;
    Section.Offset      = Sect.offset;
    Section.Alignment   = Sect.align;
    Section.RelocationTableOffset = Sect.reloff;
    Section.NumRelocationTableEntries = Sect.nreloc;
    Section.Flags       = Sect.flags;
    Section.Reserved1   = Sect.reserved1;
    Section.Reserved2   = Sect.reserved2;
    return;
  }
  MachO::section_64 Sect = Obj->getSection64(Sec);
  Section.Address     = Sect.addr;
  Section.Size        = Sect.size;
  Section.Offset      = Sect.offset;
  Section.Alignment   = Sect.align;
  Section.RelocationTableOffset = Sect.reloff;
  Section.NumRelocationTableEntries = Sect.nreloc;
  Section.Flags       = Sect.flags;
  Section.Reserved1   = Sect.reserved1;
  Section.Reserved2   = Sect.reserved2;
}


static void getSymbol(const MachOObjectFile *Obj,
                      DataRefImpl DRI,
                      MachOSymbol &Symbol) {
  if (!Obj->is64Bit()) {
    MachO::nlist Entry = Obj->getSymbolTableEntry(DRI);
    Symbol.StringIndex  = Entry.n_strx;
    Symbol.Type         = Entry.n_type;
    Symbol.SectionIndex = Entry.n_sect;
    Symbol.Flags        = Entry.n_desc;
    Symbol.Value        = Entry.n_value;
    return;
  }
  MachO::nlist_64 Entry = Obj->getSymbol64TableEntry(DRI);
  Symbol.StringIndex  = Entry.n_strx;
  Symbol.Type         = Entry.n_type;
  Symbol.SectionIndex = Entry.n_sect;
  Symbol.Flags        = Entry.n_desc;
  Symbol.Value        = Entry.n_value;
}

void MachODumper::printFileHeaders() {
  DictScope H(W, "MachHeader");
  if (!Obj->is64Bit()) {
    printFileHeaders(Obj->getHeader());
  } else {
    printFileHeaders(Obj->getHeader64());
    W.printHex("Reserved", Obj->getHeader64().reserved);
  }
}

template<class MachHeader>
void MachODumper::printFileHeaders(const MachHeader &Header) {
  W.printEnum("Magic", Header.magic, makeArrayRef(MachOMagics));
  W.printEnum("CpuType", Header.cputype, makeArrayRef(MachOHeaderCpuTypes));
  uint32_t subtype = Header.cpusubtype & ~MachO::CPU_SUBTYPE_MASK;
  switch (Header.cputype) {
  case MachO::CPU_TYPE_X86:
    W.printEnum("CpuSubType", subtype, makeArrayRef(MachOHeaderCpuSubtypesX86));
    break;
  case MachO::CPU_TYPE_X86_64:
    W.printEnum("CpuSubType", subtype, makeArrayRef(MachOHeaderCpuSubtypesX64));
    break;
  case MachO::CPU_TYPE_ARM:
    W.printEnum("CpuSubType", subtype, makeArrayRef(MachOHeaderCpuSubtypesARM));
    break;
  case MachO::CPU_TYPE_POWERPC:
    W.printEnum("CpuSubType", subtype, makeArrayRef(MachOHeaderCpuSubtypesPPC));
    break;
  case MachO::CPU_TYPE_SPARC:
    W.printEnum("CpuSubType", subtype, makeArrayRef(MachOHeaderCpuSubtypesSPARC));
    break;
  case MachO::CPU_TYPE_ARM64:
    W.printEnum("CpuSubType", subtype, makeArrayRef(MachOHeaderCpuSubtypesARM64));
    break;
  case MachO::CPU_TYPE_POWERPC64:
  default:
    W.printHex("CpuSubtype", subtype);
  }
  W.printEnum("FileType", Header.filetype, makeArrayRef(MachOHeaderFileTypes));
  W.printNumber("NumOfLoadCommands", Header.ncmds);
  W.printNumber("SizeOfLoadCommands", Header.sizeofcmds);
  W.printFlags("Flags", Header.flags, makeArrayRef(MachOHeaderFlags));
}

void MachODumper::printSections() {
  return printSections(Obj);
}

void MachODumper::printSections(const MachOObjectFile *Obj) {
  ListScope Group(W, "Sections");

  int SectionIndex = -1;
  for (const SectionRef &Section : Obj->sections()) {
    ++SectionIndex;

    MachOSection MOSection;
    getSection(Obj, Section.getRawDataRefImpl(), MOSection);
    DataRefImpl DR = Section.getRawDataRefImpl();

    StringRef Name;
    if (error(Section.getName(Name)))
      Name = "";

    ArrayRef<char> RawName = Obj->getSectionRawName(DR);
    StringRef SegmentName = Obj->getSectionFinalSegmentName(DR);
    ArrayRef<char> RawSegmentName = Obj->getSectionRawFinalSegmentName(DR);

    DictScope SectionD(W, "Section");
    W.printNumber("Index", SectionIndex);
    W.printBinary("Name", Name, RawName);
    W.printBinary("Segment", SegmentName, RawSegmentName);
    W.printHex("Address", MOSection.Address);
    W.printHex("Size", MOSection.Size);
    W.printNumber("Offset", MOSection.Offset);
    W.printNumber("Alignment", MOSection.Alignment);
    W.printHex("RelocationOffset", MOSection.RelocationTableOffset);
    W.printNumber("RelocationCount", MOSection.NumRelocationTableEntries);
    W.printEnum("Type", MOSection.Flags & 0xFF,
                makeArrayRef(MachOSectionAttributes));
    W.printFlags("Attributes", MOSection.Flags >> 8,
                 makeArrayRef(MachOSectionAttributes));
    W.printHex("Reserved1", MOSection.Reserved1);
    W.printHex("Reserved2", MOSection.Reserved2);

    if (opts::SectionRelocations) {
      ListScope D(W, "Relocations");
      for (const RelocationRef &Reloc : Section.relocations())
        printRelocation(Reloc);
    }

    if (opts::SectionSymbols) {
      ListScope D(W, "Symbols");
      for (const SymbolRef &Symbol : Obj->symbols()) {
        if (!Section.containsSymbol(Symbol))
          continue;

        printSymbol(Symbol);
      }
    }

    if (opts::SectionData) {
      bool IsBSS = Section.isBSS();
      if (!IsBSS) {
        StringRef Data;
        if (error(Section.getContents(Data)))
          break;

        W.printBinaryBlock("SectionData", Data);
      }
    }
  }
}

void MachODumper::printRelocations() {
  ListScope D(W, "Relocations");

  std::error_code EC;
  for (const SectionRef &Section : Obj->sections()) {
    StringRef Name;
    if (error(Section.getName(Name)))
      continue;

    bool PrintedGroup = false;
    for (const RelocationRef &Reloc : Section.relocations()) {
      if (!PrintedGroup) {
        W.startLine() << "Section " << Name << " {\n";
        W.indent();
        PrintedGroup = true;
      }

      printRelocation(Reloc);
    }

    if (PrintedGroup) {
      W.unindent();
      W.startLine() << "}\n";
    }
  }
}

void MachODumper::printRelocation(const RelocationRef &Reloc) {
  return printRelocation(Obj, Reloc);
}

void MachODumper::printRelocation(const MachOObjectFile *Obj,
                                  const RelocationRef &Reloc) {
  uint64_t Offset = Reloc.getOffset();
  SmallString<32> RelocName;
  Reloc.getTypeName(RelocName);

  DataRefImpl DR = Reloc.getRawDataRefImpl();
  MachO::any_relocation_info RE = Obj->getRelocation(DR);
  bool IsScattered = Obj->isRelocationScattered(RE);
  bool IsExtern = !IsScattered && Obj->getPlainRelocationExternal(RE);

  StringRef TargetName;
  if (IsExtern) {
    symbol_iterator Symbol = Reloc.getSymbol();
    if (Symbol != Obj->symbol_end()) {
      ErrorOr<StringRef> TargetNameOrErr = Symbol->getName();
      if (error(TargetNameOrErr.getError()))
        return;
      TargetName = *TargetNameOrErr;
    }
  } else if (!IsScattered) {
    section_iterator SecI = Obj->getRelocationSection(DR);
    if (SecI != Obj->section_end()) {
      if (error(SecI->getName(TargetName)))
        return;
    }
  }
  if (TargetName.empty())
    TargetName = "-";

  if (opts::ExpandRelocs) {
    DictScope Group(W, "Relocation");
    W.printHex("Offset", Offset);
    W.printNumber("PCRel", Obj->getAnyRelocationPCRel(RE));
    W.printNumber("Length", Obj->getAnyRelocationLength(RE));
    W.printNumber("Type", RelocName, Obj->getAnyRelocationType(RE));
    if (IsScattered) {
      W.printHex("Value", Obj->getScatteredRelocationValue(RE));
    } else {
      const char *Kind = IsExtern ? "Symbol" : "Section";
      W.printNumber(Kind, TargetName, Obj->getPlainRelocationSymbolNum(RE));
    }
  } else {
    SmallString<32> SymbolNameOrOffset("0x");
    if (IsScattered) {
      // Scattered relocations don't really have an associated symbol for some
      // reason, even if one exists in the symtab at the correct address.
      SymbolNameOrOffset += utohexstr(Obj->getScatteredRelocationValue(RE));
    } else {
      SymbolNameOrOffset = TargetName;
    }

    raw_ostream& OS = W.startLine();
    OS << W.hex(Offset)
       << " " << Obj->getAnyRelocationPCRel(RE)
       << " " << Obj->getAnyRelocationLength(RE);
    if (IsScattered)
      OS << " n/a";
    else
      OS << " " << Obj->getPlainRelocationExternal(RE);
    OS << " " << RelocName
       << " " << IsScattered
       << " " << SymbolNameOrOffset
       << "\n";
  }
}

void MachODumper::printSymbols() {
  ListScope Group(W, "Symbols");

  for (const SymbolRef &Symbol : Obj->symbols()) {
    printSymbol(Symbol);
  }
}

void MachODumper::printDynamicSymbols() {
  ListScope Group(W, "DynamicSymbols");
}

void MachODumper::printSymbol(const SymbolRef &Symbol) {
  StringRef SymbolName;
  if (ErrorOr<StringRef> SymbolNameOrErr = Symbol.getName())
    SymbolName = *SymbolNameOrErr;

  MachOSymbol MOSymbol;
  getSymbol(Obj, Symbol.getRawDataRefImpl(), MOSymbol);

  StringRef SectionName = "";
  section_iterator SecI(Obj->section_begin());
  if (!error(Symbol.getSection(SecI)) && SecI != Obj->section_end())
    error(SecI->getName(SectionName));

  DictScope D(W, "Symbol");
  W.printNumber("Name", SymbolName, MOSymbol.StringIndex);
  if (MOSymbol.Type & MachO::N_STAB) {
    W.printHex("Type", "SymDebugTable", MOSymbol.Type);
  } else {
    if (MOSymbol.Type & MachO::N_PEXT)
      W.startLine() << "PrivateExtern\n";
    if (MOSymbol.Type & MachO::N_EXT)
      W.startLine() << "Extern\n";
    W.printEnum("Type", uint8_t(MOSymbol.Type & MachO::N_TYPE),
                makeArrayRef(MachOSymbolTypes));
  }
  W.printHex("Section", SectionName, MOSymbol.SectionIndex);
  W.printEnum("RefType", static_cast<uint16_t>(MOSymbol.Flags & 0xF),
              makeArrayRef(MachOSymbolRefTypes));
  W.printFlags("Flags", static_cast<uint16_t>(MOSymbol.Flags & ~0xF),
               makeArrayRef(MachOSymbolFlags));
  W.printHex("Value", MOSymbol.Value);
}

void MachODumper::printUnwindInfo() {
  W.startLine() << "UnwindInfo not implemented.\n";
}

void MachODumper::printStackMap() const {
  object::SectionRef StackMapSection;
  for (auto Sec : Obj->sections()) {
    StringRef Name;
    Sec.getName(Name);
    if (Name == "__llvm37_stackmaps") {
      StackMapSection = Sec;
      break;
    }
  }

  if (StackMapSection == object::SectionRef())
    return;

  StringRef StackMapContents;
  StackMapSection.getContents(StackMapContents);
  ArrayRef<uint8_t> StackMapContentsArray(
      reinterpret_cast<const uint8_t*>(StackMapContents.data()),
      StackMapContents.size());

  if (Obj->isLittleEndian())
     prettyPrintStackMap(
                      llvm37::outs(),
                      StackMapV1Parser<support::little>(StackMapContentsArray));
  else
     prettyPrintStackMap(llvm37::outs(),
                         StackMapV1Parser<support::big>(StackMapContentsArray));
}
