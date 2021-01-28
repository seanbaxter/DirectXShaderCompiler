//===- llvm37/unittest/AsmParser/AsmParserTest.cpp - asm parser unittests ---===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/ADT/StringRef.h"
#include "llvm37/AsmParser/Parser.h"
#include "llvm37/AsmParser/SlotMapping.h"
#include "llvm37/IR/LLVMContext.h"
#include "llvm37/IR/Module.h"
#include "llvm37/Support/SourceMgr.h"
#include "gtest/gtest.h"

using namespace llvm37;

namespace {

TEST(AsmParserTest, NullTerminatedInput) {
  LLVM37Context &Ctx = getGlobalContext();
  StringRef Source = "; Empty module \n";
  SMDiagnostic Error;
  auto Mod = parseAssemblyString(Source, Error, Ctx);

  EXPECT_TRUE(Mod != nullptr);
  EXPECT_TRUE(Error.getMessage().empty());
}

#ifdef GTEST_HAS_DEATH_TEST
#ifndef NDEBUG

TEST(AsmParserTest, NonNullTerminatedInput) {
  LLVM37Context &Ctx = getGlobalContext();
  StringRef Source = "; Empty module \n\1\2";
  SMDiagnostic Error;
  std::unique_ptr<Module> Mod;
  EXPECT_DEATH(Mod = parseAssemblyString(Source.substr(0, Source.size() - 2),
                                         Error, Ctx),
               "Buffer is not null terminated!");
}

#endif
#endif

TEST(AsmParserTest, SlotMappingTest) {
  LLVM37Context &Ctx = getGlobalContext();
  StringRef Source = "@0 = global i32 0\n !0 = !{}\n !42 = !{i32 42}";
  SMDiagnostic Error;
  SlotMapping Mapping;
  auto Mod = parseAssemblyString(Source, Error, Ctx, &Mapping);

  EXPECT_TRUE(Mod != nullptr);
  EXPECT_TRUE(Error.getMessage().empty());

  ASSERT_EQ(Mapping.GlobalValues.size(), 1u);
  EXPECT_TRUE(isa<GlobalVariable>(Mapping.GlobalValues[0]));

  EXPECT_EQ(Mapping.MetadataNodes.size(), 2u);
  EXPECT_EQ(Mapping.MetadataNodes.count(0), 1u);
  EXPECT_EQ(Mapping.MetadataNodes.count(42), 1u);
  EXPECT_EQ(Mapping.MetadataNodes.count(1), 0u);
}

} // end anonymous namespace
