//===- unittests/Support/TargetRegistry.cpp - -----------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Support/TargetRegistry.h"
#include "llvm37/Support/TargetSelect.h"
#include "gtest/gtest.h"

using namespace llvm37;

namespace {

TEST(TargetRegistry, TargetHasArchType) {
  // Presence of at least one target will be asserted when done with the loop,
  // else this would pass by accident if InitializeAllTargetInfos were omitted.
  int Count = 0;

  llvm37::InitializeAllTargetInfos();

  for (const Target &T : TargetRegistry::targets()) {
    StringRef Name = T.getName();
    // There is really no way (at present) to ask a Target whether it targets
    // a specific architecture, because the logic for that is buried in a
    // predicate.
    // We can't ask the predicate "Are you a function that always returns
    // false?"
    // So given that the cpp backend truly has no target arch, it is skipped.
    if (Name != "cpp") {
      Triple::ArchType Arch = Triple::getArchTypeForLLVM37Name(Name);
      EXPECT_NE(Arch, Triple::UnknownArch);
      ++Count;
    }
  }
  ASSERT_NE(Count, 0);
}

} // end namespace
