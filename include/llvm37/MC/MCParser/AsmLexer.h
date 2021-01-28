//===- AsmLexer.h - Lexer for Assembly Files --------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class declares the lexer for assembly files.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_MC_MCPARSER_ASMLEXER_H
#define LLVM37_MC_MCPARSER_ASMLEXER_H

#include "llvm37/ADT/StringRef.h"
#include "llvm37/MC/MCParser/MCAsmLexer.h"
#include "llvm37/Support/DataTypes.h"
#include <string>

namespace llvm37 {
class MemoryBuffer;
class MCAsmInfo;

/// AsmLexer - Lexer class for assembly files.
class AsmLexer : public MCAsmLexer {
  const MCAsmInfo &MAI;

  const char *CurPtr;
  StringRef CurBuf;
  bool isAtStartOfLine;

  void operator=(const AsmLexer&) = delete;
  AsmLexer(const AsmLexer&) = delete;

protected:
  /// LexToken - Read the next token and return its code.
  AsmToken LexToken() override;

public:
  AsmLexer(const MCAsmInfo &MAI);
  ~AsmLexer() override;

  void setBuffer(StringRef Buf, const char *ptr = nullptr);

  StringRef LexUntilEndOfStatement() override;
  StringRef LexUntilEndOfLine();

  const AsmToken peekTok(bool ShouldSkipSpace = true) override;

  bool isAtStartOfComment(const char *Ptr);
  bool isAtStatementSeparator(const char *Ptr);

  const MCAsmInfo &getMAI() const { return MAI; }

private:
  int getNextChar();
  AsmToken ReturnError(const char *Loc, const std::string &Msg);

  AsmToken LexIdentifier();
  AsmToken LexSlash();
  AsmToken LexLineComment();
  AsmToken LexDigit();
  AsmToken LexSingleQuote();
  AsmToken LexQuote();
  AsmToken LexFloatLiteral();
  AsmToken LexHexFloatLiteral(bool NoIntDigits);
};

} // end namespace llvm37

#endif
