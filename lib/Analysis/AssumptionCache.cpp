//===- AssumptionCache.cpp - Cache finding @llvm37.assume calls -------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a pass that keeps track of @llvm37.assume intrinsics in
// the functions of a module.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Analysis/AssumptionCache.h"
#include "llvm37/IR/CallSite.h"
#include "llvm37/IR/Dominators.h"
#include "llvm37/IR/Function.h"
#include "llvm37/IR/Instructions.h"
#include "llvm37/IR/IntrinsicInst.h"
#include "llvm37/IR/PassManager.h"
#include "llvm37/IR/PatternMatch.h"
#include "llvm37/Support/Debug.h"
using namespace llvm37;
using namespace llvm37::PatternMatch;

void AssumptionCache::scanFunction() {
  assert(!Scanned && "Tried to scan the function twice!");
  assert(AssumeHandles.empty() && "Already have assumes when scanning!");

  // Go through all instructions in all blocks, add all calls to @llvm37.assume
  // to this cache.
  for (BasicBlock &B : F)
    for (Instruction &II : B)
      if (match(&II, m_Intrinsic<Intrinsic::assume>()))
        AssumeHandles.push_back(&II);

  // Mark the scan as complete.
  Scanned = true;
}

void AssumptionCache::registerAssumption(CallInst *CI) {
  assert(match(CI, m_Intrinsic<Intrinsic::assume>()) &&
         "Registered call does not call @llvm37.assume");

  // If we haven't scanned the function yet, just drop this assumption. It will
  // be found when we scan later.
  if (!Scanned)
    return;

  AssumeHandles.push_back(CI);

#ifndef NDEBUG
  assert(CI->getParent() &&
         "Cannot register @llvm37.assume call not in a basic block");
  assert(&F == CI->getParent()->getParent() &&
         "Cannot register @llvm37.assume call not in this function");

  // We expect the number of assumptions to be small, so in an asserts build
  // check that we don't accumulate duplicates and that all assumptions point
  // to the same function.
  SmallPtrSet<Value *, 16> AssumptionSet;
  for (auto &VH : AssumeHandles) {
    if (!VH)
      continue;

    assert(&F == cast<Instruction>(VH)->getParent()->getParent() &&
           "Cached assumption not inside this function!");
    assert(match(cast<CallInst>(VH), m_Intrinsic<Intrinsic::assume>()) &&
           "Cached something other than a call to @llvm37.assume!");
    assert(AssumptionSet.insert(VH).second &&
           "Cache contains multiple copies of a call!");
  }
#endif
}

char AssumptionAnalysis::PassID;

PreservedAnalyses AssumptionPrinterPass::run(Function &F,
                                             AnalysisManager<Function> *AM) {
  AssumptionCache &AC = AM->getResult<AssumptionAnalysis>(F);

  OS << "Cached assumptions for function: " << F.getName() << "\n";
  for (auto &VH : AC.assumptions())
    if (VH)
      OS << "  " << *cast<CallInst>(VH)->getArgOperand(0) << "\n";

  return PreservedAnalyses::all();
}

void AssumptionCacheTracker::FunctionCallbackVH::deleted() {
  auto I = ACT->AssumptionCaches.find_as(cast<Function>(getValPtr()));
  if (I != ACT->AssumptionCaches.end())
    ACT->AssumptionCaches.erase(I);
  // 'this' now dangles!
}

AssumptionCache &AssumptionCacheTracker::getAssumptionCache(Function &F) {
  // We probe the function map twice to try and avoid creating a value handle
  // around the function in common cases. This makes insertion a bit slower,
  // but if we have to insert we're going to scan the whole function so that
  // shouldn't matter.
  auto I = AssumptionCaches.find_as(&F);
  if (I != AssumptionCaches.end())
    return *I->second;

  // Ok, build a new cache by scanning the function, insert it and the value
  // handle into our map, and return the newly populated cache.
  auto IP = AssumptionCaches.insert(std::make_pair(
      FunctionCallbackVH(&F, this), llvm37::make_unique<AssumptionCache>(F)));
  assert(IP.second && "Scanning function already in the map?");
  return *IP.first->second;
}

void AssumptionCacheTracker::verifyAnalysis() const {
#ifndef NDEBUG
  SmallPtrSet<const CallInst *, 4> AssumptionSet;
  for (const auto &I : AssumptionCaches) {
    for (auto &VH : I.second->assumptions())
      if (VH)
        AssumptionSet.insert(cast<CallInst>(VH));

    for (const BasicBlock &B : cast<Function>(*I.first))
      for (const Instruction &II : B)
        if (match(&II, m_Intrinsic<Intrinsic::assume>()))
          assert(AssumptionSet.count(cast<CallInst>(&II)) &&
                 "Assumption in scanned function not in cache");
  }
#endif
}

AssumptionCacheTracker::AssumptionCacheTracker() : ImmutablePass(ID) {
  initializeAssumptionCacheTrackerPass(*PassRegistry::getPassRegistry());
}

AssumptionCacheTracker::~AssumptionCacheTracker() {}

INITIALIZELLVM37_PASS(AssumptionCacheTracker, "assumption-cache-tracker",
                "Assumption Cache Tracker", false, true)
char AssumptionCacheTracker::ID = 0;
