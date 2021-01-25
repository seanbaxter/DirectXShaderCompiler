//===-- DWARFDebugFrame.h - Parsing of .debug_frame -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_LIB_DEBUGINFO_DWARFDEBUGFRAME_H
#define LLVM37_LIB_DEBUGINFO_DWARFDEBUGFRAME_H

#include "llvm37/Support/DataExtractor.h"
#include "llvm37/Support/raw_ostream.h"
#include <memory>
#include <vector>

namespace llvm37 {

class FrameEntry;

/// \brief A parsed .debug_frame section
///
class DWARFDebugFrame {
public:
  DWARFDebugFrame();
  ~DWARFDebugFrame();

  /// \brief Dump the section data into the given stream.
  void dump(raw_ostream &OS) const;

  /// \brief Parse the section from raw data.
  /// data is assumed to be pointing to the beginning of the section.
  void parse(DataExtractor Data);

private:
  std::vector<std::unique_ptr<FrameEntry>> Entries;
};


} // namespace llvm37

#endif
