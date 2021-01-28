//===- llvm37/unittest/IR/TypesTest.cpp - Type unit tests -------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/IR/DerivedTypes.h"
#include "llvm37/IR/LLVMContext.h"
#include "gtest/gtest.h"
using namespace llvm37;

namespace {

TEST(TypesTest, StructType) {
  LLVM37Context C;

  // PR13522
  StructType *Struct = StructType::create(C, "FooBar");
  EXPECT_EQ("FooBar", Struct->getName());
  Struct->setName(Struct->getName().substr(0, 3));
  EXPECT_EQ("Foo", Struct->getName());
  Struct->setName("");
  EXPECT_TRUE(Struct->getName().empty());
  EXPECT_FALSE(Struct->hasName());
}

}  // end anonymous namespace
