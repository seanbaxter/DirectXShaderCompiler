//===--- ObjectBuffer.h - Utility class to wrap object memory ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a wrapper class to hold the memory into which an
// object will be generated.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_EXECUTIONENGINE_OBJECTBUFFER_H
#define LLVM37_EXECUTIONENGINE_OBJECTBUFFER_H

#include "llvm37/ADT/SmallVector.h"
#include "llvm37/Support/MemoryBuffer.h"
#include "llvm37/Support/raw_ostream.h"

namespace llvm37 {

class ObjectMemoryBuffer : public MemoryBuffer {
public:
  template <unsigned N>
  ObjectMemoryBuffer(SmallVector<char, N> SV)
    : SV(SV), BufferName("<in-memory object>") {
    init(this->SV.begin(), this->SV.end(), false);
  }

  template <unsigned N>
  ObjectMemoryBuffer(SmallVector<char, N> SV, StringRef Name)
    : SV(SV), BufferName(Name) {
    init(this->SV.begin(), this->SV.end(), false);
  }
  const char* getBufferIdentifier() const override { return BufferName.c_str(); }

  BufferKind getBufferKind() const override { return MemoryBuffer_Malloc; }

private:
  SmallVector<char, 4096> SV;
  std::string BufferName;
};

} // namespace llvm37

#endif
