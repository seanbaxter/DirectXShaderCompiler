//===------ llvm37/MC/MCAsmParserUtils.h - Asm Parser Utilities ---*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_MC_MCPARSER_MCASMPARSERUTILS_H
#define LLVM37_MC_MCPARSER_MCASMPARSERUTILS_H

namespace llvm37 {

class MCAsmParser;
class MCExpr;
class MCSymbol;
class StringRef;

namespace MCParserUtils {

/// Parse a value expression and return whether it can be assigned to a symbol
/// with the given name.
///
/// On success, returns false and sets the Symbol and Value output parameters.
bool parseAssignmentExpression(StringRef Name, bool allow_redef,
                               MCAsmParser &Parser, MCSymbol *&Symbol,
                               const MCExpr *&Value);

} // namespace MCParserUtils
} // namespace llvm37

#endif
