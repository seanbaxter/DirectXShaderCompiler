//===- llvm37/unittest/Object/YAMLTest.cpp - Tests for Object YAML ----------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/MC/YAML.h"
#include "llvm37/Support/YAMLTraits.h"
#include "gtest/gtest.h"

using namespace llvm37;

struct BinaryHolder {
  yaml::BinaryRef Binary;
};

namespace llvm37 {
namespace yaml {
template <>
struct MappingTraits<BinaryHolder> {
  static void mapping(IO &IO, BinaryHolder &BH) {
    IO.mapRequired("Binary", BH.Binary);
  }
};
} // end namespace yaml
} // end namespace llvm37

TEST(ObjectYAML, BinaryRef) {
  BinaryHolder BH;
  SmallVector<char, 32> Buf;
  llvm37::raw_svector_ostream OS(Buf);
  yaml::Output YOut(OS);
  YOut << BH;
  EXPECT_NE(OS.str().find("''"), StringRef::npos);
}
