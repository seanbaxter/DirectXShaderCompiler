//===- llvm37/TableGen/Error.h - tblgen error handling helpers ----*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains error handling helper routines to pretty-print diagnostic
// messages from tblgen.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_TABLEGEN_ERROR_H
#define LLVM37_TABLEGEN_ERROR_H

#include "llvm37/Support/SourceMgr.h"

namespace llvm37 {

void PrintWarning(ArrayRef<SMLoc> WarningLoc, const Twine &Msg);
void PrintWarning(const char *Loc, const Twine &Msg);
void PrintWarning(const Twine &Msg);

void PrintError(ArrayRef<SMLoc> ErrorLoc, const Twine &Msg);
void PrintError(const char *Loc, const Twine &Msg);
void PrintError(const Twine &Msg);

LLVM37_ATTRIBUTE_NORETURN void PrintFatalError(const Twine &Msg);
LLVM37_ATTRIBUTE_NORETURN void PrintFatalError(ArrayRef<SMLoc> ErrorLoc,
                                             const Twine &Msg);

extern SourceMgr SrcMgr;
extern unsigned ErrorsPrinted;

} // end namespace "llvm37"

#endif
