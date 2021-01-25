//===-------- SplitKit.h - Toolkit for splitting live ranges ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the SplitAnalysis class as well as mutator functions for
// live range splitting.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_LIB_CODEGEN_SPLITKIT_H
#define LLVM37_LIB_CODEGEN_SPLITKIT_H

#include "LiveRangeCalc.h"
#include "llvm37/ADT/ArrayRef.h"
#include "llvm37/ADT/DenseMap.h"
#include "llvm37/ADT/IntervalMap.h"
#include "llvm37/ADT/SmallPtrSet.h"

namespace llvm37 {

class ConnectedVNInfoEqClasses;
class LiveInterval;
class LiveIntervals;
class LiveRangeEdit;
class MachineBlockFrequencyInfo;
class MachineInstr;
class MachineLoopInfo;
class MachineRegisterInfo;
class TargetInstrInfo;
class TargetRegisterInfo;
class VirtRegMap;
class VNInfo;
class raw_ostream;

/// SplitAnalysis - Analyze a LiveInterval, looking for live range splitting
/// opportunities.
class LLVM37_LIBRARY_VISIBILITY SplitAnalysis {
public:
  const MachineFunction &MF;
  const VirtRegMap &VRM;
  const LiveIntervals &LIS;
  const MachineLoopInfo &Loops;
  const TargetInstrInfo &TII;

  /// Additional information about basic blocks where the current variable is
  /// live. Such a block will look like one of these templates:
  ///
  ///  1. |   o---x   | Internal to block. Variable is only live in this block.
  ///  2. |---x       | Live-in, kill.
  ///  3. |       o---| Def, live-out.
  ///  4. |---x   o---| Live-in, kill, def, live-out. Counted by NumGapBlocks.
  ///  5. |---o---o---| Live-through with uses or defs.
  ///  6. |-----------| Live-through without uses. Counted by NumThroughBlocks.
  ///
  /// Two BlockInfo entries are created for template 4. One for the live-in
  /// segment, and one for the live-out segment. These entries look as if the
  /// block were split in the middle where the live range isn't live.
  ///
  /// Live-through blocks without any uses don't get BlockInfo entries. They
  /// are simply listed in ThroughBlocks instead.
  ///
  struct BlockInfo {
    MachineBasicBlock *MBB;
    SlotIndex FirstInstr; ///< First instr accessing current reg.
    SlotIndex LastInstr;  ///< Last instr accessing current reg.
    SlotIndex FirstDef;   ///< First non-phi valno->def, or SlotIndex().
    bool LiveIn;          ///< Current reg is live in.
    bool LiveOut;         ///< Current reg is live out.

    /// isOneInstr - Returns true when this BlockInfo describes a single
    /// instruction.
    bool isOneInstr() const {
      return SlotIndex::isSameInstr(FirstInstr, LastInstr);
    }
  };

private:
  // Current live interval.
  const LiveInterval *CurLI;

  // Sorted slot indexes of using instructions.
  SmallVector<SlotIndex, 8> UseSlots;

  /// LastSplitPoint - Last legal split point in each basic block in the current
  /// function. The first entry is the first terminator, the second entry is the
  /// last valid split point for a variable that is live in to a landing pad
  /// successor.
  SmallVector<std::pair<SlotIndex, SlotIndex>, 8> LastSplitPoint;

  /// UseBlocks - Blocks where CurLI has uses.
  SmallVector<BlockInfo, 8> UseBlocks;

  /// NumGapBlocks - Number of duplicate entries in UseBlocks for blocks where
  /// the live range has a gap.
  unsigned NumGapBlocks;

  /// ThroughBlocks - Block numbers where CurLI is live through without uses.
  BitVector ThroughBlocks;

  /// NumThroughBlocks - Number of live-through blocks.
  unsigned NumThroughBlocks;

  /// DidRepairRange - analyze was forced to shrinkToUses().
  bool DidRepairRange;

  SlotIndex computeLastSplitPoint(unsigned Num);

  // Sumarize statistics by counting instructions using CurLI.
  void analyzeUses();

  /// calcLiveBlockInfo - Compute per-block information about CurLI.
  bool calcLiveBlockInfo();

public:
  SplitAnalysis(const VirtRegMap &vrm, const LiveIntervals &lis,
                const MachineLoopInfo &mli);

  /// analyze - set CurLI to the specified interval, and analyze how it may be
  /// split.
  void analyze(const LiveInterval *li);

  /// didRepairRange() - Returns true if CurLI was invalid and has been repaired
  /// by analyze(). This really shouldn't happen, but sometimes the coalescer
  /// can create live ranges that end in mid-air.
  bool didRepairRange() const { return DidRepairRange; }

  /// clear - clear all data structures so SplitAnalysis is ready to analyze a
  /// new interval.
  void clear();

  /// getParent - Return the last analyzed interval.
  const LiveInterval &getParent() const { return *CurLI; }

  /// getLastSplitPoint - Return the base index of the last valid split point
  /// in the basic block numbered Num.
  SlotIndex getLastSplitPoint(unsigned Num) {
    // Inline the common simple case.
    if (LastSplitPoint[Num].first.isValid() &&
        !LastSplitPoint[Num].second.isValid())
      return LastSplitPoint[Num].first;
    return computeLastSplitPoint(Num);
  }

  /// getLastSplitPointIter - Returns the last split point as an iterator.
  MachineBasicBlock::iterator getLastSplitPointIter(MachineBasicBlock*);

  /// isOriginalEndpoint - Return true if the original live range was killed or
  /// (re-)defined at Idx. Idx should be the 'def' slot for a normal kill/def,
  /// and 'use' for an early-clobber def.
  /// This can be used to recognize code inserted by earlier live range
  /// splitting.
  bool isOriginalEndpoint(SlotIndex Idx) const;

  /// getUseSlots - Return an array of SlotIndexes of instructions using CurLI.
  /// This include both use and def operands, at most one entry per instruction.
  ArrayRef<SlotIndex> getUseSlots() const { return UseSlots; }

  /// getUseBlocks - Return an array of BlockInfo objects for the basic blocks
  /// where CurLI has uses.
  ArrayRef<BlockInfo> getUseBlocks() const { return UseBlocks; }

  /// getNumThroughBlocks - Return the number of through blocks.
  unsigned getNumThroughBlocks() const { return NumThroughBlocks; }

  /// isThroughBlock - Return true if CurLI is live through MBB without uses.
  bool isThroughBlock(unsigned MBB) const { return ThroughBlocks.test(MBB); }

  /// getThroughBlocks - Return the set of through blocks.
  const BitVector &getThroughBlocks() const { return ThroughBlocks; }

  /// getNumLiveBlocks - Return the number of blocks where CurLI is live.
  unsigned getNumLiveBlocks() const {
    return getUseBlocks().size() - NumGapBlocks + getNumThroughBlocks();
  }

  /// countLiveBlocks - Return the number of blocks where li is live. This is
  /// guaranteed to return the same number as getNumLiveBlocks() after calling
  /// analyze(li).
  unsigned countLiveBlocks(const LiveInterval *li) const;

  typedef SmallPtrSet<const MachineBasicBlock*, 16> BlockPtrSet;

  /// shouldSplitSingleBlock - Returns true if it would help to create a local
  /// live range for the instructions in BI. There is normally no benefit to
  /// creating a live range for a single instruction, but it does enable
  /// register class inflation if the instruction has a restricted register
  /// class.
  ///
  /// @param BI           The block to be isolated.
  /// @param SingleInstrs True when single instructions should be isolated.
  bool shouldSplitSingleBlock(const BlockInfo &BI, bool SingleInstrs) const;
};


/// SplitEditor - Edit machine code and LiveIntervals for live range
/// splitting.
///
/// - Create a SplitEditor from a SplitAnalysis.
/// - Start a new live interval with openIntv.
/// - Mark the places where the new interval is entered using enterIntv*
/// - Mark the ranges where the new interval is used with useIntv* 
/// - Mark the places where the interval is exited with exitIntv*.
/// - Finish the current interval with closeIntv and repeat from 2.
/// - Rewrite instructions with finish().
///
class LLVM37_LIBRARY_VISIBILITY SplitEditor {
  SplitAnalysis &SA;
  LiveIntervals &LIS;
  VirtRegMap &VRM;
  MachineRegisterInfo &MRI;
  MachineDominatorTree &MDT;
  const TargetInstrInfo &TII;
  const TargetRegisterInfo &TRI;
  const MachineBlockFrequencyInfo &MBFI;

public:

  /// ComplementSpillMode - Select how the complement live range should be
  /// created.  SplitEditor automatically creates interval 0 to contain
  /// anything that isn't added to another interval.  This complement interval
  /// can get quite complicated, and it can sometimes be an advantage to allow
  /// it to overlap the other intervals.  If it is going to spill anyway, no
  /// registers are wasted by keeping a value in two places at the same time.
  enum ComplementSpillMode {
    /// SM_Partition(Default) - Try to create the complement interval so it
    /// doesn't overlap any other intervals, and the original interval is
    /// partitioned.  This may require a large number of back copies and extra
    /// PHI-defs.  Only segments marked with overlapIntv will be overlapping.
    SM_Partition,

    /// SM_Size - Overlap intervals to minimize the number of inserted COPY
    /// instructions.  Copies to the complement interval are hoisted to their
    /// common dominator, so only one COPY is required per value in the
    /// complement interval.  This also means that no extra PHI-defs need to be
    /// inserted in the complement interval.
    SM_Size,

    /// SM_Speed - Overlap intervals to minimize the expected execution
    /// frequency of the inserted copies.  This is very similar to SM_Size, but
    /// the complement interval may get some extra PHI-defs.
    SM_Speed
  };

private:

  /// Edit - The current parent register and new intervals created.
  LiveRangeEdit *Edit;

  /// Index into Edit of the currently open interval.
  /// The index 0 is used for the complement, so the first interval started by
  /// openIntv will be 1.
  unsigned OpenIdx;

  /// The current spill mode, selected by reset().
  ComplementSpillMode SpillMode;

  typedef IntervalMap<SlotIndex, unsigned> RegAssignMap;

  /// Allocator for the interval map. This will eventually be shared with
  /// SlotIndexes and LiveIntervals.
  RegAssignMap::Allocator Allocator;

  /// RegAssign - Map of the assigned register indexes.
  /// Edit.get(RegAssign.lookup(Idx)) is the register that should be live at
  /// Idx.
  RegAssignMap RegAssign;

  typedef PointerIntPair<VNInfo*, 1> ValueForcePair;
  typedef DenseMap<std::pair<unsigned, unsigned>, ValueForcePair> ValueMap;

  /// Values - keep track of the mapping from parent values to values in the new
  /// intervals. Given a pair (RegIdx, ParentVNI->id), Values contains:
  ///
  /// 1. No entry - the value is not mapped to Edit.get(RegIdx).
  /// 2. (Null, false) - the value is mapped to multiple values in
  ///    Edit.get(RegIdx).  Each value is represented by a minimal live range at
  ///    its def.  The full live range can be inferred exactly from the range
  ///    of RegIdx in RegAssign.
  /// 3. (Null, true).  As above, but the ranges in RegAssign are too large, and
  ///    the live range must be recomputed using LiveRangeCalc::extend().
  /// 4. (VNI, false) The value is mapped to a single new value.
  ///    The new value has no live ranges anywhere.
  ValueMap Values;

  /// LRCalc - Cache for computing live ranges and SSA update.  Each instance
  /// can only handle non-overlapping live ranges, so use a separate
  /// LiveRangeCalc instance for the complement interval when in spill mode.
  LiveRangeCalc LRCalc[2];

  /// getLRCalc - Return the LRCalc to use for RegIdx.  In spill mode, the
  /// complement interval can overlap the other intervals, so it gets its own
  /// LRCalc instance.  When not in spill mode, all intervals can share one.
  LiveRangeCalc &getLRCalc(unsigned RegIdx) {
    return LRCalc[SpillMode != SM_Partition && RegIdx != 0];
  }

  /// defValue - define a value in RegIdx from ParentVNI at Idx.
  /// Idx does not have to be ParentVNI->def, but it must be contained within
  /// ParentVNI's live range in ParentLI. The new value is added to the value
  /// map.
  /// Return the new LI value.
  VNInfo *defValue(unsigned RegIdx, const VNInfo *ParentVNI, SlotIndex Idx);

  /// forceRecompute - Force the live range of ParentVNI in RegIdx to be
  /// recomputed by LiveRangeCalc::extend regardless of the number of defs.
  /// This is used for values whose live range doesn't match RegAssign exactly.
  /// They could have rematerialized, or back-copies may have been moved.
  void forceRecompute(unsigned RegIdx, const VNInfo *ParentVNI);

  /// defFromParent - Define Reg from ParentVNI at UseIdx using either
  /// rematerialization or a COPY from parent. Return the new value.
  VNInfo *defFromParent(unsigned RegIdx,
                        VNInfo *ParentVNI,
                        SlotIndex UseIdx,
                        MachineBasicBlock &MBB,
                        MachineBasicBlock::iterator I);

  /// removeBackCopies - Remove the copy instructions that defines the values
  /// in the vector in the complement interval.
  void removeBackCopies(SmallVectorImpl<VNInfo*> &Copies);

  /// getShallowDominator - Returns the least busy dominator of MBB that is
  /// also dominated by DefMBB.  Busy is measured by loop depth.
  MachineBasicBlock *findShallowDominator(MachineBasicBlock *MBB,
                                          MachineBasicBlock *DefMBB);

  /// hoistCopiesForSize - Hoist back-copies to the complement interval in a
  /// way that minimizes code size. This implements the SM_Size spill mode.
  void hoistCopiesForSize();

  /// transferValues - Transfer values to the new ranges.
  /// Return true if any ranges were skipped.
  bool transferValues();

  /// extendPHIKillRanges - Extend the ranges of all values killed by original
  /// parent PHIDefs.
  void extendPHIKillRanges();

  /// rewriteAssigned - Rewrite all uses of Edit.getReg() to assigned registers.
  void rewriteAssigned(bool ExtendRanges);

  /// deleteRematVictims - Delete defs that are dead after rematerializing.
  void deleteRematVictims();

public:
  /// Create a new SplitEditor for editing the LiveInterval analyzed by SA.
  /// Newly created intervals will be appended to newIntervals.
  SplitEditor(SplitAnalysis &SA, LiveIntervals&, VirtRegMap&,
              MachineDominatorTree&, MachineBlockFrequencyInfo &);

  /// reset - Prepare for a new split.
  void reset(LiveRangeEdit&, ComplementSpillMode = SM_Partition);

  /// Create a new virtual register and live interval.
  /// Return the interval index, starting from 1. Interval index 0 is the
  /// implicit complement interval.
  unsigned openIntv();

  /// currentIntv - Return the current interval index.
  unsigned currentIntv() const { return OpenIdx; }

  /// selectIntv - Select a previously opened interval index.
  void selectIntv(unsigned Idx);

  /// enterIntvBefore - Enter the open interval before the instruction at Idx.
  /// If the parent interval is not live before Idx, a COPY is not inserted.
  /// Return the beginning of the new live range.
  SlotIndex enterIntvBefore(SlotIndex Idx);

  /// enterIntvAfter - Enter the open interval after the instruction at Idx.
  /// Return the beginning of the new live range.
  SlotIndex enterIntvAfter(SlotIndex Idx);

  /// enterIntvAtEnd - Enter the open interval at the end of MBB.
  /// Use the open interval from the inserted copy to the MBB end.
  /// Return the beginning of the new live range.
  SlotIndex enterIntvAtEnd(MachineBasicBlock &MBB);

  /// useIntv - indicate that all instructions in MBB should use OpenLI.
  void useIntv(const MachineBasicBlock &MBB);

  /// useIntv - indicate that all instructions in range should use OpenLI.
  void useIntv(SlotIndex Start, SlotIndex End);

  /// leaveIntvAfter - Leave the open interval after the instruction at Idx.
  /// Return the end of the live range.
  SlotIndex leaveIntvAfter(SlotIndex Idx);

  /// leaveIntvBefore - Leave the open interval before the instruction at Idx.
  /// Return the end of the live range.
  SlotIndex leaveIntvBefore(SlotIndex Idx);

  /// leaveIntvAtTop - Leave the interval at the top of MBB.
  /// Add liveness from the MBB top to the copy.
  /// Return the end of the live range.
  SlotIndex leaveIntvAtTop(MachineBasicBlock &MBB);

  /// overlapIntv - Indicate that all instructions in range should use the open
  /// interval, but also let the complement interval be live.
  ///
  /// This doubles the register pressure, but is sometimes required to deal with
  /// register uses after the last valid split point.
  ///
  /// The Start index should be a return value from a leaveIntv* call, and End
  /// should be in the same basic block. The parent interval must have the same
  /// value across the range.
  ///
  void overlapIntv(SlotIndex Start, SlotIndex End);

  /// finish - after all the new live ranges have been created, compute the
  /// remaining live range, and rewrite instructions to use the new registers.
  /// @param LRMap When not null, this vector will map each live range in Edit
  ///              back to the indices returned by openIntv.
  ///              There may be extra indices created by dead code elimination.
  void finish(SmallVectorImpl<unsigned> *LRMap = nullptr);

  /// dump - print the current interval mapping to dbgs().
  void dump() const;

  // ===--- High level methods ---===

  /// splitSingleBlock - Split CurLI into a separate live interval around the
  /// uses in a single block. This is intended to be used as part of a larger
  /// split, and doesn't call finish().
  void splitSingleBlock(const SplitAnalysis::BlockInfo &BI);

  /// splitLiveThroughBlock - Split CurLI in the given block such that it
  /// enters the block in IntvIn and leaves it in IntvOut. There may be uses in
  /// the block, but they will be ignored when placing split points.
  ///
  /// @param MBBNum      Block number.
  /// @param IntvIn      Interval index entering the block.
  /// @param LeaveBefore When set, leave IntvIn before this point.
  /// @param IntvOut     Interval index leaving the block.
  /// @param EnterAfter  When set, enter IntvOut after this point.
  void splitLiveThroughBlock(unsigned MBBNum,
                             unsigned IntvIn, SlotIndex LeaveBefore,
                             unsigned IntvOut, SlotIndex EnterAfter);

  /// splitRegInBlock - Split CurLI in the given block such that it enters the
  /// block in IntvIn and leaves it on the stack (or not at all). Split points
  /// are placed in a way that avoids putting uses in the stack interval. This
  /// may require creating a local interval when there is interference.
  ///
  /// @param BI          Block descriptor.
  /// @param IntvIn      Interval index entering the block. Not 0.
  /// @param LeaveBefore When set, leave IntvIn before this point.
  void splitRegInBlock(const SplitAnalysis::BlockInfo &BI,
                       unsigned IntvIn, SlotIndex LeaveBefore);

  /// splitRegOutBlock - Split CurLI in the given block such that it enters the
  /// block on the stack (or isn't live-in at all) and leaves it in IntvOut.
  /// Split points are placed to avoid interference and such that the uses are
  /// not in the stack interval. This may require creating a local interval
  /// when there is interference.
  ///
  /// @param BI          Block descriptor.
  /// @param IntvOut     Interval index leaving the block.
  /// @param EnterAfter  When set, enter IntvOut after this point.
  void splitRegOutBlock(const SplitAnalysis::BlockInfo &BI,
                        unsigned IntvOut, SlotIndex EnterAfter);
};

}

#endif
