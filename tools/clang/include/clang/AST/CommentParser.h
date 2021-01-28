//===--- CommentParser.h - Doxygen comment parser ---------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the Doxygen comment parser.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_AST_COMMENTPARSER_H
#define LLVM37_CLANG_AST_COMMENTPARSER_H

#include "clang/AST/Comment.h"
#include "clang/AST/CommentLexer.h"
#include "clang/AST/CommentSema.h"
#include "clang/Basic/Diagnostic.h"
#include "llvm37/Support/Allocator.h"

namespace clang {
class SourceManager;

namespace comments {
class CommandTraits;

/// Doxygen comment parser.
class Parser {
  Parser(const Parser &) = delete;
  void operator=(const Parser &) = delete;

  friend class TextTokenRetokenizer;

  Lexer &L;

  Sema &S;

  /// Allocator for anything that goes into AST nodes.
  llvm37::BumpPtrAllocator &Allocator;

  /// Source manager for the comment being parsed.
  const SourceManager &SourceMgr;

  DiagnosticsEngine &Diags;

  DiagnosticBuilder Diag(SourceLocation Loc, unsigned DiagID) {
    return Diags.Report(Loc, DiagID);
  }

  const CommandTraits &Traits;

  /// Current lookahead token.  We can safely assume that all tokens are from
  /// a single source file.
  Token Tok;

  /// A stack of additional lookahead tokens.
  SmallVector<Token, 8> MoreLATokens;

  void consumeToken() {
    if (MoreLATokens.empty())
      L.lex(Tok);
    else
      Tok = MoreLATokens.pop_back_val();
  }

  void putBack(const Token &OldTok) {
    MoreLATokens.push_back(Tok);
    Tok = OldTok;
  }

  void putBack(ArrayRef<Token> Toks) {
    if (Toks.empty())
      return;

    MoreLATokens.push_back(Tok);
    MoreLATokens.append(Toks.rbegin(), std::prev(Toks.rend()));

    Tok = Toks[0];
  }

  bool isTokBlockCommand() {
    return (Tok.is(tok::backslash_command) || Tok.is(tok::at_command)) &&
           Traits.getCommandInfo(Tok.getCommandID())->IsBlockCommand;
  }

public:
  Parser(Lexer &L, Sema &S, llvm37::BumpPtrAllocator &Allocator,
         const SourceManager &SourceMgr, DiagnosticsEngine &Diags,
         const CommandTraits &Traits);

  /// Parse arguments for \\param command.
  void parseParamCommandArgs(ParamCommandComment *PC,
                             TextTokenRetokenizer &Retokenizer);

  /// Parse arguments for \\tparam command.
  void parseTParamCommandArgs(TParamCommandComment *TPC,
                              TextTokenRetokenizer &Retokenizer);

  void parseBlockCommandArgs(BlockCommandComment *BC,
                             TextTokenRetokenizer &Retokenizer,
                             unsigned NumArgs);

  BlockCommandComment *parseBlockCommand();
  InlineCommandComment *parseInlineCommand();

  HTMLStartTagComment *parseHTMLStartTag();
  HTMLEndTagComment *parseHTMLEndTag();

  BlockContentComment *parseParagraphOrBlockCommand();

  VerbatimBlockComment *parseVerbatimBlock();
  VerbatimLineComment *parseVerbatimLine();
  BlockContentComment *parseBlockContent();
  FullComment *parseFullComment();
};

} // end namespace comments
} // end namespace clang

#endif

