//===--------- DxilValueCache.h - Dxil Constant Value Cache --------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_ANALYSIS_DXILVALUECACHE_H
#define LLVM37_ANALYSIS_DXILVALUECACHE_H

#include "llvm37/Pass.h"
#include "llvm37/IR/ValueMap.h"

namespace llvm37 {

class Module;
class DominatorTree;
class Constant;
class ConstantInt;

struct DxilValueCache : public ImmutablePass {
  static char ID;

  // Special Weak Value to Weak Value map.
  struct WeakValueMap {
    struct ValueVH : public CallbackVH {
      ValueVH(Value *V) : CallbackVH(V) {}
      void allUsesReplacedWith(Value *) override { setValPtr(nullptr); }
    };
    struct ValueEntry {
      WeakVH Value;
      ValueVH Self;
      ValueEntry() : Value(nullptr), Self(nullptr) {}
      inline void Set(llvm37::Value *Key, llvm37::Value *V) { Self = Key; Value = V; }
      inline bool IsStale() const { return Self == nullptr; }
    };
    ValueMap<const Value *, ValueEntry> Map;
    Value *Get(Value *V);
    void Set(Value *Key, Value *V);
    bool Seen(Value *v);
    void SetSentinel(Value *V);
    void ResetUnknowns();
    void dump() const;
  private:
    Value *GetSentinel(LLVM37Context &Ctx);
    std::unique_ptr<Value> Sentinel;
  };

private:

  WeakValueMap ValueMap;

  void MarkAlwaysReachable(BasicBlock *BB);
  void MarkUnreachable(BasicBlock *BB);
  bool IsAlwaysReachable_(BasicBlock *BB);
  bool IsUnreachable_(BasicBlock *BB);
  bool MayBranchTo(BasicBlock *A, BasicBlock *B);
  Value *TryGetCachedValue(Value *V);
  Value *ProcessValue(Value *V, DominatorTree *DT);

  Value *ProcessAndSimplify_PHI(Instruction *I, DominatorTree *DT);
  Value *ProcessAndSimplify_Br(Instruction *I, DominatorTree *DT);
  Value *ProcessAndSimplify_Load(Instruction *LI, DominatorTree *DT);
  Value *SimplifyAndCacheResult(Instruction *I, DominatorTree *DT);

public:

  const char *getPassName() const override;
  DxilValueCache();
  void getAnalysisUsage(AnalysisUsage &) const override;

  void dump() const;
  Value *GetValue(Value *V, DominatorTree *DT=nullptr);
  Constant *GetConstValue(Value *V, DominatorTree *DT = nullptr);
  ConstantInt *GetConstInt(Value *V, DominatorTree *DT = nullptr);
  void ResetUnknowns() { ValueMap.ResetUnknowns(); }
  bool IsAlwaysReachable(BasicBlock *BB, DominatorTree *DT=nullptr);
  bool IsUnreachable(BasicBlock *BB, DominatorTree *DT=nullptr);
};

void initializeDxilValueCachePass(class llvm37::PassRegistry &);
Pass *createDxilValueCachePass();

}

#endif
