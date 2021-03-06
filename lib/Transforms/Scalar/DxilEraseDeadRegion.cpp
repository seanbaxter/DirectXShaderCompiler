//===- DxilEraseDeadRegion.cpp - Heuristically Remove Dead Region ---------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// Overview:
//   1. Identify potentially dead regions by finding blocks with multiple
//      predecessors but no PHIs
//   2. Find common dominant ancestor of all the predecessors
//   3. Ensure original block post-dominates the ancestor
//   4. Ensure no instructions in the region have side effects (not including
//      original block and ancestor)
//   5. Remove all blocks in the region (excluding original block and ancestor)
//

#include "llvm37/Pass.h"
#include "llvm37/Analysis/CFG.h"
#include "llvm37/Analysis/PostDominators.h"
#include "llvm37/Transforms/Scalar.h"
#include "llvm37/IR/Instructions.h"
#include "llvm37/IR/IntrinsicInst.h"
#include "llvm37/IR/Function.h"
#include "llvm37/IR/Module.h"
#include "llvm37/IR/BasicBlock.h"

#include "dxc/DXIL/DxilOperations.h"

#include <unordered_map>
#include <unordered_set>

using namespace llvm37;
using namespace hlsl;

struct DxilEraseDeadRegion : public FunctionPass {
  static char ID;

  DxilEraseDeadRegion() : FunctionPass(ID) {
    initializeDxilEraseDeadRegionPass(*PassRegistry::getPassRegistry());
  }

  std::unordered_map<BasicBlock *, bool> m_HasSideEffect;

  bool HasSideEffects(BasicBlock *BB) {
    auto FindIt = m_HasSideEffect.find(BB);
    if (FindIt != m_HasSideEffect.end()) {
      return FindIt->second;
    }

    for (Instruction &I : *BB)
      if (I.mayHaveSideEffects()) {
        m_HasSideEffect[BB] = true;
        return true;
      }

    m_HasSideEffect[BB] = false;
    return false;
  }
  bool IsEmptySelfLoop(BasicBlock *BB) {
    // Make sure all inst not used outside BB.
    for (Instruction &I : *BB) {
      for (User *U : I.users()) {
        if (Instruction *UI = dyn_cast<Instruction>(U)) {
          if (UI->getParent() != BB)
            return false;
        }
      }

      if (!I.mayHaveSideEffects())
        continue;

      if (CallInst *CI = dyn_cast<CallInst>(&I)) {
        if (hlsl::OP::IsDxilOpFuncCallInst(CI)) {
          DXIL::OpCode opcode = hlsl::OP::GetDxilOpFuncCallInst(CI);
          // Wave Ops are marked has side effect to avoid move cross control flow.
          // But they're safe to remove if unused.
          if (hlsl::OP::IsDxilOpWave(opcode))
            continue;
        }
      }

      return false;
    }
    return true;
  }
  bool FindDeadRegion(BasicBlock *Begin, BasicBlock *End, std::set<BasicBlock *> &Region) {
    std::vector<BasicBlock *> WorkList;
    auto ProcessSuccessors = [this, &WorkList, Begin, End, &Region](BasicBlock *BB) {
      for (BasicBlock *Succ : successors(BB)) {
        if (Succ == End) continue;
        if (Succ == Begin) return false; // If goes back to the beginning, there's a loop, give up.
        if (Region.count(Succ)) continue;
        if (this->HasSideEffects(Succ)) return false; // Give up if the block may have side effects

        WorkList.push_back(Succ);
        Region.insert(Succ);
      }
      return true;
    };

    if (!ProcessSuccessors(Begin))
      return false;

    while (WorkList.size()) {
      BasicBlock *BB = WorkList.back();
      WorkList.pop_back();
      if (!ProcessSuccessors(BB))
        return false;
    }

    return Region.size() != 0;
  }

  bool TrySimplify(DominatorTree *DT, PostDominatorTree *PDT, BasicBlock *BB) {
    // Give up if BB has any Phis
    if (BB->begin() != BB->end() && isa<PHINode>(BB->begin()))
      return false;

    std::vector<BasicBlock *> Predecessors(pred_begin(BB), pred_end(BB));
    if (Predecessors.size() < 2) return false;

    // Give up if BB is a self loop
    for (BasicBlock *PredBB : Predecessors)
      if (PredBB == BB) {
        if (!IsEmptySelfLoop(BB)) {
          return false;
        } else if (Predecessors.size() != 2) {
          return false;
        } else {
          BasicBlock *PredBB0 = Predecessors[0];
          BasicBlock *PredBB1 = Predecessors[1];
          BasicBlock *LoopBB = PredBB;
          BasicBlock *LoopPrevBB = PredBB == PredBB0? PredBB1 : PredBB0;
          // Remove LoopBB, LoopPrevBB branch to succ of LoopBB.
          BranchInst *BI = cast<BranchInst>(LoopBB->getTerminator());

          if (BI->getNumSuccessors() != 2)
            return false;

          BasicBlock *Succ0 = BI->getSuccessor(0);
          BasicBlock *Succ1 = BI->getSuccessor(1);
          BasicBlock *NextBB = Succ0 == PredBB ? Succ1 : Succ0;
          // Make sure it is not a dead loop.
          if (NextBB == LoopPrevBB || NextBB == BB)
            return false;

          LoopPrevBB->getTerminator()->eraseFromParent();
          BranchInst::Create(NextBB, LoopPrevBB);
          return true;
        }
      }

    // Find the common ancestor of all the predecessors
    BasicBlock *Common = DT->findNearestCommonDominator(Predecessors[0], Predecessors[1]);
    if (!Common) return false;
    for (unsigned i = 2; i < Predecessors.size(); i++) {
      Common = DT->findNearestCommonDominator(Common, Predecessors[i]);
      if (!Common) return false;
    }

   // If there are any metadata on Common block's branch, give up.
    if (Common->getTerminator()->hasMetadataOtherThanDebugLoc())
      return false;

    if (!DT->properlyDominates(Common, BB))
      return false;
    if (!PDT->properlyDominates(BB, Common))
      return false;

    std::set<BasicBlock *> Region;
    if (!this->FindDeadRegion(Common, BB, Region))
      return false;

    // If BB branches INTO the region, forming a loop give up.
    for (BasicBlock *Succ : successors(BB))
      if (Region.count(Succ))
        return false;

    // Replace Common's branch with an unconditional branch to BB
    Common->getTerminator()->eraseFromParent();
    BranchInst::Create(BB, Common);

    // Delete the region
    for (BasicBlock *BB : Region) {
      for (Instruction &I : *BB)
        I.dropAllReferences();
      BB->dropAllReferences();
    }
    for (BasicBlock *BB : Region) {
      while (BB->begin() != BB->end())
        BB->begin()->eraseFromParent();
      BB->eraseFromParent();
    }

    return true;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<PostDominatorTree>();
  }

  bool runOnFunction(Function &F) override {
    auto *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    auto *PDT = &getAnalysis<PostDominatorTree>();

    std::unordered_set<BasicBlock *> FailedSet;
    bool Changed = false;
    while (1) {
      bool LocalChanged = false;
      for (Function::iterator It = F.begin(), E = F.end(); It != E; It++) {
        BasicBlock &BB = *It;
        if (FailedSet.count(&BB))
          continue;

        if (this->TrySimplify(DT, PDT, &BB)) {
          LocalChanged = true;
          break;
        }
        else {
          FailedSet.insert(&BB);
        }
      }

      Changed |= LocalChanged;
      if (!LocalChanged)
        break;
    }

    return Changed;
  }
};

char DxilEraseDeadRegion::ID;

Pass *llvm37::createDxilEraseDeadRegionPass() {
  return new DxilEraseDeadRegion();
}

INITIALIZELLVM37_PASS_BEGIN(DxilEraseDeadRegion, "dxil-erase-dead-region", "Dxil Erase Dead Region", false, false)
INITIALIZELLVM37_PASS_DEPENDENCY(DominatorTreeWrapperPass)
INITIALIZELLVM37_PASS_DEPENDENCY(PostDominatorTree)
INITIALIZELLVM37_PASS_END(DxilEraseDeadRegion, "dxil-erase-dead-region", "Dxil Erase Dead Region", false, false)


