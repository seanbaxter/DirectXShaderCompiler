//===- FuzzerMain.cpp - main() function and flags -------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// main() and flags.
//===----------------------------------------------------------------------===//

#include "FuzzerInterface.h"
#include "FuzzerInternal.h"

// This function should be defined by the user.
extern "C" void LLVM37FuzzerTestOneInput(const uint8_t *Data, size_t Size);

int main(int argc, char **argv) {
  return fuzzer::FuzzerDriver(argc, argv, LLVM37FuzzerTestOneInput);
}
