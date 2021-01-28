//===- RegionInfo.cpp - SESE region detection analysis --------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// Detects single entry single exit regions in the control flow graph.
//===----------------------------------------------------------------------===//

#include "llvm37/Analysis/RegionInfo.h"
#include "llvm37/ADT/PostOrderIterator.h"
#include "llvm37/ADT/Statistic.h"
#include "llvm37/Analysis/LoopInfo.h"
#include "llvm37/Analysis/RegionInfoImpl.h"
#include "llvm37/Analysis/RegionIterator.h"
#include "llvm37/Support/CommandLine.h"
#include "llvm37/Support/Debug.h"
#include "llvm37/Support/ErrorHandling.h"
#include <algorithm>
#include <iterator>
#include <set>

using namespace llvm37;

#define DEBUG_TYPE "region"

namespace llvm37 {
template class RegionBase<RegionTraits<Function>>;
template class RegionNodeBase<RegionTraits<Function>>;
template class RegionInfoBase<RegionTraits<Function>>;
}

STATISTIC(numRegions,       "The # of regions");
STATISTIC(numSimpleRegions, "The # of simple regions");

// Always verify if expensive checking is enabled.

#if 0 // HLSL Change Starts - option pending
static cl::opt<bool,true>
VerifyRegionInfoX(
  "verify-region-info",
  cl::location(RegionInfoBase<RegionTraits<Function>>::VerifyRegionInfo),
  cl::desc("Verify region info (time consuming)"));


static cl::opt<Region::PrintStyle, true> printStyleX("print-region-style",
  cl::location(RegionInfo::printStyle),
  cl::Hidden,
  cl::desc("style of printing regions"),
  cl::values(
    clEnumValN(Region::PrintNone, "none",  "print no details"),
    clEnumValN(Region::PrintBB, "bb",
               "print regions in detail with block_iterator"),
    clEnumValN(Region::PrintRN, "rn",
               "print regions in detail with element_iterator"),
    clEnumValEnd));
#else
#endif // HLSL Change Ends

//===----------------------------------------------------------------------===//
// Region implementation
//

Region::Region(BasicBlock *Entry, BasicBlock *Exit,
               RegionInfo* RI,
               DominatorTree *DT, Region *Parent) :
  RegionBase<RegionTraits<Function>>(Entry, Exit, RI, DT, Parent) {

}

Region::~Region() { }

//===----------------------------------------------------------------------===//
// RegionInfo implementation
//

RegionInfo::RegionInfo() :
  RegionInfoBase<RegionTraits<Function>>() {

}

RegionInfo::~RegionInfo() {

}

void RegionInfo::updateStatistics(Region *R) {
  ++numRegions;

  // TODO: Slow. Should only be enabled if -stats is used.
  if (R->isSimple())
    ++numSimpleRegions;
}

void RegionInfo::recalculate(Function &F, DominatorTree *DT_,
                             PostDominatorTree *PDT_, DominanceFrontier *DF_) {
  DT = DT_;
  PDT = PDT_;
  DF = DF_;

  TopLevelRegion = new Region(&F.getEntryBlock(), nullptr,
                              this, DT, nullptr);
  updateStatistics(TopLevelRegion);
  calculate(F);
}

//===----------------------------------------------------------------------===//
// RegionInfoPass implementation
//

RegionInfoPass::RegionInfoPass() : FunctionPass(ID) {
  initializeRegionInfoPassPass(*PassRegistry::getPassRegistry());
}

RegionInfoPass::~RegionInfoPass() {

}

bool RegionInfoPass::runOnFunction(Function &F) {
  releaseMemory();

  auto DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
  auto PDT = &getAnalysis<PostDominatorTree>();
  auto DF = &getAnalysis<DominanceFrontier>();

  RI.recalculate(F, DT, PDT, DF);
  return false;
}

void RegionInfoPass::releaseMemory() {
  RI.releaseMemory();
}

void RegionInfoPass::verifyAnalysis() const {
    RI.verifyAnalysis();
}

void RegionInfoPass::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
  AU.addRequiredTransitive<DominatorTreeWrapperPass>();
  AU.addRequired<PostDominatorTree>();
  AU.addRequired<DominanceFrontier>();
}

void RegionInfoPass::print(raw_ostream &OS, const Module *) const {
  RI.print(OS);
}

#if !defined(NDEBUG) || defined(LLVM37_ENABLE_DUMP)
void RegionInfoPass::dump() const {
  RI.dump();
}
#endif

char RegionInfoPass::ID = 0;

INITIALIZELLVM37_PASS_BEGIN(RegionInfoPass, "regions",
                "Detect single entry single exit regions", true, true)
INITIALIZELLVM37_PASS_DEPENDENCY(DominatorTreeWrapperPass)
INITIALIZELLVM37_PASS_DEPENDENCY(PostDominatorTree)
INITIALIZELLVM37_PASS_DEPENDENCY(DominanceFrontier)
INITIALIZELLVM37_PASS_END(RegionInfoPass, "regions",
                "Detect single entry single exit regions", true, true)

// Create methods available outside of this file, to use them
// "include/llvm37/LinkAllPasses.h". Otherwise the pass would be deleted by
// the link time optimization.

namespace llvm37 {
  FunctionPass *createRegionInfoPass() {
    return new RegionInfoPass();
  }
}

