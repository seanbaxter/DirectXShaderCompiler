//===-- llvm37/CodeGen/ByteStreamer.h - ByteStreamer class --------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a class that can take bytes that would normally be
// streamed via the AsmPrinter.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_LIB_CODEGEN_ASMPRINTER_BYTESTREAMER_H
#define LLVM37_LIB_CODEGEN_ASMPRINTER_BYTESTREAMER_H

#include "DIEHash.h"
#include "llvm37/ADT/ArrayRef.h"
#include "llvm37/CodeGen/AsmPrinter.h"
#include "llvm37/MC/MCStreamer.h"
#include "llvm37/Support/LEB128.h"
#include <string>

namespace llvm37 {
class ByteStreamer {
 public:
  virtual ~ByteStreamer() {}

  // For now we're just handling the calls we need for dwarf emission/hashing.
  virtual void EmitInt8(uint8_t Byte, const Twine &Comment = "") = 0;
  virtual void EmitSLEB128(uint64_t DWord, const Twine &Comment = "") = 0;
  virtual void EmitULEB128(uint64_t DWord, const Twine &Comment = "") = 0;
};

class APByteStreamer : public ByteStreamer {
private:
  AsmPrinter &AP;

public:
  APByteStreamer(AsmPrinter &Asm) : AP(Asm) {}
  void EmitInt8(uint8_t Byte, const Twine &Comment) override {
    AP.OutStreamer->AddComment(Comment);
    AP.EmitInt8(Byte);
  }
  void EmitSLEB128(uint64_t DWord, const Twine &Comment) override {
    AP.OutStreamer->AddComment(Comment);
    AP.EmitSLEB128(DWord);
  }
  void EmitULEB128(uint64_t DWord, const Twine &Comment) override {
    AP.OutStreamer->AddComment(Comment);
    AP.EmitULEB128(DWord);
  }
};

class HashingByteStreamer : public ByteStreamer {
 private:
  DIEHash &Hash;
 public:
 HashingByteStreamer(DIEHash &H) : Hash(H) {}
  void EmitInt8(uint8_t Byte, const Twine &Comment) override {
    Hash.update(Byte);
  }
  void EmitSLEB128(uint64_t DWord, const Twine &Comment) override {
    Hash.addSLEB128(DWord);
  }
  void EmitULEB128(uint64_t DWord, const Twine &Comment) override {
    Hash.addULEB128(DWord);
  }
};

class BufferByteStreamer : public ByteStreamer {
private:
  SmallVectorImpl<char> &Buffer;
  SmallVectorImpl<std::string> &Comments;

  /// \brief Only verbose textual output needs comments.  This will be set to
  /// true for that case, and false otherwise.  If false, comments passed in to
  /// the emit methods will be ignored.
  bool GenerateComments;

public:
  BufferByteStreamer(SmallVectorImpl<char> &Buffer,
                     SmallVectorImpl<std::string> &Comments,
                     bool GenerateComments)
  : Buffer(Buffer), Comments(Comments), GenerateComments(GenerateComments) {}
  void EmitInt8(uint8_t Byte, const Twine &Comment) override {
    Buffer.push_back(Byte);
    if (GenerateComments)
      Comments.push_back(Comment.str());
  }
  void EmitSLEB128(uint64_t DWord, const Twine &Comment) override {
    raw_svector_ostream OSE(Buffer);
    encodeSLEB128(DWord, OSE);
    if (GenerateComments)
      Comments.push_back(Comment.str());
  }
  void EmitULEB128(uint64_t DWord, const Twine &Comment) override {
    raw_svector_ostream OSE(Buffer);
    encodeULEB128(DWord, OSE);
    if (GenerateComments)
      Comments.push_back(Comment.str());
  }
};

}

#endif
