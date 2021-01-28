//===-- llvm37/ADT/APSInt.cpp - Arbitrary Precision Signed Int ---*- C++ -*--===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the APSInt class, which is a simple class that
// represents an arbitrary sized integer that knows its signedness.
//
//===----------------------------------------------------------------------===//

#include "llvm37/ADT/APSInt.h"
#include "llvm37/ADT/FoldingSet.h"

using namespace llvm37;

APSInt::APSInt(StringRef Str) {
  assert(!Str.empty() && "Invalid string length");

  // (Over-)estimate the required number of bits.
  unsigned NumBits = ((Str.size() * 64) / 19) + 2;
  APInt Tmp(NumBits, Str, /*Radix=*/10);
  if (Str[0] == '-') {
    unsigned MinBits = Tmp.getMinSignedBits();
    if (MinBits > 0 && MinBits < NumBits)
      Tmp = Tmp.trunc(MinBits);
    *this = APSInt(Tmp, /*IsUnsigned=*/false);
    return;
  }
  unsigned ActiveBits = Tmp.getActiveBits();
  if (ActiveBits > 0 && ActiveBits < NumBits)
    Tmp = Tmp.trunc(ActiveBits);
  *this = APSInt(Tmp, /*IsUnsigned=*/true);
}

void APSInt::Profile(FoldingSetNodeID& ID) const {
  ID.AddInteger((unsigned) (IsUnsigned ? 1 : 0));
  APInt::Profile(ID);
}
