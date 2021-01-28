//=== MC/MCRegisterInfo.cpp - Target Register Description -------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements MCRegisterInfo functions.
//
//===----------------------------------------------------------------------===//

#include "llvm37/MC/MCRegisterInfo.h"

using namespace llvm37;

unsigned MCRegisterInfo::getMatchingSuperReg(unsigned Reg, unsigned SubIdx,
                                             const MCRegisterClass *RC) const {
  for (MCSuperRegIterator Supers(Reg, this); Supers.isValid(); ++Supers)
    if (RC->contains(*Supers) && Reg == getSubReg(*Supers, SubIdx))
      return *Supers;
  return 0;
}

unsigned MCRegisterInfo::getSubReg(unsigned Reg, unsigned Idx) const {
  assert(Idx && Idx < getNumSubRegIndices() &&
         "This is not a subregister index");
  // Get a pointer to the corresponding SubRegIndices list. This list has the
  // name of each sub-register in the same order as MCSubRegIterator.
  const uint16_t *SRI = SubRegIndices + get(Reg).SubRegIndices;
  for (MCSubRegIterator Subs(Reg, this); Subs.isValid(); ++Subs, ++SRI)
    if (*SRI == Idx)
      return *Subs;
  return 0;
}

unsigned MCRegisterInfo::getSubRegIndex(unsigned Reg, unsigned SubReg) const {
  assert(SubReg && SubReg < getNumRegs() && "This is not a register");
  // Get a pointer to the corresponding SubRegIndices list. This list has the
  // name of each sub-register in the same order as MCSubRegIterator.
  const uint16_t *SRI = SubRegIndices + get(Reg).SubRegIndices;
  for (MCSubRegIterator Subs(Reg, this); Subs.isValid(); ++Subs, ++SRI)
    if (*Subs == SubReg)
      return *SRI;
  return 0;
}

unsigned MCRegisterInfo::getSubRegIdxSize(unsigned Idx) const {
  assert(Idx && Idx < getNumSubRegIndices() &&
         "This is not a subregister index");
  return SubRegIdxRanges[Idx].Size;
}

unsigned MCRegisterInfo::getSubRegIdxOffset(unsigned Idx) const {
  assert(Idx && Idx < getNumSubRegIndices() &&
         "This is not a subregister index");
  return SubRegIdxRanges[Idx].Offset;
}

int MCRegisterInfo::getDwarfRegNum(unsigned RegNum, bool isEH) const {
  const DwarfLLVM37RegPair *M = isEH ? EHL2DwarfRegs : L2DwarfRegs;
  unsigned Size = isEH ? EHL2DwarfRegsSize : L2DwarfRegsSize;

  DwarfLLVM37RegPair Key = { RegNum, 0 };
  const DwarfLLVM37RegPair *I = std::lower_bound(M, M+Size, Key);
  if (I == M+Size || I->FromReg != RegNum)
    return -1;
  return I->ToReg;
}

int MCRegisterInfo::getLLVM37RegNum(unsigned RegNum, bool isEH) const {
  const DwarfLLVM37RegPair *M = isEH ? EHDwarf2LRegs : Dwarf2LRegs;
  unsigned Size = isEH ? EHDwarf2LRegsSize : Dwarf2LRegsSize;

  DwarfLLVM37RegPair Key = { RegNum, 0 };
  const DwarfLLVM37RegPair *I = std::lower_bound(M, M+Size, Key);
  assert(I != M+Size && I->FromReg == RegNum && "Invalid RegNum");
  return I->ToReg;
}

int MCRegisterInfo::getSEHRegNum(unsigned RegNum) const {
  const DenseMap<unsigned, int>::const_iterator I = L2SEHRegs.find(RegNum);
  if (I == L2SEHRegs.end()) return (int)RegNum;
  return I->second;
}
