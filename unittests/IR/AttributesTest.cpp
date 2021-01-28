//===- llvm37/unittest/IR/AttributesTest.cpp - Attributes unit tests --------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/IR/Attributes.h"
#include "llvm37/IR/LLVMContext.h"
#include "gtest/gtest.h"
using namespace llvm37;

namespace {

TEST(Attributes, Uniquing) {
  LLVM37Context C;

  Attribute AttrA = Attribute::get(C, Attribute::AlwaysInline);
  Attribute AttrB = Attribute::get(C, Attribute::AlwaysInline);
  EXPECT_EQ(AttrA, AttrB);

  AttributeSet ASs[] = {
    AttributeSet::get(C, 1, Attribute::ZExt),
    AttributeSet::get(C, 2, Attribute::SExt)
  };

  AttributeSet SetA = AttributeSet::get(C, ASs);
  AttributeSet SetB = AttributeSet::get(C, ASs);
  EXPECT_EQ(SetA, SetB);
}

TEST(Attributes, Ordering) {
  LLVM37Context C;

  AttributeSet ASs[] = {
    AttributeSet::get(C, 2, Attribute::ZExt),
    AttributeSet::get(C, 1, Attribute::SExt)
  };

  AttributeSet SetA = AttributeSet::get(C, ASs);
  AttributeSet SetB = SetA.removeAttributes(C, 1, ASs[1]);
  EXPECT_NE(SetA, SetB);
}

} // end anonymous namespace
