//===- ValueMapper.cpp - Unit tests for ValueMapper -----------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/IR/LLVMContext.h"
#include "llvm37/IR/Metadata.h"
#include "llvm37/Transforms/Utils/ValueMapper.h"
#include "gtest/gtest.h"

using namespace llvm37;

namespace {

TEST(ValueMapperTest, MapMetadataUnresolved) {
  LLVM37Context Context;
  TempMDTuple T = MDTuple::getTemporary(Context, None);

  ValueToValueMapTy VM;
  EXPECT_EQ(T.get(), MapMetadata(T.get(), VM, RF_NoModuleLevelChanges));
}

}
