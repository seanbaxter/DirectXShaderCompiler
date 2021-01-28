//===-- llvm37/Support/Compression.h ---Compression----------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains basic functions for compression/uncompression.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_SUPPORT_COMPRESSION_H
#define LLVM37_SUPPORT_COMPRESSION_H

#include "llvm37/Support/DataTypes.h"

namespace llvm37 {
template <typename T> class SmallVectorImpl;
class StringRef;

namespace zlib {

enum CompressionLevel {
  NoCompression,
  DefaultCompression,
  BestSpeedCompression,
  BestSizeCompression
};

enum Status {
  StatusOK,
  StatusUnsupported,    // zlib is unavailable
  StatusOutOfMemory,    // there was not enough memory
  StatusBufferTooShort, // there was not enough room in the output buffer
  StatusInvalidArg,     // invalid input parameter
  StatusInvalidData     // data was corrupted or incomplete
};

bool isAvailable();

Status compress(StringRef InputBuffer, SmallVectorImpl<char> &CompressedBuffer,
                CompressionLevel Level = DefaultCompression);

Status uncompress(StringRef InputBuffer,
                  SmallVectorImpl<char> &UncompressedBuffer,
                  size_t UncompressedSize);

uint32_t crc32(StringRef Buffer);

}  // End of namespace zlib

} // End of namespace llvm37

#endif

