//===-- llvm37/MC/MCAsmParserExtension.h - Asm Parser Hooks -------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_MC_MCPARSER_MCASMPARSEREXTENSION_H
#define LLVM37_MC_MCPARSER_MCASMPARSEREXTENSION_H

#include "llvm37/ADT/StringRef.h"
#include "llvm37/MC/MCParser/MCAsmParser.h"
#include "llvm37/Support/SMLoc.h"

namespace llvm37 {
class Twine;

/// \brief Generic interface for extending the MCAsmParser,
/// which is implemented by target and object file assembly parser
/// implementations.
class MCAsmParserExtension {
  MCAsmParserExtension(const MCAsmParserExtension &) = delete;
  void operator=(const MCAsmParserExtension &) = delete;

  MCAsmParser *Parser;

protected:
  MCAsmParserExtension();

  // Helper template for implementing static dispatch functions.
  template<typename T, bool (T::*Handler)(StringRef, SMLoc)>
  static bool HandleDirective(MCAsmParserExtension *Target,
                              StringRef Directive,
                              SMLoc DirectiveLoc) {
    T *Obj = static_cast<T*>(Target);
    return (Obj->*Handler)(Directive, DirectiveLoc);
  }

  bool BracketExpressionsSupported;

public:
  virtual ~MCAsmParserExtension();

  /// \brief Initialize the extension for parsing using the given \p Parser.
  /// The extension should use the AsmParser interfaces to register its
  /// parsing routines.
  virtual void Initialize(MCAsmParser &Parser);

  /// \name MCAsmParser Proxy Interfaces
  /// @{

  MCContext &getContext() { return getParser().getContext(); }

  MCAsmLexer &getLexer() { return getParser().getLexer(); }
  const MCAsmLexer &getLexer() const {
    return const_cast<MCAsmParserExtension *>(this)->getLexer();
  }

  MCAsmParser &getParser() { return *Parser; }
  const MCAsmParser &getParser() const {
    return const_cast<MCAsmParserExtension*>(this)->getParser();
  }

  SourceMgr &getSourceManager() { return getParser().getSourceManager(); }
  MCStreamer &getStreamer() { return getParser().getStreamer(); }
  bool Warning(SMLoc L, const Twine &Msg) {
    return getParser().Warning(L, Msg);
  }
  bool Error(SMLoc L, const Twine &Msg) {
    return getParser().Error(L, Msg);
  }
  bool TokError(const Twine &Msg) {
    return getParser().TokError(Msg);
  }

  const AsmToken &Lex() { return getParser().Lex(); }

  const AsmToken &getTok() { return getParser().getTok(); }

  bool HasBracketExpressions() const { return BracketExpressionsSupported; }

  /// @}
};

} // End llvm37 namespace

#endif
