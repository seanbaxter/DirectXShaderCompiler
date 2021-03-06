//===- llvm37/unittest/IR/UseTest.cpp - Use unit tests ----------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/AsmParser/Parser.h"
#include "llvm37/IR/Function.h"
#include "llvm37/IR/Instructions.h"
#include "llvm37/IR/LLVMContext.h"
#include "llvm37/IR/Module.h"
#include "llvm37/IR/User.h"
#include "llvm37/Support/Format.h"
#include "llvm37/Support/SourceMgr.h"
#include "gtest/gtest.h"

using namespace llvm37;

namespace {

TEST(UseTest, sort) {
  LLVM37Context C;

  const char *ModuleString = "define void @f(i32 %x) {\n"
                             "entry:\n"
                             "  %v0 = add i32 %x, 0\n"
                             "  %v2 = add i32 %x, 2\n"
                             "  %v5 = add i32 %x, 5\n"
                             "  %v1 = add i32 %x, 1\n"
                             "  %v3 = add i32 %x, 3\n"
                             "  %v7 = add i32 %x, 7\n"
                             "  %v6 = add i32 %x, 6\n"
                             "  %v4 = add i32 %x, 4\n"
                             "  ret void\n"
                             "}\n";
  SMDiagnostic Err;
  char vnbuf[8];
  std::unique_ptr<Module> M = parseAssemblyString(ModuleString, Err, C);
  Function *F = M->getFunction("f");
  ASSERT_TRUE(F);
  ASSERT_TRUE(F->arg_begin() != F->arg_end());
  Argument &X = *F->arg_begin();
  ASSERT_EQ("x", X.getName());

  X.sortUseList([](const Use &L, const Use &R) {
    return L.getUser()->getName() < R.getUser()->getName();
  });
  unsigned I = 0;
  for (User *U : X.users()) {
    format("v%u", I++).snprint(vnbuf, sizeof(vnbuf));
    EXPECT_EQ(vnbuf, U->getName());
  }
  ASSERT_EQ(8u, I);

  X.sortUseList([](const Use &L, const Use &R) {
    return L.getUser()->getName() > R.getUser()->getName();
  });
  I = 0;
  for (User *U : X.users()) {
    format("v%u", (7 - I++)).snprint(vnbuf, sizeof(vnbuf));
    EXPECT_EQ(vnbuf, U->getName());
  }
  ASSERT_EQ(8u, I);
}

TEST(UseTest, reverse) {
  LLVM37Context C;

  const char *ModuleString = "define void @f(i32 %x) {\n"
                             "entry:\n"
                             "  %v0 = add i32 %x, 0\n"
                             "  %v2 = add i32 %x, 2\n"
                             "  %v5 = add i32 %x, 5\n"
                             "  %v1 = add i32 %x, 1\n"
                             "  %v3 = add i32 %x, 3\n"
                             "  %v7 = add i32 %x, 7\n"
                             "  %v6 = add i32 %x, 6\n"
                             "  %v4 = add i32 %x, 4\n"
                             "  ret void\n"
                             "}\n";
  SMDiagnostic Err;
  char vnbuf[8];
  std::unique_ptr<Module> M = parseAssemblyString(ModuleString, Err, C);
  Function *F = M->getFunction("f");
  ASSERT_TRUE(F);
  ASSERT_TRUE(F->arg_begin() != F->arg_end());
  Argument &X = *F->arg_begin();
  ASSERT_EQ("x", X.getName());

  X.sortUseList([](const Use &L, const Use &R) {
    return L.getUser()->getName() < R.getUser()->getName();
  });
  unsigned I = 0;
  for (User *U : X.users()) {
    format("v%u", I++).snprint(vnbuf, sizeof(vnbuf));
    EXPECT_EQ(vnbuf, U->getName());
  }
  ASSERT_EQ(8u, I);

  X.reverseUseList();
  I = 0;
  for (User *U : X.users()) {
    format("v%u", (7 - I++)).snprint(vnbuf, sizeof(vnbuf));
    EXPECT_EQ(vnbuf, U->getName());
  }
  ASSERT_EQ(8u, I);
}

} // end anonymous namespace
