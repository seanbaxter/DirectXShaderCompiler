//===- llvm37/unittest/Object/Disassembler.cpp ------------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37-c/Disassembler.h"
#include "llvm37/Support/TargetSelect.h"
#include "gtest/gtest.h"

using namespace llvm37;

static const char *symbolLookupCallback(void *DisInfo, uint64_t ReferenceValue,
                                        uint64_t *ReferenceType,
                                        uint64_t ReferencePC,
                                        const char **ReferenceName) {
  *ReferenceType = LLVM37Disassembler_ReferenceType_InOut_None;
  return nullptr;
}

TEST(Disassembler, Test1) {
  llvm37::InitializeAllTargetInfos();
  llvm37::InitializeAllTargetMCs();
  llvm37::InitializeAllDisassemblers();

  uint8_t Bytes[] = {0x90, 0x90, 0xeb, 0xfd};
  uint8_t *BytesP = Bytes;
  const char OutStringSize = 100;
  char OutString[OutStringSize];
  LLVM37DisasmContextRef DCR = LLVM37CreateDisasm("x86_64-pc-linux", nullptr, 0,
                                              nullptr, symbolLookupCallback);
  if (!DCR)
    return;

  size_t InstSize;
  unsigned NumBytes = sizeof(Bytes);
  unsigned PC = 0;

  InstSize = LLVM37DisasmInstruction(DCR, BytesP, NumBytes, PC, OutString,
                                   OutStringSize);
  EXPECT_EQ(InstSize, 1U);
  EXPECT_EQ(StringRef(OutString), "\tnop");
  PC += InstSize;
  BytesP += InstSize;
  NumBytes -= InstSize;

  InstSize = LLVM37DisasmInstruction(DCR, BytesP, NumBytes, PC, OutString,
                                   OutStringSize);
  EXPECT_EQ(InstSize, 1U);
  EXPECT_EQ(StringRef(OutString), "\tnop");
  PC += InstSize;
  BytesP += InstSize;
  NumBytes -= InstSize;

  InstSize = LLVM37DisasmInstruction(DCR, BytesP, NumBytes, PC, OutString,
                                   OutStringSize);
  EXPECT_EQ(InstSize, 2U);
  EXPECT_EQ(StringRef(OutString), "\tjmp\t0x1");

  LLVM37DisasmDispose(DCR);
}
