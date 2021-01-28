//===------------------- FaultMaps.h - The "FaultMaps" section --*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CODEGEN_FAULTMAPS_H
#define LLVM37_CODEGEN_FAULTMAPS_H

#include "llvm37/ADT/DenseMap.h"
#include "llvm37/MC/MCSymbol.h"
#include "llvm37/Support/Endian.h"
#include "llvm37/Support/Format.h"

#include <vector>
#include <map>

namespace llvm37 {

class AsmPrinter;
class MCExpr;
class MCSymbol;
class MCStreamer;

class FaultMaps {
public:
  enum FaultKind { FaultingLoad = 1, FaultKindMax };

  static const char *faultTypeToString(FaultKind);

  explicit FaultMaps(AsmPrinter &AP);

  void recordFaultingOp(FaultKind FaultTy, const MCSymbol *HandlerLabel);
  void serializeToFaultMapSection();

private:
  static const char *WFMP;

  struct FaultInfo {
    FaultKind Kind;
    const MCExpr *FaultingOffsetExpr;
    const MCExpr *HandlerOffsetExpr;

    FaultInfo()
        : Kind(FaultKindMax), FaultingOffsetExpr(nullptr),
          HandlerOffsetExpr(nullptr) {}

    explicit FaultInfo(FaultMaps::FaultKind Kind, const MCExpr *FaultingOffset,
                       const MCExpr *HandlerOffset)
        : Kind(Kind), FaultingOffsetExpr(FaultingOffset),
          HandlerOffsetExpr(HandlerOffset) {}
  };

  typedef std::vector<FaultInfo> FunctionFaultInfos;

  // We'd like to keep a stable iteration order for FunctionInfos to help
  // FileCheck based testing.
  struct MCSymbolComparator {
    bool operator()(const MCSymbol *LHS, const MCSymbol *RHS) const {
      return LHS->getName() < RHS->getName();
    }
  };

  std::map<const MCSymbol *, FunctionFaultInfos, MCSymbolComparator>
      FunctionInfos;
  AsmPrinter &AP;

  void emitFunctionInfo(const MCSymbol *FnLabel, const FunctionFaultInfos &FFI);
};

/// A parser for the __llvm37_faultmaps section generated by the FaultMaps class
/// above.  This parser is version locked with with the __llvm37_faultmaps section
/// generated by the version of LLVM37 that includes it.  No guarantees are made
/// with respect to forward or backward compatibility.
class FaultMapParser {
  typedef uint8_t FaultMapVersionType;
  static const size_t FaultMapVersionOffset = 0;

  typedef uint8_t Reserved0Type;
  static const size_t Reserved0Offset =
      FaultMapVersionOffset + sizeof(FaultMapVersionType);

  typedef uint16_t Reserved1Type;
  static const size_t Reserved1Offset = Reserved0Offset + sizeof(Reserved0Type);

  typedef uint32_t NumFunctionsType;
  static const size_t NumFunctionsOffset =
      Reserved1Offset + sizeof(Reserved1Type);

  static const size_t FunctionInfosOffset =
      NumFunctionsOffset + sizeof(NumFunctionsType);

  const uint8_t *P;
  const uint8_t *E;

  template <typename T> static T read(const uint8_t *P, const uint8_t *E) {
    assert(P + sizeof(T) <= E && "out of bounds read!");
    return support::endian::read<T, support::little, 1>(P);
  }

public:
  class FunctionFaultInfoAccessor {
    typedef uint32_t FaultKindType;
    static const size_t FaultKindOffset = 0;

    typedef uint32_t FaultingPCOffsetType;
    static const size_t FaultingPCOffsetOffset =
        FaultKindOffset + sizeof(FaultKindType);

    typedef uint32_t HandlerPCOffsetType;
    static const size_t HandlerPCOffsetOffset =
        FaultingPCOffsetOffset + sizeof(FaultingPCOffsetType);

    const uint8_t *P;
    const uint8_t *E;

  public:
    static const size_t Size =
        HandlerPCOffsetOffset + sizeof(HandlerPCOffsetType);

    explicit FunctionFaultInfoAccessor(const uint8_t *P, const uint8_t *E)
        : P(P), E(E) {}

    FaultKindType getFaultKind() const {
      return read<FaultKindType>(P + FaultKindOffset, E);
    }

    FaultingPCOffsetType getFaultingPCOffset() const {
      return read<FaultingPCOffsetType>(P + FaultingPCOffsetOffset, E);
    }

    HandlerPCOffsetType getHandlerPCOffset() const {
      return read<HandlerPCOffsetType>(P + HandlerPCOffsetOffset, E);
    }
  };

  class FunctionInfoAccessor {
    typedef uint64_t FunctionAddrType;
    static const size_t FunctionAddrOffset = 0;

    typedef uint32_t NumFaultingPCsType;
    static const size_t NumFaultingPCsOffset =
        FunctionAddrOffset + sizeof(FunctionAddrType);

    typedef uint32_t ReservedType;
    static const size_t ReservedOffset =
        NumFaultingPCsOffset + sizeof(NumFaultingPCsType);

    static const size_t FunctionFaultInfosOffset =
        ReservedOffset + sizeof(ReservedType);

    static const size_t FunctionInfoHeaderSize = FunctionFaultInfosOffset;

    const uint8_t *P;
    const uint8_t *E;

  public:
    FunctionInfoAccessor() : P(nullptr), E(nullptr) {}

    explicit FunctionInfoAccessor(const uint8_t *P, const uint8_t *E)
        : P(P), E(E) {}

    FunctionAddrType getFunctionAddr() const {
      return read<FunctionAddrType>(P + FunctionAddrOffset, E);
    }

    NumFaultingPCsType getNumFaultingPCs() const {
      return read<NumFaultingPCsType>(P + NumFaultingPCsOffset, E);
    }

    FunctionFaultInfoAccessor getFunctionFaultInfoAt(uint32_t Index) const {
      assert(Index < getNumFaultingPCs() && "index out of bounds!");
      const uint8_t *Begin = P + FunctionFaultInfosOffset +
                             FunctionFaultInfoAccessor::Size * Index;
      return FunctionFaultInfoAccessor(Begin, E);
    }

    FunctionInfoAccessor getNextFunctionInfo() const {
      size_t MySize = FunctionInfoHeaderSize +
                      getNumFaultingPCs() * FunctionFaultInfoAccessor::Size;

      const uint8_t *Begin = P + MySize;
      assert(Begin < E && "out of bounds!");
      return FunctionInfoAccessor(Begin, E);
    }
  };

  explicit FaultMapParser(const uint8_t *Begin, const uint8_t *End)
      : P(Begin), E(End) {}

  FaultMapVersionType getFaultMapVersion() const {
    auto Version = read<FaultMapVersionType>(P + FaultMapVersionOffset, E);
    assert(Version == 1 && "only version 1 supported!");
    return Version;
  }

  NumFunctionsType getNumFunctions() const {
    return read<NumFunctionsType>(P + NumFunctionsOffset, E);
  }

  FunctionInfoAccessor getFirstFunctionInfo() const {
    const uint8_t *Begin = P + FunctionInfosOffset;
    return FunctionInfoAccessor(Begin, E);
  }
};

raw_ostream &
operator<<(raw_ostream &OS, const FaultMapParser::FunctionFaultInfoAccessor &);

raw_ostream &operator<<(raw_ostream &OS,
                        const FaultMapParser::FunctionInfoAccessor &);

raw_ostream &operator<<(raw_ostream &OS, const FaultMapParser &);

} // namespace llvm37

#endif
