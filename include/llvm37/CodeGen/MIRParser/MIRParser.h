//===- MIRParser.h - MIR serialization format parser ----------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This MIR serialization library is currently a work in progress. It can't
// serialize machine functions at this time.
//
// This file declares the functions that parse the MIR serialization format
// files.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CODEGEN_MIRPARSER_MIRPARSER_H
#define LLVM37_CODEGEN_MIRPARSER_MIRPARSER_H

#include "llvm37/ADT/StringRef.h"
#include "llvm37/CodeGen/MachineFunctionInitializer.h"
#include "llvm37/IR/Module.h"
#include "llvm37/Support/MemoryBuffer.h"
#include <memory>

namespace llvm37 {

class MIRParserImpl;
class SMDiagnostic;

/// This class initializes machine functions by applying the state loaded from
/// a MIR file.
class MIRParser : public MachineFunctionInitializer {
  std::unique_ptr<MIRParserImpl> Impl;

public:
  MIRParser(std::unique_ptr<MIRParserImpl> Impl);
  MIRParser(const MIRParser &) = delete;
  ~MIRParser();

  /// Parse the optional LLVM37 IR module that's embedded in the MIR file.
  ///
  /// A new, empty module is created if the LLVM37 IR isn't present.
  /// Returns null if a parsing error occurred.
  std::unique_ptr<Module> parseLLVM37Module();

  /// Initialize the machine function to the state that's described in the MIR
  /// file.
  ///
  /// Return true if error occurred.
  bool initializeMachineFunction(MachineFunction &MF) override;
};

/// This function is the main interface to the MIR serialization format parser.
///
/// It reads in a MIR file and returns a MIR parser that can parse the embedded
/// LLVM37 IR module and initialize the machine functions by parsing the machine
/// function's state.
///
/// \param Filename - The name of the file to parse.
/// \param Error - Error result info.
/// \param Context - Context which will be used for the parsed LLVM37 IR module.
std::unique_ptr<MIRParser> createMIRParserFromFile(StringRef Filename,
                                                   SMDiagnostic &Error,
                                                   LLVM37Context &Context);

/// This function is another interface to the MIR serialization format parser.
///
/// It returns a MIR parser that works with the given memory buffer and that can
/// parse the embedded LLVM37 IR module and initialize the machine functions by
/// parsing the machine function's state.
///
/// \param Contents - The MemoryBuffer containing the machine level IR.
/// \param Context - Context which will be used for the parsed LLVM37 IR module.
std::unique_ptr<MIRParser>
createMIRParser(std::unique_ptr<MemoryBuffer> Contents, LLVM37Context &Context);

} // end namespace llvm37

#endif
