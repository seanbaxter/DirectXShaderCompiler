//===- PostOrderIteratorTest.cpp - PostOrderIterator unit tests -----------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "gtest/gtest.h"
#include "llvm37/ADT/PostOrderIterator.h"
#include "llvm37/IR/BasicBlock.h"
#include "llvm37/IR/CFG.h"
using namespace llvm37;
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
namespace {

// Whether we're able to compile
TEST(PostOrderIteratorTest, Compiles) {
  typedef SmallPtrSet<void *, 4> ExtSetTy;

  // Tests that template specializations are kept up to date
  void *Null = nullptr;
  po_iterator_storage<std::set<void *>, false> PIS;
  PIS.insertEdge(Null, Null);
  ExtSetTy Ext;
  po_iterator_storage<ExtSetTy, true> PISExt(Ext);
  PIS.insertEdge(Null, Null);

  // Test above, but going through po_iterator (which inherits from template
  // base)
  BasicBlock *NullBB = nullptr;
  auto PI = po_end(NullBB);
  PI.insertEdge(NullBB, NullBB);
  auto PIExt = po_ext_end(NullBB, Ext);
  PIExt.insertEdge(NullBB, NullBB);
}
}
