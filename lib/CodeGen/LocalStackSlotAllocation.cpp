//===- LocalStackSlotAllocation.cpp - Pre-allocate locals to stack slots --===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This pass assigns local frame indices to stack slots relative to one another
// and allocates additional base registers to access them when the target
// estimates they are likely to be out of range of stack pointer and frame
// pointer relative addressing.
//
//===----------------------------------------------------------------------===//

#include "llvm37/CodeGen/Passes.h"
#include "llvm37/ADT/STLExtras.h"
#include "llvm37/ADT/SetVector.h"
#include "llvm37/ADT/SmallSet.h"
#include "llvm37/ADT/Statistic.h"
#include "llvm37/CodeGen/MachineFrameInfo.h"
#include "llvm37/CodeGen/MachineFunction.h"
#include "llvm37/CodeGen/MachineFunctionPass.h"
#include "llvm37/CodeGen/MachineRegisterInfo.h"
#include "llvm37/CodeGen/StackProtector.h"
#include "llvm37/IR/Constants.h"
#include "llvm37/IR/DerivedTypes.h"
#include "llvm37/IR/Instructions.h"
#include "llvm37/IR/Intrinsics.h"
#include "llvm37/IR/LLVMContext.h"
#include "llvm37/IR/Module.h"
#include "llvm37/Pass.h"
#include "llvm37/Support/Debug.h"
#include "llvm37/Support/ErrorHandling.h"
#include "llvm37/Support/raw_ostream.h"
#include "llvm37/Target/TargetFrameLowering.h"
#include "llvm37/Target/TargetRegisterInfo.h"
#include "llvm37/Target/TargetSubtargetInfo.h"

using namespace llvm37;

#define DEBUG_TYPE "localstackalloc"

STATISTIC(NumAllocations, "Number of frame indices allocated into local block");
STATISTIC(NumBaseRegisters, "Number of virtual frame base registers allocated");
STATISTIC(NumReplacements, "Number of frame indices references replaced");

namespace {
  class FrameRef {
    MachineBasicBlock::iterator MI; // Instr referencing the frame
    int64_t LocalOffset;            // Local offset of the frame idx referenced
    int FrameIdx;                   // The frame index
  public:
    FrameRef(MachineBasicBlock::iterator I, int64_t Offset, int Idx) :
      MI(I), LocalOffset(Offset), FrameIdx(Idx) {}
    bool operator<(const FrameRef &RHS) const {
      return LocalOffset < RHS.LocalOffset;
    }
    MachineBasicBlock::iterator getMachineInstr() const { return MI; }
    int64_t getLocalOffset() const { return LocalOffset; }
    int getFrameIndex() const { return FrameIdx; }
  };

  class LocalStackSlotPass: public MachineFunctionPass {
    SmallVector<int64_t,16> LocalOffsets;
    /// StackObjSet - A set of stack object indexes
    typedef SmallSetVector<int, 8> StackObjSet;

    void AdjustStackOffset(MachineFrameInfo *MFI, int FrameIdx, int64_t &Offset,
                           bool StackGrowsDown, unsigned &MaxAlign);
    void AssignProtectedObjSet(const StackObjSet &UnassignedObjs,
                               SmallSet<int, 16> &ProtectedObjs,
                               MachineFrameInfo *MFI, bool StackGrowsDown,
                               int64_t &Offset, unsigned &MaxAlign);
    void calculateFrameObjectOffsets(MachineFunction &Fn);
    bool insertFrameReferenceRegisters(MachineFunction &Fn);
  public:
    static char ID; // Pass identification, replacement for typeid
    explicit LocalStackSlotPass() : MachineFunctionPass(ID) { 
      initializeLocalStackSlotPassPass(*PassRegistry::getPassRegistry());
    }
    bool runOnMachineFunction(MachineFunction &MF) override;

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesCFG();
      AU.addRequired<StackProtector>();
      MachineFunctionPass::getAnalysisUsage(AU);
    }

  private:
  };
} // end anonymous namespace

char LocalStackSlotPass::ID = 0;
char &llvm37::LocalStackSlotAllocationID = LocalStackSlotPass::ID;
INITIALIZELLVM37_PASS_BEGIN(LocalStackSlotPass, "localstackalloc",
                      "Local Stack Slot Allocation", false, false)
INITIALIZELLVM37_PASS_DEPENDENCY(StackProtector)
INITIALIZELLVM37_PASS_END(LocalStackSlotPass, "localstackalloc",
                    "Local Stack Slot Allocation", false, false)


bool LocalStackSlotPass::runOnMachineFunction(MachineFunction &MF) {
  MachineFrameInfo *MFI = MF.getFrameInfo();
  const TargetRegisterInfo *TRI = MF.getSubtarget().getRegisterInfo();
  unsigned LocalObjectCount = MFI->getObjectIndexEnd();

  // If the target doesn't want/need this pass, or if there are no locals
  // to consider, early exit.
  if (!TRI->requiresVirtualBaseRegisters(MF) || LocalObjectCount == 0)
    return true;

  // Make sure we have enough space to store the local offsets.
  LocalOffsets.resize(MFI->getObjectIndexEnd());

  // Lay out the local blob.
  calculateFrameObjectOffsets(MF);

  // Insert virtual base registers to resolve frame index references.
  bool UsedBaseRegs = insertFrameReferenceRegisters(MF);

  // Tell MFI whether any base registers were allocated. PEI will only
  // want to use the local block allocations from this pass if there were any.
  // Otherwise, PEI can do a bit better job of getting the alignment right
  // without a hole at the start since it knows the alignment of the stack
  // at the start of local allocation, and this pass doesn't.
  MFI->setUseLocalStackAllocationBlock(UsedBaseRegs);

  return true;
}

/// AdjustStackOffset - Helper function used to adjust the stack frame offset.
void LocalStackSlotPass::AdjustStackOffset(MachineFrameInfo *MFI,
                                           int FrameIdx, int64_t &Offset,
                                           bool StackGrowsDown,
                                           unsigned &MaxAlign) {
  // If the stack grows down, add the object size to find the lowest address.
  if (StackGrowsDown)
    Offset += MFI->getObjectSize(FrameIdx);

  unsigned Align = MFI->getObjectAlignment(FrameIdx);

  // If the alignment of this object is greater than that of the stack, then
  // increase the stack alignment to match.
  MaxAlign = std::max(MaxAlign, Align);

  // Adjust to alignment boundary.
  Offset = (Offset + Align - 1) / Align * Align;

  int64_t LocalOffset = StackGrowsDown ? -Offset : Offset;
  DEBUG(dbgs() << "Allocate FI(" << FrameIdx << ") to local offset "
        << LocalOffset << "\n");
  // Keep the offset available for base register allocation
  LocalOffsets[FrameIdx] = LocalOffset;
  // And tell MFI about it for PEI to use later
  MFI->mapLocalFrameObject(FrameIdx, LocalOffset);

  if (!StackGrowsDown)
    Offset += MFI->getObjectSize(FrameIdx);

  ++NumAllocations;
}

/// AssignProtectedObjSet - Helper function to assign large stack objects (i.e.,
/// those required to be close to the Stack Protector) to stack offsets.
void LocalStackSlotPass::AssignProtectedObjSet(const StackObjSet &UnassignedObjs,
                                           SmallSet<int, 16> &ProtectedObjs,
                                           MachineFrameInfo *MFI,
                                           bool StackGrowsDown, int64_t &Offset,
                                           unsigned &MaxAlign) {

  for (StackObjSet::const_iterator I = UnassignedObjs.begin(),
        E = UnassignedObjs.end(); I != E; ++I) {
    int i = *I;
    AdjustStackOffset(MFI, i, Offset, StackGrowsDown, MaxAlign);
    ProtectedObjs.insert(i);
  }
}

/// calculateFrameObjectOffsets - Calculate actual frame offsets for all of the
/// abstract stack objects.
///
void LocalStackSlotPass::calculateFrameObjectOffsets(MachineFunction &Fn) {
  // Loop over all of the stack objects, assigning sequential addresses...
  MachineFrameInfo *MFI = Fn.getFrameInfo();
  const TargetFrameLowering &TFI = *Fn.getSubtarget().getFrameLowering();
  bool StackGrowsDown =
    TFI.getStackGrowthDirection() == TargetFrameLowering::StackGrowsDown;
  int64_t Offset = 0;
  unsigned MaxAlign = 0;
  StackProtector *SP = &getAnalysis<StackProtector>();

  // Make sure that the stack protector comes before the local variables on the
  // stack.
  SmallSet<int, 16> ProtectedObjs;
  if (MFI->getStackProtectorIndex() >= 0) {
    StackObjSet LargeArrayObjs;
    StackObjSet SmallArrayObjs;
    StackObjSet AddrOfObjs;

    AdjustStackOffset(MFI, MFI->getStackProtectorIndex(), Offset,
                      StackGrowsDown, MaxAlign);

    // Assign large stack objects first.
    for (unsigned i = 0, e = MFI->getObjectIndexEnd(); i != e; ++i) {
      if (MFI->isDeadObjectIndex(i))
        continue;
      if (MFI->getStackProtectorIndex() == (int)i)
        continue;

      switch (SP->getSSPLayout(MFI->getObjectAllocation(i))) {
      case StackProtector::SSPLK_None:
        continue;
      case StackProtector::SSPLK_SmallArray:
        SmallArrayObjs.insert(i);
        continue;
      case StackProtector::SSPLK_AddrOf:
        AddrOfObjs.insert(i);
        continue;
      case StackProtector::SSPLK_LargeArray:
        LargeArrayObjs.insert(i);
        continue;
      }
      llvm37_unreachable("Unexpected SSPLayoutKind.");
    }

    AssignProtectedObjSet(LargeArrayObjs, ProtectedObjs, MFI, StackGrowsDown,
                          Offset, MaxAlign);
    AssignProtectedObjSet(SmallArrayObjs, ProtectedObjs, MFI, StackGrowsDown,
                          Offset, MaxAlign);
    AssignProtectedObjSet(AddrOfObjs, ProtectedObjs, MFI, StackGrowsDown,
                          Offset, MaxAlign);
  }

  // Then assign frame offsets to stack objects that are not used to spill
  // callee saved registers.
  for (unsigned i = 0, e = MFI->getObjectIndexEnd(); i != e; ++i) {
    if (MFI->isDeadObjectIndex(i))
      continue;
    if (MFI->getStackProtectorIndex() == (int)i)
      continue;
    if (ProtectedObjs.count(i))
      continue;

    AdjustStackOffset(MFI, i, Offset, StackGrowsDown, MaxAlign);
  }

  // Remember how big this blob of stack space is
  MFI->setLocalFrameSize(Offset);
  MFI->setLocalFrameMaxAlign(MaxAlign);
}

static inline bool
lookupCandidateBaseReg(unsigned BaseReg,
                       int64_t BaseOffset,
                       int64_t FrameSizeAdjust,
                       int64_t LocalFrameOffset,
                       const MachineInstr *MI,
                       const TargetRegisterInfo *TRI) {
  // Check if the relative offset from the where the base register references
  // to the target address is in range for the instruction.
  int64_t Offset = FrameSizeAdjust + LocalFrameOffset - BaseOffset;
  return TRI->isFrameOffsetLegal(MI, BaseReg, Offset);
}

bool LocalStackSlotPass::insertFrameReferenceRegisters(MachineFunction &Fn) {
  // Scan the function's instructions looking for frame index references.
  // For each, ask the target if it wants a virtual base register for it
  // based on what we can tell it about where the local will end up in the
  // stack frame. If it wants one, re-use a suitable one we've previously
  // allocated, or if there isn't one that fits the bill, allocate a new one
  // and ask the target to create a defining instruction for it.
  bool UsedBaseReg = false;

  MachineFrameInfo *MFI = Fn.getFrameInfo();
  const TargetRegisterInfo *TRI = Fn.getSubtarget().getRegisterInfo();
  const TargetFrameLowering &TFI = *Fn.getSubtarget().getFrameLowering();
  bool StackGrowsDown =
    TFI.getStackGrowthDirection() == TargetFrameLowering::StackGrowsDown;

  // Collect all of the instructions in the block that reference
  // a frame index. Also store the frame index referenced to ease later
  // lookup. (For any insn that has more than one FI reference, we arbitrarily
  // choose the first one).
  SmallVector<FrameRef, 64> FrameReferenceInsns;

  for (MachineFunction::iterator BB = Fn.begin(), E = Fn.end(); BB != E; ++BB) {
    for (MachineBasicBlock::iterator I = BB->begin(); I != BB->end(); ++I) {
      MachineInstr *MI = I;

      // Debug value, stackmap and patchpoint instructions can't be out of
      // range, so they don't need any updates.
      if (MI->isDebugValue() ||
          MI->getOpcode() == TargetOpcode::STATEPOINT ||
          MI->getOpcode() == TargetOpcode::STACKMAP ||
          MI->getOpcode() == TargetOpcode::PATCHPOINT)
        continue;

      // For now, allocate the base register(s) within the basic block
      // where they're used, and don't try to keep them around outside
      // of that. It may be beneficial to try sharing them more broadly
      // than that, but the increased register pressure makes that a
      // tricky thing to balance. Investigate if re-materializing these
      // becomes an issue.
      for (unsigned i = 0, e = MI->getNumOperands(); i != e; ++i) {
        // Consider replacing all frame index operands that reference
        // an object allocated in the local block.
        if (MI->getOperand(i).isFI()) {
          // Don't try this with values not in the local block.
          if (!MFI->isObjectPreAllocated(MI->getOperand(i).getIndex()))
            break;
          int Idx = MI->getOperand(i).getIndex();
          int64_t LocalOffset = LocalOffsets[Idx];
          if (!TRI->needsFrameBaseReg(MI, LocalOffset))
            break;
          FrameReferenceInsns.
            push_back(FrameRef(MI, LocalOffset, Idx));
          break;
        }
      }
    }
  }

  // Sort the frame references by local offset
  array_pod_sort(FrameReferenceInsns.begin(), FrameReferenceInsns.end());

  MachineBasicBlock *Entry = Fn.begin();

  unsigned BaseReg = 0;
  int64_t BaseOffset = 0;

  // Loop through the frame references and allocate for them as necessary.
  for (int ref = 0, e = FrameReferenceInsns.size(); ref < e ; ++ref) {
    FrameRef &FR = FrameReferenceInsns[ref];
    MachineBasicBlock::iterator I = FR.getMachineInstr();
    MachineInstr *MI = I;
    int64_t LocalOffset = FR.getLocalOffset();
    int FrameIdx = FR.getFrameIndex();
    assert(MFI->isObjectPreAllocated(FrameIdx) &&
           "Only pre-allocated locals expected!");

    DEBUG(dbgs() << "Considering: " << *MI);

    unsigned idx = 0;
    for (unsigned f = MI->getNumOperands(); idx != f; ++idx) {
      if (!MI->getOperand(idx).isFI())
        continue;

      if (FrameIdx == I->getOperand(idx).getIndex())
        break;
    }

    assert(idx < MI->getNumOperands() && "Cannot find FI operand");

    int64_t Offset = 0;
    int64_t FrameSizeAdjust = StackGrowsDown ? MFI->getLocalFrameSize() : 0;

    DEBUG(dbgs() << "  Replacing FI in: " << *MI);

    // If we have a suitable base register available, use it; otherwise
    // create a new one. Note that any offset encoded in the
    // instruction itself will be taken into account by the target,
    // so we don't have to adjust for it here when reusing a base
    // register.
    if (UsedBaseReg && lookupCandidateBaseReg(BaseReg, BaseOffset,
                                              FrameSizeAdjust, LocalOffset, MI,
                                              TRI)) {
      DEBUG(dbgs() << "  Reusing base register " << BaseReg << "\n");
      // We found a register to reuse.
      Offset = FrameSizeAdjust + LocalOffset - BaseOffset;
    } else {
      // No previously defined register was in range, so create a // new one.
 
      int64_t InstrOffset = TRI->getFrameIndexInstrOffset(MI, idx);

      int64_t PrevBaseOffset = BaseOffset;
      BaseOffset = FrameSizeAdjust + LocalOffset + InstrOffset;

      // We'd like to avoid creating single-use virtual base registers.
      // Because the FrameRefs are in sorted order, and we've already
      // processed all FrameRefs before this one, just check whether or not
      // the next FrameRef will be able to reuse this new register. If not,
      // then don't bother creating it.
      if (ref + 1 >= e ||
          !lookupCandidateBaseReg(
              BaseReg, BaseOffset, FrameSizeAdjust,
              FrameReferenceInsns[ref + 1].getLocalOffset(),
              FrameReferenceInsns[ref + 1].getMachineInstr(), TRI)) {
        BaseOffset = PrevBaseOffset;
        continue;
      }

      const MachineFunction *MF = MI->getParent()->getParent();
      const TargetRegisterClass *RC = TRI->getPointerRegClass(*MF);
      BaseReg = Fn.getRegInfo().createVirtualRegister(RC);

      DEBUG(dbgs() << "  Materializing base register " << BaseReg <<
            " at frame local offset " << LocalOffset + InstrOffset << "\n");

      // Tell the target to insert the instruction to initialize
      // the base register.
      //            MachineBasicBlock::iterator InsertionPt = Entry->begin();
      TRI->materializeFrameBaseRegister(Entry, BaseReg, FrameIdx,
                                        InstrOffset);

      // The base register already includes any offset specified
      // by the instruction, so account for that so it doesn't get
      // applied twice.
      Offset = -InstrOffset;

      ++NumBaseRegisters;
      UsedBaseReg = true;
    }
    assert(BaseReg != 0 && "Unable to allocate virtual base register!");

    // Modify the instruction to use the new base register rather
    // than the frame index operand.
    TRI->resolveFrameIndex(*I, BaseReg, Offset);
    DEBUG(dbgs() << "Resolved: " << *MI);

    ++NumReplacements;
  }

  return UsedBaseReg;
}
