//===--- Frontend/PCHContainerOperations.h - PCH Containers -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_PCH_CONTAINER_OPERATIONS_H
#define LLVM37_CLANG_PCH_CONTAINER_OPERATIONS_H

#include "llvm37/ADT/SmallVector.h"
#include "llvm37/ADT/StringMap.h"
#include "llvm37/Support/MemoryBuffer.h"
#include <memory>

namespace llvm37 {
class raw_pwrite_stream;
class BitstreamReader;
}

using llvm37::StringRef;

namespace clang {

class ASTConsumer;
class CodeGenOptions;
class DiagnosticsEngine;
class HeaderSearchOptions;
class LangOptions;
class PreprocessorOptions;
class TargetOptions;

struct PCHBuffer {
  bool IsComplete;
  llvm37::SmallVector<char, 0> Data;
};
  
/// This abstract interface provides operations for creating
/// containers for serialized ASTs (precompiled headers and clang
/// modules).
class PCHContainerWriter {
public: 
  virtual ~PCHContainerWriter() = 0;
  virtual StringRef getFormat() const = 0;

  /// Return an ASTConsumer that can be chained with a
  /// PCHGenerator that produces a wrapper file format containing a
  /// serialized AST bitstream.
  virtual std::unique_ptr<ASTConsumer> CreatePCHContainerGenerator(
      DiagnosticsEngine &Diags, const HeaderSearchOptions &HSO,
      const PreprocessorOptions &PPO, const TargetOptions &TO,
      const LangOptions &LO, const std::string &MainFileName,
      const std::string &OutputFileName, llvm37::raw_pwrite_stream *OS,
      std::shared_ptr<PCHBuffer> Buffer) const = 0;
};

/// This abstract interface provides operations for unwrapping
/// containers for serialized ASTs (precompiled headers and clang
/// modules).
class PCHContainerReader {
public: 
  virtual ~PCHContainerReader() = 0;
  /// Equivalent to the format passed to -fmodule-format=
  virtual StringRef getFormat() const = 0;

  /// Initialize an llvm37::BitstreamReader with the serialized AST inside
  /// the PCH container Buffer.
  virtual void ExtractPCH(llvm37::MemoryBufferRef Buffer,
                          llvm37::BitstreamReader &StreamFile) const = 0;
};

/// Implements write operations for a raw pass-through PCH container.
class RawPCHContainerWriter : public PCHContainerWriter {
  StringRef getFormat() const override { return "raw"; }

  /// Return an ASTConsumer that can be chained with a
  /// PCHGenerator that writes the module to a flat file.
  std::unique_ptr<ASTConsumer> CreatePCHContainerGenerator(
      DiagnosticsEngine &Diags, const HeaderSearchOptions &HSO,
      const PreprocessorOptions &PPO, const TargetOptions &TO,
      const LangOptions &LO, const std::string &MainFileName,
      const std::string &OutputFileName, llvm37::raw_pwrite_stream *OS,
      std::shared_ptr<PCHBuffer> Buffer) const override;
};

/// Implements read operations for a raw pass-through PCH container.
class RawPCHContainerReader : public PCHContainerReader {
  StringRef getFormat() const override { return "raw"; }

  /// Initialize an llvm37::BitstreamReader with Buffer.
  void ExtractPCH(llvm37::MemoryBufferRef Buffer,
                  llvm37::BitstreamReader &StreamFile) const override;
};

/// A registry of PCHContainerWriter and -Reader objects for different formats.
class PCHContainerOperations {
  llvm37::StringMap<std::unique_ptr<PCHContainerWriter>> Writers;
  llvm37::StringMap<std::unique_ptr<PCHContainerReader>> Readers;
public:
  /// Automatically registers a RawPCHContainerWriter and
  /// RawPCHContainerReader.
  PCHContainerOperations();
  void registerWriter(std::unique_ptr<PCHContainerWriter> Writer) {
    Writers[Writer->getFormat()] = std::move(Writer);
  }
  void registerReader(std::unique_ptr<PCHContainerReader> Reader) {
    Readers[Reader->getFormat()] = std::move(Reader);
  }  
  const PCHContainerWriter *getWriterOrNull(StringRef Format) {
    return Writers[Format].get();
  }
  const PCHContainerReader *getReaderOrNull(StringRef Format) {
    return Readers[Format].get();
  }
  const PCHContainerReader &getRawReader() {
    return *getReaderOrNull("raw");
  }
};

}

#endif
