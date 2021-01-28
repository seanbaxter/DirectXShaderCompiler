//=-- CoverageMappingWriter.h - Code coverage mapping writer ------*- C++ -*-=//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains support for writing coverage mapping data for
// instrumentation based coverage.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_PROFILEDATA_COVERAGEMAPPINGWRITER_H
#define LLVM37_PROFILEDATA_COVERAGEMAPPINGWRITER_H

#include "llvm37/ADT/ArrayRef.h"
#include "llvm37/ADT/StringMap.h"
#include "llvm37/ProfileData/CoverageMapping.h"
#include "llvm37/Support/raw_ostream.h"

namespace llvm37 {
namespace coverage {

/// \brief Writer of the filenames section for the instrumentation
/// based code coverage.
class CoverageFilenamesSectionWriter {
  ArrayRef<StringRef> Filenames;

public:
  CoverageFilenamesSectionWriter(ArrayRef<StringRef> Filenames)
      : Filenames(Filenames) {}

  /// \brief Write encoded filenames to the given output stream.
  void write(raw_ostream &OS);
};

/// \brief Writer for instrumentation based coverage mapping data.
class CoverageMappingWriter {
  ArrayRef<unsigned> VirtualFileMapping;
  ArrayRef<CounterExpression> Expressions;
  MutableArrayRef<CounterMappingRegion> MappingRegions;

public:
  CoverageMappingWriter(ArrayRef<unsigned> VirtualFileMapping,
                        ArrayRef<CounterExpression> Expressions,
                        MutableArrayRef<CounterMappingRegion> MappingRegions)
      : VirtualFileMapping(VirtualFileMapping), Expressions(Expressions),
        MappingRegions(MappingRegions) {}

  CoverageMappingWriter(ArrayRef<CounterExpression> Expressions,
                        MutableArrayRef<CounterMappingRegion> MappingRegions)
      : Expressions(Expressions), MappingRegions(MappingRegions) {}

  /// \brief Write encoded coverage mapping data to the given output stream.
  void write(raw_ostream &OS);
};

} // end namespace coverage
} // end namespace llvm37

#endif
