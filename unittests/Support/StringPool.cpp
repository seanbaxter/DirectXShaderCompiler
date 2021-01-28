//===- llvm37/unittest/Support/StringPoiil.cpp - StringPool tests -----------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Support/StringPool.h"
#include "gtest/gtest.h"

using namespace llvm37;

namespace {

TEST(PooledStringPtrTest, OperatorEquals) {
  StringPool pool;
  const PooledStringPtr a = pool.intern("a");
  const PooledStringPtr b = pool.intern("b");
  EXPECT_FALSE(a == b);
}

TEST(PooledStringPtrTest, OperatorNotEquals) {
  StringPool pool;
  const PooledStringPtr a = pool.intern("a");
  const PooledStringPtr b = pool.intern("b");
  EXPECT_TRUE(a != b);
}

}
