///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilInstructions.h                                                        //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// This file provides a library of instruction helper classes.               //
// MUCH WORK YET TO BE DONE - EXPECT THIS WILL CHANGE - GENERATED FILE       //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "llvm37/IR/Constants.h"
#include "llvm37/IR/Instruction.h"
#include "llvm37/IR/Instructions.h"

// TODO: add correct include directives
// TODO: add accessors with values
// TODO: add validation support code, including calling into right fn
// TODO: add type hierarchy
namespace hlsl {
/* <py>
import hctdb_instrhelp
</py> */
/* <py::lines('INSTR-HELPER')>hctdb_instrhelp.get_instrhelper()</py>*/
// INSTR-HELPER:BEGIN
/// This instruction returns a value (possibly void), from a function.
struct LlvmInst_Ret {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_Ret(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::Ret;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction branches (conditional or unconditional)
struct LlvmInst_Br {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_Br(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::Br;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction performs a multiway switch
struct LlvmInst_Switch {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_Switch(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::Switch;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction branches indirectly
struct LlvmInst_IndirectBr {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_IndirectBr(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::IndirectBr;
  }
  // Validation support
  bool isAllowed() const { return false; }
};

/// This instruction invokes function with normal and exceptional returns
struct LlvmInst_Invoke {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_Invoke(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::Invoke;
  }
  // Validation support
  bool isAllowed() const { return false; }
};

/// This instruction resumes the propagation of an exception
struct LlvmInst_Resume {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_Resume(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::Resume;
  }
  // Validation support
  bool isAllowed() const { return false; }
};

/// This instruction is unreachable
struct LlvmInst_Unreachable {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_Unreachable(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::Unreachable;
  }
  // Validation support
  bool isAllowed() const { return false; }
};

/// This instruction returns the sum of its two operands
struct LlvmInst_Add {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_Add(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::Add;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction returns the sum of its two operands
struct LlvmInst_FAdd {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_FAdd(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::FAdd;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction returns the difference of its two operands
struct LlvmInst_Sub {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_Sub(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::Sub;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction returns the difference of its two operands
struct LlvmInst_FSub {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_FSub(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::FSub;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction returns the product of its two operands
struct LlvmInst_Mul {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_Mul(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::Mul;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction returns the product of its two operands
struct LlvmInst_FMul {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_FMul(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::FMul;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction returns the quotient of its two unsigned operands
struct LlvmInst_UDiv {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_UDiv(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::UDiv;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction returns the quotient of its two signed operands
struct LlvmInst_SDiv {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_SDiv(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::SDiv;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction returns the quotient of its two operands
struct LlvmInst_FDiv {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_FDiv(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::FDiv;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction returns the remainder from the unsigned division of its two operands
struct LlvmInst_URem {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_URem(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::URem;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction returns the remainder from the signed division of its two operands
struct LlvmInst_SRem {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_SRem(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::SRem;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction returns the remainder from the division of its two operands
struct LlvmInst_FRem {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_FRem(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::FRem;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction shifts left (logical)
struct LlvmInst_Shl {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_Shl(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::Shl;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction shifts right (logical), with zero bit fill
struct LlvmInst_LShr {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_LShr(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::LShr;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction shifts right (arithmetic), with 'a' operand sign bit fill
struct LlvmInst_AShr {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_AShr(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::AShr;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction returns a  bitwise logical and of its two operands
struct LlvmInst_And {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_And(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::And;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction returns a bitwise logical or of its two operands
struct LlvmInst_Or {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_Or(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::Or;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction returns a bitwise logical xor of its two operands
struct LlvmInst_Xor {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_Xor(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::Xor;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction allocates memory on the stack frame of the currently executing function
struct LlvmInst_Alloca {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_Alloca(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::Alloca;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction reads from memory
struct LlvmInst_Load {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_Load(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::Load;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction writes to memory
struct LlvmInst_Store {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_Store(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::Store;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction gets the address of a subelement of an aggregate value
struct LlvmInst_GetElementPtr {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_GetElementPtr(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::GetElementPtr;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction introduces happens-before edges between operations
struct LlvmInst_Fence {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_Fence(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::Fence;
  }
  // Validation support
  bool isAllowed() const { return false; }
};

/// This instruction atomically modifies memory
struct LlvmInst_AtomicCmpXchg {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_AtomicCmpXchg(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::AtomicCmpXchg;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction atomically modifies memory
struct LlvmInst_AtomicRMW {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_AtomicRMW(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::AtomicRMW;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction truncates an integer
struct LlvmInst_Trunc {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_Trunc(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::Trunc;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction zero extends an integer
struct LlvmInst_ZExt {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_ZExt(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::ZExt;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction sign extends an integer
struct LlvmInst_SExt {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_SExt(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::SExt;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction converts a floating point to UInt
struct LlvmInst_FPToUI {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_FPToUI(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::FPToUI;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction converts a floating point to SInt
struct LlvmInst_FPToSI {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_FPToSI(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::FPToSI;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction converts a UInt to floating point
struct LlvmInst_UIToFP {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_UIToFP(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::UIToFP;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction converts a SInt to floating point
struct LlvmInst_SIToFP {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_SIToFP(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::SIToFP;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction truncates a floating point
struct LlvmInst_FPTrunc {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_FPTrunc(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::FPTrunc;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction extends a floating point
struct LlvmInst_FPExt {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_FPExt(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::FPExt;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction converts a pointer to integer
struct LlvmInst_PtrToInt {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_PtrToInt(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::PtrToInt;
  }
  // Validation support
  bool isAllowed() const { return false; }
};

/// This instruction converts an integer to Pointer
struct LlvmInst_IntToPtr {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_IntToPtr(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::IntToPtr;
  }
  // Validation support
  bool isAllowed() const { return false; }
};

/// This instruction performs a bit-preserving type cast
struct LlvmInst_BitCast {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_BitCast(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::BitCast;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction casts a value addrspace
struct LlvmInst_AddrSpaceCast {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_AddrSpaceCast(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::AddrSpaceCast;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction compares integers
struct LlvmInst_ICmp {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_ICmp(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::ICmp;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction compares floating points
struct LlvmInst_FCmp {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_FCmp(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::FCmp;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction is a PHI node instruction
struct LlvmInst_PHI {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_PHI(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::PHI;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction calls a function
struct LlvmInst_Call {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_Call(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::Call;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction selects an instruction
struct LlvmInst_Select {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_Select(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::Select;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction may be used internally in a pass
struct LlvmInst_UserOp1 {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_UserOp1(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::UserOp1;
  }
  // Validation support
  bool isAllowed() const { return false; }
};

/// This instruction internal to passes only
struct LlvmInst_UserOp2 {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_UserOp2(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::UserOp2;
  }
  // Validation support
  bool isAllowed() const { return false; }
};

/// This instruction vaarg instruction
struct LlvmInst_VAArg {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_VAArg(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::VAArg;
  }
  // Validation support
  bool isAllowed() const { return false; }
};

/// This instruction extracts from aggregate
struct LlvmInst_ExtractValue {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_ExtractValue(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::ExtractValue;
  }
  // Validation support
  bool isAllowed() const { return true; }
};

/// This instruction represents a landing pad
struct LlvmInst_LandingPad {
  llvm37::Instruction *Instr;
  // Construction and identification
  LlvmInst_LandingPad(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return Instr->getOpcode() == llvm37::Instruction::LandingPad;
  }
  // Validation support
  bool isAllowed() const { return false; }
};

/// This instruction Helper load operation
struct DxilInst_TempRegLoad {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_TempRegLoad(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::TempRegLoad);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_index = 1,
  };
  // Accessors
  llvm37::Value *get_index() const { return Instr->getOperand(1); }
  void set_index(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction Helper store operation
struct DxilInst_TempRegStore {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_TempRegStore(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::TempRegStore);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_index = 1,
    arg_value = 2,
  };
  // Accessors
  llvm37::Value *get_index() const { return Instr->getOperand(1); }
  void set_index(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_value() const { return Instr->getOperand(2); }
  void set_value(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction Helper load operation for minprecision
struct DxilInst_MinPrecXRegLoad {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_MinPrecXRegLoad(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::MinPrecXRegLoad);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_regIndex = 1,
    arg_index = 2,
    arg_component = 3,
  };
  // Accessors
  llvm37::Value *get_regIndex() const { return Instr->getOperand(1); }
  void set_regIndex(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_index() const { return Instr->getOperand(2); }
  void set_index(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_component() const { return Instr->getOperand(3); }
  void set_component(llvm37::Value *val) { Instr->setOperand(3, val); }
};

/// This instruction Helper store operation for minprecision
struct DxilInst_MinPrecXRegStore {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_MinPrecXRegStore(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::MinPrecXRegStore);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (5 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_regIndex = 1,
    arg_index = 2,
    arg_component = 3,
    arg_value = 4,
  };
  // Accessors
  llvm37::Value *get_regIndex() const { return Instr->getOperand(1); }
  void set_regIndex(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_index() const { return Instr->getOperand(2); }
  void set_index(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_component() const { return Instr->getOperand(3); }
  void set_component(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_value() const { return Instr->getOperand(4); }
  void set_value(llvm37::Value *val) { Instr->setOperand(4, val); }
};

/// This instruction Loads the value from shader input
struct DxilInst_LoadInput {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_LoadInput(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::LoadInput);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (5 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_inputSigId = 1,
    arg_rowIndex = 2,
    arg_colIndex = 3,
    arg_gsVertexAxis = 4,
  };
  // Accessors
  llvm37::Value *get_inputSigId() const { return Instr->getOperand(1); }
  void set_inputSigId(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_rowIndex() const { return Instr->getOperand(2); }
  void set_rowIndex(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_colIndex() const { return Instr->getOperand(3); }
  void set_colIndex(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_gsVertexAxis() const { return Instr->getOperand(4); }
  void set_gsVertexAxis(llvm37::Value *val) { Instr->setOperand(4, val); }
};

/// This instruction Stores the value to shader output
struct DxilInst_StoreOutput {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_StoreOutput(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::StoreOutput);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (5 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_outputSigId = 1,
    arg_rowIndex = 2,
    arg_colIndex = 3,
    arg_value = 4,
  };
  // Accessors
  llvm37::Value *get_outputSigId() const { return Instr->getOperand(1); }
  void set_outputSigId(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_rowIndex() const { return Instr->getOperand(2); }
  void set_rowIndex(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_colIndex() const { return Instr->getOperand(3); }
  void set_colIndex(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_value() const { return Instr->getOperand(4); }
  void set_value(llvm37::Value *val) { Instr->setOperand(4, val); }
};

/// This instruction returns the absolute value of the input value.
struct DxilInst_FAbs {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_FAbs(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::FAbs);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction clamps the result of a single or double precision floating point value to [0.0f...1.0f]
struct DxilInst_Saturate {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Saturate(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Saturate);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction Returns true if x is NAN or QNAN, false otherwise.
struct DxilInst_IsNaN {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_IsNaN(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::IsNaN);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction Returns true if x is +INF or -INF, false otherwise.
struct DxilInst_IsInf {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_IsInf(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::IsInf);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction Returns true if x is finite, false otherwise.
struct DxilInst_IsFinite {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_IsFinite(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::IsFinite);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns IsNormal
struct DxilInst_IsNormal {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_IsNormal(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::IsNormal);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns cosine(theta) for theta in radians.
struct DxilInst_Cos {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Cos(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Cos);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns sine(theta) for theta in radians.
struct DxilInst_Sin {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Sin(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Sin);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns tan(theta) for theta in radians.
struct DxilInst_Tan {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Tan(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Tan);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction Returns the arccosine of the specified value. Input should be a floating-point value within the range of -1 to 1.
struct DxilInst_Acos {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Acos(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Acos);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction Returns the arccosine of the specified value. Input should be a floating-point value within the range of -1 to 1
struct DxilInst_Asin {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Asin(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Asin);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction Returns the arctangent of the specified value. The return value is within the range of -PI/2 to PI/2.
struct DxilInst_Atan {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Atan(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Atan);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns the hyperbolic cosine of the specified value.
struct DxilInst_Hcos {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Hcos(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Hcos);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns the hyperbolic sine of the specified value.
struct DxilInst_Hsin {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Hsin(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Hsin);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns the hyperbolic tangent of the specified value.
struct DxilInst_Htan {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Htan(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Htan);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns 2^exponent
struct DxilInst_Exp {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Exp(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Exp);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction extract fracitonal component.
struct DxilInst_Frc {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Frc(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Frc);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns log base 2.
struct DxilInst_Log {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Log(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Log);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns square root
struct DxilInst_Sqrt {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Sqrt(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Sqrt);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns reciprocal square root (1 / sqrt(src)
struct DxilInst_Rsqrt {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Rsqrt(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Rsqrt);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction floating-point round to integral float.
struct DxilInst_Round_ne {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Round_ne(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Round_ne);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction floating-point round to integral float.
struct DxilInst_Round_ni {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Round_ni(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Round_ni);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction floating-point round to integral float.
struct DxilInst_Round_pi {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Round_pi(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Round_pi);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction floating-point round to integral float.
struct DxilInst_Round_z {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Round_z(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Round_z);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction Reverses the order of the bits.
struct DxilInst_Bfrev {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Bfrev(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Bfrev);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction Counts the number of bits in the input integer.
struct DxilInst_Countbits {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Countbits(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Countbits);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction Returns the location of the first set bit starting from the lowest order bit and working upward.
struct DxilInst_FirstbitLo {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_FirstbitLo(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::FirstbitLo);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction Returns the location of the first set bit starting from the highest order bit and working downward.
struct DxilInst_FirstbitHi {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_FirstbitHi(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::FirstbitHi);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction Returns the location of the first set bit from the highest order bit based on the sign.
struct DxilInst_FirstbitSHi {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_FirstbitSHi(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::FirstbitSHi);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns a if a >= b, else b
struct DxilInst_FMax {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_FMax(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::FMax);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_a = 1,
    arg_b = 2,
  };
  // Accessors
  llvm37::Value *get_a() const { return Instr->getOperand(1); }
  void set_a(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_b() const { return Instr->getOperand(2); }
  void set_b(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction returns a if a < b, else b
struct DxilInst_FMin {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_FMin(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::FMin);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_a = 1,
    arg_b = 2,
  };
  // Accessors
  llvm37::Value *get_a() const { return Instr->getOperand(1); }
  void set_a(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_b() const { return Instr->getOperand(2); }
  void set_b(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction IMax(a,b) returns a if a > b, else b
struct DxilInst_IMax {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_IMax(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::IMax);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_a = 1,
    arg_b = 2,
  };
  // Accessors
  llvm37::Value *get_a() const { return Instr->getOperand(1); }
  void set_a(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_b() const { return Instr->getOperand(2); }
  void set_b(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction IMin(a,b) returns a if a < b, else b
struct DxilInst_IMin {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_IMin(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::IMin);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_a = 1,
    arg_b = 2,
  };
  // Accessors
  llvm37::Value *get_a() const { return Instr->getOperand(1); }
  void set_a(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_b() const { return Instr->getOperand(2); }
  void set_b(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction unsigned integer maximum. UMax(a,b) = a > b ? a : b
struct DxilInst_UMax {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_UMax(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::UMax);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_a = 1,
    arg_b = 2,
  };
  // Accessors
  llvm37::Value *get_a() const { return Instr->getOperand(1); }
  void set_a(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_b() const { return Instr->getOperand(2); }
  void set_b(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction unsigned integer minimum. UMin(a,b) = a < b ? a : b
struct DxilInst_UMin {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_UMin(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::UMin);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_a = 1,
    arg_b = 2,
  };
  // Accessors
  llvm37::Value *get_a() const { return Instr->getOperand(1); }
  void set_a(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_b() const { return Instr->getOperand(2); }
  void set_b(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction multiply of 32-bit operands to produce the correct full 64-bit result.
struct DxilInst_IMul {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_IMul(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::IMul);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_a = 1,
    arg_b = 2,
  };
  // Accessors
  llvm37::Value *get_a() const { return Instr->getOperand(1); }
  void set_a(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_b() const { return Instr->getOperand(2); }
  void set_b(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction multiply of 32-bit operands to produce the correct full 64-bit result.
struct DxilInst_UMul {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_UMul(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::UMul);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_a = 1,
    arg_b = 2,
  };
  // Accessors
  llvm37::Value *get_a() const { return Instr->getOperand(1); }
  void set_a(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_b() const { return Instr->getOperand(2); }
  void set_b(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction unsigned divide of the 32-bit operand src0 by the 32-bit operand src1.
struct DxilInst_UDiv {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_UDiv(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::UDiv);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_a = 1,
    arg_b = 2,
  };
  // Accessors
  llvm37::Value *get_a() const { return Instr->getOperand(1); }
  void set_a(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_b() const { return Instr->getOperand(2); }
  void set_b(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction unsigned add of 32-bit operand with the carry
struct DxilInst_UAddc {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_UAddc(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::UAddc);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_a = 1,
    arg_b = 2,
  };
  // Accessors
  llvm37::Value *get_a() const { return Instr->getOperand(1); }
  void set_a(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_b() const { return Instr->getOperand(2); }
  void set_b(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction unsigned subtract of 32-bit operands with the borrow
struct DxilInst_USubb {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_USubb(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::USubb);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_a = 1,
    arg_b = 2,
  };
  // Accessors
  llvm37::Value *get_a() const { return Instr->getOperand(1); }
  void set_a(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_b() const { return Instr->getOperand(2); }
  void set_b(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction floating point multiply & add
struct DxilInst_FMad {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_FMad(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::FMad);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_a = 1,
    arg_b = 2,
    arg_c = 3,
  };
  // Accessors
  llvm37::Value *get_a() const { return Instr->getOperand(1); }
  void set_a(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_b() const { return Instr->getOperand(2); }
  void set_b(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_c() const { return Instr->getOperand(3); }
  void set_c(llvm37::Value *val) { Instr->setOperand(3, val); }
};

/// This instruction fused multiply-add
struct DxilInst_Fma {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Fma(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Fma);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_a = 1,
    arg_b = 2,
    arg_c = 3,
  };
  // Accessors
  llvm37::Value *get_a() const { return Instr->getOperand(1); }
  void set_a(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_b() const { return Instr->getOperand(2); }
  void set_b(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_c() const { return Instr->getOperand(3); }
  void set_c(llvm37::Value *val) { Instr->setOperand(3, val); }
};

/// This instruction Signed integer multiply & add
struct DxilInst_IMad {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_IMad(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::IMad);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_a = 1,
    arg_b = 2,
    arg_c = 3,
  };
  // Accessors
  llvm37::Value *get_a() const { return Instr->getOperand(1); }
  void set_a(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_b() const { return Instr->getOperand(2); }
  void set_b(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_c() const { return Instr->getOperand(3); }
  void set_c(llvm37::Value *val) { Instr->setOperand(3, val); }
};

/// This instruction Unsigned integer multiply & add
struct DxilInst_UMad {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_UMad(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::UMad);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_a = 1,
    arg_b = 2,
    arg_c = 3,
  };
  // Accessors
  llvm37::Value *get_a() const { return Instr->getOperand(1); }
  void set_a(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_b() const { return Instr->getOperand(2); }
  void set_b(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_c() const { return Instr->getOperand(3); }
  void set_c(llvm37::Value *val) { Instr->setOperand(3, val); }
};

/// This instruction masked Sum of Absolute Differences.
struct DxilInst_Msad {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Msad(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Msad);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_a = 1,
    arg_b = 2,
    arg_c = 3,
  };
  // Accessors
  llvm37::Value *get_a() const { return Instr->getOperand(1); }
  void set_a(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_b() const { return Instr->getOperand(2); }
  void set_b(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_c() const { return Instr->getOperand(3); }
  void set_c(llvm37::Value *val) { Instr->setOperand(3, val); }
};

/// This instruction Integer bitfield extract
struct DxilInst_Ibfe {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Ibfe(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Ibfe);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_a = 1,
    arg_b = 2,
    arg_c = 3,
  };
  // Accessors
  llvm37::Value *get_a() const { return Instr->getOperand(1); }
  void set_a(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_b() const { return Instr->getOperand(2); }
  void set_b(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_c() const { return Instr->getOperand(3); }
  void set_c(llvm37::Value *val) { Instr->setOperand(3, val); }
};

/// This instruction Unsigned integer bitfield extract
struct DxilInst_Ubfe {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Ubfe(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Ubfe);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_a = 1,
    arg_b = 2,
    arg_c = 3,
  };
  // Accessors
  llvm37::Value *get_a() const { return Instr->getOperand(1); }
  void set_a(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_b() const { return Instr->getOperand(2); }
  void set_b(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_c() const { return Instr->getOperand(3); }
  void set_c(llvm37::Value *val) { Instr->setOperand(3, val); }
};

/// This instruction Given a bit range from the LSB of a number, places that number of bits in another number at any offset
struct DxilInst_Bfi {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Bfi(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Bfi);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (5 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_width = 1,
    arg_offset = 2,
    arg_value = 3,
    arg_replacedValue = 4,
  };
  // Accessors
  llvm37::Value *get_width() const { return Instr->getOperand(1); }
  void set_width(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_offset() const { return Instr->getOperand(2); }
  void set_offset(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_value() const { return Instr->getOperand(3); }
  void set_value(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_replacedValue() const { return Instr->getOperand(4); }
  void set_replacedValue(llvm37::Value *val) { Instr->setOperand(4, val); }
};

/// This instruction Two-dimensional vector dot-product
struct DxilInst_Dot2 {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Dot2(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Dot2);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (5 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_ax = 1,
    arg_ay = 2,
    arg_bx = 3,
    arg_by = 4,
  };
  // Accessors
  llvm37::Value *get_ax() const { return Instr->getOperand(1); }
  void set_ax(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_ay() const { return Instr->getOperand(2); }
  void set_ay(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_bx() const { return Instr->getOperand(3); }
  void set_bx(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_by() const { return Instr->getOperand(4); }
  void set_by(llvm37::Value *val) { Instr->setOperand(4, val); }
};

/// This instruction Three-dimensional vector dot-product
struct DxilInst_Dot3 {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Dot3(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Dot3);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (7 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_ax = 1,
    arg_ay = 2,
    arg_az = 3,
    arg_bx = 4,
    arg_by = 5,
    arg_bz = 6,
  };
  // Accessors
  llvm37::Value *get_ax() const { return Instr->getOperand(1); }
  void set_ax(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_ay() const { return Instr->getOperand(2); }
  void set_ay(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_az() const { return Instr->getOperand(3); }
  void set_az(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_bx() const { return Instr->getOperand(4); }
  void set_bx(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_by() const { return Instr->getOperand(5); }
  void set_by(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_bz() const { return Instr->getOperand(6); }
  void set_bz(llvm37::Value *val) { Instr->setOperand(6, val); }
};

/// This instruction Four-dimensional vector dot-product
struct DxilInst_Dot4 {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Dot4(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Dot4);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (9 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_ax = 1,
    arg_ay = 2,
    arg_az = 3,
    arg_aw = 4,
    arg_bx = 5,
    arg_by = 6,
    arg_bz = 7,
    arg_bw = 8,
  };
  // Accessors
  llvm37::Value *get_ax() const { return Instr->getOperand(1); }
  void set_ax(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_ay() const { return Instr->getOperand(2); }
  void set_ay(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_az() const { return Instr->getOperand(3); }
  void set_az(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_aw() const { return Instr->getOperand(4); }
  void set_aw(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_bx() const { return Instr->getOperand(5); }
  void set_bx(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_by() const { return Instr->getOperand(6); }
  void set_by(llvm37::Value *val) { Instr->setOperand(6, val); }
  llvm37::Value *get_bz() const { return Instr->getOperand(7); }
  void set_bz(llvm37::Value *val) { Instr->setOperand(7, val); }
  llvm37::Value *get_bw() const { return Instr->getOperand(8); }
  void set_bw(llvm37::Value *val) { Instr->setOperand(8, val); }
};

/// This instruction creates the handle to a resource
struct DxilInst_CreateHandle {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_CreateHandle(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::CreateHandle);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (5 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_resourceClass = 1,
    arg_rangeId = 2,
    arg_index = 3,
    arg_nonUniformIndex = 4,
  };
  // Accessors
  llvm37::Value *get_resourceClass() const { return Instr->getOperand(1); }
  void set_resourceClass(llvm37::Value *val) { Instr->setOperand(1, val); }
  int8_t get_resourceClass_val() const { return (int8_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(1))->getZExtValue()); }
  void set_resourceClass_val(int8_t val) { Instr->setOperand(1, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 8), llvm37::APInt(8, (uint64_t)val))); }
  llvm37::Value *get_rangeId() const { return Instr->getOperand(2); }
  void set_rangeId(llvm37::Value *val) { Instr->setOperand(2, val); }
  int32_t get_rangeId_val() const { return (int32_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(2))->getZExtValue()); }
  void set_rangeId_val(int32_t val) { Instr->setOperand(2, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 32), llvm37::APInt(32, (uint64_t)val))); }
  llvm37::Value *get_index() const { return Instr->getOperand(3); }
  void set_index(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_nonUniformIndex() const { return Instr->getOperand(4); }
  void set_nonUniformIndex(llvm37::Value *val) { Instr->setOperand(4, val); }
  bool get_nonUniformIndex_val() const { return (bool)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(4))->getZExtValue()); }
  void set_nonUniformIndex_val(bool val) { Instr->setOperand(4, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 1), llvm37::APInt(1, (uint64_t)val))); }
};

/// This instruction loads a value from a constant buffer resource
struct DxilInst_CBufferLoad {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_CBufferLoad(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::CBufferLoad);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_handle = 1,
    arg_byteOffset = 2,
    arg_alignment = 3,
  };
  // Accessors
  llvm37::Value *get_handle() const { return Instr->getOperand(1); }
  void set_handle(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_byteOffset() const { return Instr->getOperand(2); }
  void set_byteOffset(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_alignment() const { return Instr->getOperand(3); }
  void set_alignment(llvm37::Value *val) { Instr->setOperand(3, val); }
  uint32_t get_alignment_val() const { return (uint32_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(3))->getZExtValue()); }
  void set_alignment_val(uint32_t val) { Instr->setOperand(3, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 32), llvm37::APInt(32, (uint64_t)val))); }
};

/// This instruction loads a value from a constant buffer resource
struct DxilInst_CBufferLoadLegacy {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_CBufferLoadLegacy(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::CBufferLoadLegacy);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_handle = 1,
    arg_regIndex = 2,
  };
  // Accessors
  llvm37::Value *get_handle() const { return Instr->getOperand(1); }
  void set_handle(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_regIndex() const { return Instr->getOperand(2); }
  void set_regIndex(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction samples a texture
struct DxilInst_Sample {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Sample(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Sample);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (11 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_srv = 1,
    arg_sampler = 2,
    arg_coord0 = 3,
    arg_coord1 = 4,
    arg_coord2 = 5,
    arg_coord3 = 6,
    arg_offset0 = 7,
    arg_offset1 = 8,
    arg_offset2 = 9,
    arg_clamp = 10,
  };
  // Accessors
  llvm37::Value *get_srv() const { return Instr->getOperand(1); }
  void set_srv(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_sampler() const { return Instr->getOperand(2); }
  void set_sampler(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_coord0() const { return Instr->getOperand(3); }
  void set_coord0(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_coord1() const { return Instr->getOperand(4); }
  void set_coord1(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_coord2() const { return Instr->getOperand(5); }
  void set_coord2(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_coord3() const { return Instr->getOperand(6); }
  void set_coord3(llvm37::Value *val) { Instr->setOperand(6, val); }
  llvm37::Value *get_offset0() const { return Instr->getOperand(7); }
  void set_offset0(llvm37::Value *val) { Instr->setOperand(7, val); }
  llvm37::Value *get_offset1() const { return Instr->getOperand(8); }
  void set_offset1(llvm37::Value *val) { Instr->setOperand(8, val); }
  llvm37::Value *get_offset2() const { return Instr->getOperand(9); }
  void set_offset2(llvm37::Value *val) { Instr->setOperand(9, val); }
  llvm37::Value *get_clamp() const { return Instr->getOperand(10); }
  void set_clamp(llvm37::Value *val) { Instr->setOperand(10, val); }
};

/// This instruction samples a texture after applying the input bias to the mipmap level
struct DxilInst_SampleBias {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_SampleBias(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::SampleBias);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (12 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_srv = 1,
    arg_sampler = 2,
    arg_coord0 = 3,
    arg_coord1 = 4,
    arg_coord2 = 5,
    arg_coord3 = 6,
    arg_offset0 = 7,
    arg_offset1 = 8,
    arg_offset2 = 9,
    arg_bias = 10,
    arg_clamp = 11,
  };
  // Accessors
  llvm37::Value *get_srv() const { return Instr->getOperand(1); }
  void set_srv(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_sampler() const { return Instr->getOperand(2); }
  void set_sampler(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_coord0() const { return Instr->getOperand(3); }
  void set_coord0(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_coord1() const { return Instr->getOperand(4); }
  void set_coord1(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_coord2() const { return Instr->getOperand(5); }
  void set_coord2(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_coord3() const { return Instr->getOperand(6); }
  void set_coord3(llvm37::Value *val) { Instr->setOperand(6, val); }
  llvm37::Value *get_offset0() const { return Instr->getOperand(7); }
  void set_offset0(llvm37::Value *val) { Instr->setOperand(7, val); }
  llvm37::Value *get_offset1() const { return Instr->getOperand(8); }
  void set_offset1(llvm37::Value *val) { Instr->setOperand(8, val); }
  llvm37::Value *get_offset2() const { return Instr->getOperand(9); }
  void set_offset2(llvm37::Value *val) { Instr->setOperand(9, val); }
  llvm37::Value *get_bias() const { return Instr->getOperand(10); }
  void set_bias(llvm37::Value *val) { Instr->setOperand(10, val); }
  llvm37::Value *get_clamp() const { return Instr->getOperand(11); }
  void set_clamp(llvm37::Value *val) { Instr->setOperand(11, val); }
};

/// This instruction samples a texture using a mipmap-level offset
struct DxilInst_SampleLevel {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_SampleLevel(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::SampleLevel);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (11 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_srv = 1,
    arg_sampler = 2,
    arg_coord0 = 3,
    arg_coord1 = 4,
    arg_coord2 = 5,
    arg_coord3 = 6,
    arg_offset0 = 7,
    arg_offset1 = 8,
    arg_offset2 = 9,
    arg_LOD = 10,
  };
  // Accessors
  llvm37::Value *get_srv() const { return Instr->getOperand(1); }
  void set_srv(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_sampler() const { return Instr->getOperand(2); }
  void set_sampler(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_coord0() const { return Instr->getOperand(3); }
  void set_coord0(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_coord1() const { return Instr->getOperand(4); }
  void set_coord1(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_coord2() const { return Instr->getOperand(5); }
  void set_coord2(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_coord3() const { return Instr->getOperand(6); }
  void set_coord3(llvm37::Value *val) { Instr->setOperand(6, val); }
  llvm37::Value *get_offset0() const { return Instr->getOperand(7); }
  void set_offset0(llvm37::Value *val) { Instr->setOperand(7, val); }
  llvm37::Value *get_offset1() const { return Instr->getOperand(8); }
  void set_offset1(llvm37::Value *val) { Instr->setOperand(8, val); }
  llvm37::Value *get_offset2() const { return Instr->getOperand(9); }
  void set_offset2(llvm37::Value *val) { Instr->setOperand(9, val); }
  llvm37::Value *get_LOD() const { return Instr->getOperand(10); }
  void set_LOD(llvm37::Value *val) { Instr->setOperand(10, val); }
};

/// This instruction samples a texture using a gradient to influence the way the sample location is calculated
struct DxilInst_SampleGrad {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_SampleGrad(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::SampleGrad);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (17 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_srv = 1,
    arg_sampler = 2,
    arg_coord0 = 3,
    arg_coord1 = 4,
    arg_coord2 = 5,
    arg_coord3 = 6,
    arg_offset0 = 7,
    arg_offset1 = 8,
    arg_offset2 = 9,
    arg_ddx0 = 10,
    arg_ddx1 = 11,
    arg_ddx2 = 12,
    arg_ddy0 = 13,
    arg_ddy1 = 14,
    arg_ddy2 = 15,
    arg_clamp = 16,
  };
  // Accessors
  llvm37::Value *get_srv() const { return Instr->getOperand(1); }
  void set_srv(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_sampler() const { return Instr->getOperand(2); }
  void set_sampler(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_coord0() const { return Instr->getOperand(3); }
  void set_coord0(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_coord1() const { return Instr->getOperand(4); }
  void set_coord1(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_coord2() const { return Instr->getOperand(5); }
  void set_coord2(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_coord3() const { return Instr->getOperand(6); }
  void set_coord3(llvm37::Value *val) { Instr->setOperand(6, val); }
  llvm37::Value *get_offset0() const { return Instr->getOperand(7); }
  void set_offset0(llvm37::Value *val) { Instr->setOperand(7, val); }
  llvm37::Value *get_offset1() const { return Instr->getOperand(8); }
  void set_offset1(llvm37::Value *val) { Instr->setOperand(8, val); }
  llvm37::Value *get_offset2() const { return Instr->getOperand(9); }
  void set_offset2(llvm37::Value *val) { Instr->setOperand(9, val); }
  llvm37::Value *get_ddx0() const { return Instr->getOperand(10); }
  void set_ddx0(llvm37::Value *val) { Instr->setOperand(10, val); }
  llvm37::Value *get_ddx1() const { return Instr->getOperand(11); }
  void set_ddx1(llvm37::Value *val) { Instr->setOperand(11, val); }
  llvm37::Value *get_ddx2() const { return Instr->getOperand(12); }
  void set_ddx2(llvm37::Value *val) { Instr->setOperand(12, val); }
  llvm37::Value *get_ddy0() const { return Instr->getOperand(13); }
  void set_ddy0(llvm37::Value *val) { Instr->setOperand(13, val); }
  llvm37::Value *get_ddy1() const { return Instr->getOperand(14); }
  void set_ddy1(llvm37::Value *val) { Instr->setOperand(14, val); }
  llvm37::Value *get_ddy2() const { return Instr->getOperand(15); }
  void set_ddy2(llvm37::Value *val) { Instr->setOperand(15, val); }
  llvm37::Value *get_clamp() const { return Instr->getOperand(16); }
  void set_clamp(llvm37::Value *val) { Instr->setOperand(16, val); }
};

/// This instruction samples a texture and compares a single component against the specified comparison value
struct DxilInst_SampleCmp {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_SampleCmp(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::SampleCmp);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (12 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_srv = 1,
    arg_sampler = 2,
    arg_coord0 = 3,
    arg_coord1 = 4,
    arg_coord2 = 5,
    arg_coord3 = 6,
    arg_offset0 = 7,
    arg_offset1 = 8,
    arg_offset2 = 9,
    arg_compareValue = 10,
    arg_clamp = 11,
  };
  // Accessors
  llvm37::Value *get_srv() const { return Instr->getOperand(1); }
  void set_srv(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_sampler() const { return Instr->getOperand(2); }
  void set_sampler(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_coord0() const { return Instr->getOperand(3); }
  void set_coord0(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_coord1() const { return Instr->getOperand(4); }
  void set_coord1(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_coord2() const { return Instr->getOperand(5); }
  void set_coord2(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_coord3() const { return Instr->getOperand(6); }
  void set_coord3(llvm37::Value *val) { Instr->setOperand(6, val); }
  llvm37::Value *get_offset0() const { return Instr->getOperand(7); }
  void set_offset0(llvm37::Value *val) { Instr->setOperand(7, val); }
  llvm37::Value *get_offset1() const { return Instr->getOperand(8); }
  void set_offset1(llvm37::Value *val) { Instr->setOperand(8, val); }
  llvm37::Value *get_offset2() const { return Instr->getOperand(9); }
  void set_offset2(llvm37::Value *val) { Instr->setOperand(9, val); }
  llvm37::Value *get_compareValue() const { return Instr->getOperand(10); }
  void set_compareValue(llvm37::Value *val) { Instr->setOperand(10, val); }
  llvm37::Value *get_clamp() const { return Instr->getOperand(11); }
  void set_clamp(llvm37::Value *val) { Instr->setOperand(11, val); }
};

/// This instruction samples a texture and compares a single component against the specified comparison value
struct DxilInst_SampleCmpLevelZero {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_SampleCmpLevelZero(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::SampleCmpLevelZero);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (11 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_srv = 1,
    arg_sampler = 2,
    arg_coord0 = 3,
    arg_coord1 = 4,
    arg_coord2 = 5,
    arg_coord3 = 6,
    arg_offset0 = 7,
    arg_offset1 = 8,
    arg_offset2 = 9,
    arg_compareValue = 10,
  };
  // Accessors
  llvm37::Value *get_srv() const { return Instr->getOperand(1); }
  void set_srv(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_sampler() const { return Instr->getOperand(2); }
  void set_sampler(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_coord0() const { return Instr->getOperand(3); }
  void set_coord0(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_coord1() const { return Instr->getOperand(4); }
  void set_coord1(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_coord2() const { return Instr->getOperand(5); }
  void set_coord2(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_coord3() const { return Instr->getOperand(6); }
  void set_coord3(llvm37::Value *val) { Instr->setOperand(6, val); }
  llvm37::Value *get_offset0() const { return Instr->getOperand(7); }
  void set_offset0(llvm37::Value *val) { Instr->setOperand(7, val); }
  llvm37::Value *get_offset1() const { return Instr->getOperand(8); }
  void set_offset1(llvm37::Value *val) { Instr->setOperand(8, val); }
  llvm37::Value *get_offset2() const { return Instr->getOperand(9); }
  void set_offset2(llvm37::Value *val) { Instr->setOperand(9, val); }
  llvm37::Value *get_compareValue() const { return Instr->getOperand(10); }
  void set_compareValue(llvm37::Value *val) { Instr->setOperand(10, val); }
};

/// This instruction reads texel data without any filtering or sampling
struct DxilInst_TextureLoad {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_TextureLoad(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::TextureLoad);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (9 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_srv = 1,
    arg_mipLevelOrSampleCount = 2,
    arg_coord0 = 3,
    arg_coord1 = 4,
    arg_coord2 = 5,
    arg_offset0 = 6,
    arg_offset1 = 7,
    arg_offset2 = 8,
  };
  // Accessors
  llvm37::Value *get_srv() const { return Instr->getOperand(1); }
  void set_srv(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_mipLevelOrSampleCount() const { return Instr->getOperand(2); }
  void set_mipLevelOrSampleCount(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_coord0() const { return Instr->getOperand(3); }
  void set_coord0(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_coord1() const { return Instr->getOperand(4); }
  void set_coord1(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_coord2() const { return Instr->getOperand(5); }
  void set_coord2(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_offset0() const { return Instr->getOperand(6); }
  void set_offset0(llvm37::Value *val) { Instr->setOperand(6, val); }
  llvm37::Value *get_offset1() const { return Instr->getOperand(7); }
  void set_offset1(llvm37::Value *val) { Instr->setOperand(7, val); }
  llvm37::Value *get_offset2() const { return Instr->getOperand(8); }
  void set_offset2(llvm37::Value *val) { Instr->setOperand(8, val); }
};

/// This instruction reads texel data without any filtering or sampling
struct DxilInst_TextureStore {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_TextureStore(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::TextureStore);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (10 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_srv = 1,
    arg_coord0 = 2,
    arg_coord1 = 3,
    arg_coord2 = 4,
    arg_value0 = 5,
    arg_value1 = 6,
    arg_value2 = 7,
    arg_value3 = 8,
    arg_mask = 9,
  };
  // Accessors
  llvm37::Value *get_srv() const { return Instr->getOperand(1); }
  void set_srv(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_coord0() const { return Instr->getOperand(2); }
  void set_coord0(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_coord1() const { return Instr->getOperand(3); }
  void set_coord1(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_coord2() const { return Instr->getOperand(4); }
  void set_coord2(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_value0() const { return Instr->getOperand(5); }
  void set_value0(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_value1() const { return Instr->getOperand(6); }
  void set_value1(llvm37::Value *val) { Instr->setOperand(6, val); }
  llvm37::Value *get_value2() const { return Instr->getOperand(7); }
  void set_value2(llvm37::Value *val) { Instr->setOperand(7, val); }
  llvm37::Value *get_value3() const { return Instr->getOperand(8); }
  void set_value3(llvm37::Value *val) { Instr->setOperand(8, val); }
  llvm37::Value *get_mask() const { return Instr->getOperand(9); }
  void set_mask(llvm37::Value *val) { Instr->setOperand(9, val); }
};

/// This instruction reads from a TypedBuffer
struct DxilInst_BufferLoad {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_BufferLoad(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::BufferLoad);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_srv = 1,
    arg_index = 2,
    arg_wot = 3,
  };
  // Accessors
  llvm37::Value *get_srv() const { return Instr->getOperand(1); }
  void set_srv(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_index() const { return Instr->getOperand(2); }
  void set_index(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_wot() const { return Instr->getOperand(3); }
  void set_wot(llvm37::Value *val) { Instr->setOperand(3, val); }
};

/// This instruction writes to a RWTypedBuffer
struct DxilInst_BufferStore {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_BufferStore(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::BufferStore);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (9 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_uav = 1,
    arg_coord0 = 2,
    arg_coord1 = 3,
    arg_value0 = 4,
    arg_value1 = 5,
    arg_value2 = 6,
    arg_value3 = 7,
    arg_mask = 8,
  };
  // Accessors
  llvm37::Value *get_uav() const { return Instr->getOperand(1); }
  void set_uav(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_coord0() const { return Instr->getOperand(2); }
  void set_coord0(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_coord1() const { return Instr->getOperand(3); }
  void set_coord1(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_value0() const { return Instr->getOperand(4); }
  void set_value0(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_value1() const { return Instr->getOperand(5); }
  void set_value1(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_value2() const { return Instr->getOperand(6); }
  void set_value2(llvm37::Value *val) { Instr->setOperand(6, val); }
  llvm37::Value *get_value3() const { return Instr->getOperand(7); }
  void set_value3(llvm37::Value *val) { Instr->setOperand(7, val); }
  llvm37::Value *get_mask() const { return Instr->getOperand(8); }
  void set_mask(llvm37::Value *val) { Instr->setOperand(8, val); }
};

/// This instruction atomically increments/decrements the hidden 32-bit counter stored with a Count or Append UAV
struct DxilInst_BufferUpdateCounter {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_BufferUpdateCounter(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::BufferUpdateCounter);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_uav = 1,
    arg_inc = 2,
  };
  // Accessors
  llvm37::Value *get_uav() const { return Instr->getOperand(1); }
  void set_uav(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_inc() const { return Instr->getOperand(2); }
  void set_inc(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction determines whether all values from a Sample, Gather, or Load operation accessed mapped tiles in a tiled resource
struct DxilInst_CheckAccessFullyMapped {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_CheckAccessFullyMapped(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::CheckAccessFullyMapped);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_status = 1,
  };
  // Accessors
  llvm37::Value *get_status() const { return Instr->getOperand(1); }
  void set_status(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction gets texture size information
struct DxilInst_GetDimensions {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_GetDimensions(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::GetDimensions);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_handle = 1,
    arg_mipLevel = 2,
  };
  // Accessors
  llvm37::Value *get_handle() const { return Instr->getOperand(1); }
  void set_handle(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_mipLevel() const { return Instr->getOperand(2); }
  void set_mipLevel(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction gathers the four texels that would be used in a bi-linear filtering operation
struct DxilInst_TextureGather {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_TextureGather(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::TextureGather);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (10 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_srv = 1,
    arg_sampler = 2,
    arg_coord0 = 3,
    arg_coord1 = 4,
    arg_coord2 = 5,
    arg_coord3 = 6,
    arg_offset0 = 7,
    arg_offset1 = 8,
    arg_channel = 9,
  };
  // Accessors
  llvm37::Value *get_srv() const { return Instr->getOperand(1); }
  void set_srv(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_sampler() const { return Instr->getOperand(2); }
  void set_sampler(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_coord0() const { return Instr->getOperand(3); }
  void set_coord0(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_coord1() const { return Instr->getOperand(4); }
  void set_coord1(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_coord2() const { return Instr->getOperand(5); }
  void set_coord2(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_coord3() const { return Instr->getOperand(6); }
  void set_coord3(llvm37::Value *val) { Instr->setOperand(6, val); }
  llvm37::Value *get_offset0() const { return Instr->getOperand(7); }
  void set_offset0(llvm37::Value *val) { Instr->setOperand(7, val); }
  llvm37::Value *get_offset1() const { return Instr->getOperand(8); }
  void set_offset1(llvm37::Value *val) { Instr->setOperand(8, val); }
  llvm37::Value *get_channel() const { return Instr->getOperand(9); }
  void set_channel(llvm37::Value *val) { Instr->setOperand(9, val); }
};

/// This instruction same as TextureGather, except this instrution performs comparison on texels, similar to SampleCmp
struct DxilInst_TextureGatherCmp {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_TextureGatherCmp(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::TextureGatherCmp);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (11 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_srv = 1,
    arg_sampler = 2,
    arg_coord0 = 3,
    arg_coord1 = 4,
    arg_coord2 = 5,
    arg_coord3 = 6,
    arg_offset0 = 7,
    arg_offset1 = 8,
    arg_channel = 9,
    arg_compareVale = 10,
  };
  // Accessors
  llvm37::Value *get_srv() const { return Instr->getOperand(1); }
  void set_srv(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_sampler() const { return Instr->getOperand(2); }
  void set_sampler(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_coord0() const { return Instr->getOperand(3); }
  void set_coord0(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_coord1() const { return Instr->getOperand(4); }
  void set_coord1(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_coord2() const { return Instr->getOperand(5); }
  void set_coord2(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_coord3() const { return Instr->getOperand(6); }
  void set_coord3(llvm37::Value *val) { Instr->setOperand(6, val); }
  llvm37::Value *get_offset0() const { return Instr->getOperand(7); }
  void set_offset0(llvm37::Value *val) { Instr->setOperand(7, val); }
  llvm37::Value *get_offset1() const { return Instr->getOperand(8); }
  void set_offset1(llvm37::Value *val) { Instr->setOperand(8, val); }
  llvm37::Value *get_channel() const { return Instr->getOperand(9); }
  void set_channel(llvm37::Value *val) { Instr->setOperand(9, val); }
  llvm37::Value *get_compareVale() const { return Instr->getOperand(10); }
  void set_compareVale(llvm37::Value *val) { Instr->setOperand(10, val); }
};

/// This instruction gets the position of the specified sample
struct DxilInst_Texture2DMSGetSamplePosition {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Texture2DMSGetSamplePosition(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Texture2DMSGetSamplePosition);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_srv = 1,
    arg_index = 2,
  };
  // Accessors
  llvm37::Value *get_srv() const { return Instr->getOperand(1); }
  void set_srv(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_index() const { return Instr->getOperand(2); }
  void set_index(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction gets the position of the specified sample
struct DxilInst_RenderTargetGetSamplePosition {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RenderTargetGetSamplePosition(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RenderTargetGetSamplePosition);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_index = 1,
  };
  // Accessors
  llvm37::Value *get_index() const { return Instr->getOperand(1); }
  void set_index(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction gets the number of samples for a render target
struct DxilInst_RenderTargetGetSampleCount {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RenderTargetGetSampleCount(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RenderTargetGetSampleCount);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction performs an atomic operation on two operands
struct DxilInst_AtomicBinOp {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_AtomicBinOp(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::AtomicBinOp);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (7 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_handle = 1,
    arg_atomicOp = 2,
    arg_offset0 = 3,
    arg_offset1 = 4,
    arg_offset2 = 5,
    arg_newValue = 6,
  };
  // Accessors
  llvm37::Value *get_handle() const { return Instr->getOperand(1); }
  void set_handle(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_atomicOp() const { return Instr->getOperand(2); }
  void set_atomicOp(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_offset0() const { return Instr->getOperand(3); }
  void set_offset0(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_offset1() const { return Instr->getOperand(4); }
  void set_offset1(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_offset2() const { return Instr->getOperand(5); }
  void set_offset2(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_newValue() const { return Instr->getOperand(6); }
  void set_newValue(llvm37::Value *val) { Instr->setOperand(6, val); }
};

/// This instruction atomic compare and exchange to memory
struct DxilInst_AtomicCompareExchange {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_AtomicCompareExchange(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::AtomicCompareExchange);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (7 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_handle = 1,
    arg_offset0 = 2,
    arg_offset1 = 3,
    arg_offset2 = 4,
    arg_compareValue = 5,
    arg_newValue = 6,
  };
  // Accessors
  llvm37::Value *get_handle() const { return Instr->getOperand(1); }
  void set_handle(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_offset0() const { return Instr->getOperand(2); }
  void set_offset0(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_offset1() const { return Instr->getOperand(3); }
  void set_offset1(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_offset2() const { return Instr->getOperand(4); }
  void set_offset2(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_compareValue() const { return Instr->getOperand(5); }
  void set_compareValue(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_newValue() const { return Instr->getOperand(6); }
  void set_newValue(llvm37::Value *val) { Instr->setOperand(6, val); }
};

/// This instruction inserts a memory barrier in the shader
struct DxilInst_Barrier {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Barrier(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Barrier);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_barrierMode = 1,
  };
  // Accessors
  llvm37::Value *get_barrierMode() const { return Instr->getOperand(1); }
  void set_barrierMode(llvm37::Value *val) { Instr->setOperand(1, val); }
  int32_t get_barrierMode_val() const { return (int32_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(1))->getZExtValue()); }
  void set_barrierMode_val(int32_t val) { Instr->setOperand(1, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 32), llvm37::APInt(32, (uint64_t)val))); }
};

/// This instruction calculates the level of detail
struct DxilInst_CalculateLOD {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_CalculateLOD(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::CalculateLOD);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (7 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_handle = 1,
    arg_sampler = 2,
    arg_coord0 = 3,
    arg_coord1 = 4,
    arg_coord2 = 5,
    arg_clamped = 6,
  };
  // Accessors
  llvm37::Value *get_handle() const { return Instr->getOperand(1); }
  void set_handle(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_sampler() const { return Instr->getOperand(2); }
  void set_sampler(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_coord0() const { return Instr->getOperand(3); }
  void set_coord0(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_coord1() const { return Instr->getOperand(4); }
  void set_coord1(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_coord2() const { return Instr->getOperand(5); }
  void set_coord2(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_clamped() const { return Instr->getOperand(6); }
  void set_clamped(llvm37::Value *val) { Instr->setOperand(6, val); }
};

/// This instruction discard the current pixel
struct DxilInst_Discard {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Discard(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Discard);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_condition = 1,
  };
  // Accessors
  llvm37::Value *get_condition() const { return Instr->getOperand(1); }
  void set_condition(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction computes the rate of change per stamp in x direction.
struct DxilInst_DerivCoarseX {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_DerivCoarseX(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::DerivCoarseX);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction computes the rate of change per stamp in y direction.
struct DxilInst_DerivCoarseY {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_DerivCoarseY(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::DerivCoarseY);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction computes the rate of change per pixel in x direction.
struct DxilInst_DerivFineX {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_DerivFineX(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::DerivFineX);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction computes the rate of change per pixel in y direction.
struct DxilInst_DerivFineY {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_DerivFineY(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::DerivFineY);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction evaluates an input attribute at pixel center with an offset
struct DxilInst_EvalSnapped {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_EvalSnapped(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::EvalSnapped);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (6 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_inputSigId = 1,
    arg_inputRowIndex = 2,
    arg_inputColIndex = 3,
    arg_offsetX = 4,
    arg_offsetY = 5,
  };
  // Accessors
  llvm37::Value *get_inputSigId() const { return Instr->getOperand(1); }
  void set_inputSigId(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_inputRowIndex() const { return Instr->getOperand(2); }
  void set_inputRowIndex(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_inputColIndex() const { return Instr->getOperand(3); }
  void set_inputColIndex(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_offsetX() const { return Instr->getOperand(4); }
  void set_offsetX(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_offsetY() const { return Instr->getOperand(5); }
  void set_offsetY(llvm37::Value *val) { Instr->setOperand(5, val); }
};

/// This instruction evaluates an input attribute at a sample location
struct DxilInst_EvalSampleIndex {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_EvalSampleIndex(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::EvalSampleIndex);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (5 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_inputSigId = 1,
    arg_inputRowIndex = 2,
    arg_inputColIndex = 3,
    arg_sampleIndex = 4,
  };
  // Accessors
  llvm37::Value *get_inputSigId() const { return Instr->getOperand(1); }
  void set_inputSigId(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_inputRowIndex() const { return Instr->getOperand(2); }
  void set_inputRowIndex(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_inputColIndex() const { return Instr->getOperand(3); }
  void set_inputColIndex(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_sampleIndex() const { return Instr->getOperand(4); }
  void set_sampleIndex(llvm37::Value *val) { Instr->setOperand(4, val); }
};

/// This instruction evaluates an input attribute at pixel center
struct DxilInst_EvalCentroid {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_EvalCentroid(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::EvalCentroid);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_inputSigId = 1,
    arg_inputRowIndex = 2,
    arg_inputColIndex = 3,
  };
  // Accessors
  llvm37::Value *get_inputSigId() const { return Instr->getOperand(1); }
  void set_inputSigId(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_inputRowIndex() const { return Instr->getOperand(2); }
  void set_inputRowIndex(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_inputColIndex() const { return Instr->getOperand(3); }
  void set_inputColIndex(llvm37::Value *val) { Instr->setOperand(3, val); }
};

/// This instruction returns the sample index in a sample-frequency pixel shader
struct DxilInst_SampleIndex {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_SampleIndex(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::SampleIndex);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction returns the coverage mask input in a pixel shader
struct DxilInst_Coverage {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Coverage(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Coverage);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction returns underestimated coverage input from conservative rasterization in a pixel shader
struct DxilInst_InnerCoverage {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_InnerCoverage(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::InnerCoverage);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction reads the thread ID
struct DxilInst_ThreadId {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_ThreadId(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::ThreadId);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_component = 1,
  };
  // Accessors
  llvm37::Value *get_component() const { return Instr->getOperand(1); }
  void set_component(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction reads the group ID (SV_GroupID)
struct DxilInst_GroupId {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_GroupId(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::GroupId);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_component = 1,
  };
  // Accessors
  llvm37::Value *get_component() const { return Instr->getOperand(1); }
  void set_component(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction reads the thread ID within the group (SV_GroupThreadID)
struct DxilInst_ThreadIdInGroup {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_ThreadIdInGroup(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::ThreadIdInGroup);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_component = 1,
  };
  // Accessors
  llvm37::Value *get_component() const { return Instr->getOperand(1); }
  void set_component(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction provides a flattened index for a given thread within a given group (SV_GroupIndex)
struct DxilInst_FlattenedThreadIdInGroup {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_FlattenedThreadIdInGroup(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::FlattenedThreadIdInGroup);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction emits a vertex to a given stream
struct DxilInst_EmitStream {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_EmitStream(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::EmitStream);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_streamId = 1,
  };
  // Accessors
  llvm37::Value *get_streamId() const { return Instr->getOperand(1); }
  void set_streamId(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction completes the current primitive topology at the specified stream
struct DxilInst_CutStream {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_CutStream(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::CutStream);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_streamId = 1,
  };
  // Accessors
  llvm37::Value *get_streamId() const { return Instr->getOperand(1); }
  void set_streamId(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction equivalent to an EmitStream followed by a CutStream
struct DxilInst_EmitThenCutStream {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_EmitThenCutStream(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::EmitThenCutStream);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_streamId = 1,
  };
  // Accessors
  llvm37::Value *get_streamId() const { return Instr->getOperand(1); }
  void set_streamId(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction GSInstanceID
struct DxilInst_GSInstanceID {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_GSInstanceID(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::GSInstanceID);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction creates a double value
struct DxilInst_MakeDouble {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_MakeDouble(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::MakeDouble);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_lo = 1,
    arg_hi = 2,
  };
  // Accessors
  llvm37::Value *get_lo() const { return Instr->getOperand(1); }
  void set_lo(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_hi() const { return Instr->getOperand(2); }
  void set_hi(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction splits a double into low and high parts
struct DxilInst_SplitDouble {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_SplitDouble(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::SplitDouble);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction LoadOutputControlPoint
struct DxilInst_LoadOutputControlPoint {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_LoadOutputControlPoint(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::LoadOutputControlPoint);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (5 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_inputSigId = 1,
    arg_row = 2,
    arg_col = 3,
    arg_index = 4,
  };
  // Accessors
  llvm37::Value *get_inputSigId() const { return Instr->getOperand(1); }
  void set_inputSigId(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_row() const { return Instr->getOperand(2); }
  void set_row(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_col() const { return Instr->getOperand(3); }
  void set_col(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_index() const { return Instr->getOperand(4); }
  void set_index(llvm37::Value *val) { Instr->setOperand(4, val); }
};

/// This instruction LoadPatchConstant
struct DxilInst_LoadPatchConstant {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_LoadPatchConstant(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::LoadPatchConstant);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_inputSigId = 1,
    arg_row = 2,
    arg_col = 3,
  };
  // Accessors
  llvm37::Value *get_inputSigId() const { return Instr->getOperand(1); }
  void set_inputSigId(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_row() const { return Instr->getOperand(2); }
  void set_row(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_col() const { return Instr->getOperand(3); }
  void set_col(llvm37::Value *val) { Instr->setOperand(3, val); }
};

/// This instruction DomainLocation
struct DxilInst_DomainLocation {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_DomainLocation(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::DomainLocation);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_component = 1,
  };
  // Accessors
  llvm37::Value *get_component() const { return Instr->getOperand(1); }
  void set_component(llvm37::Value *val) { Instr->setOperand(1, val); }
  int8_t get_component_val() const { return (int8_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(1))->getZExtValue()); }
  void set_component_val(int8_t val) { Instr->setOperand(1, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 8), llvm37::APInt(8, (uint64_t)val))); }
};

/// This instruction StorePatchConstant
struct DxilInst_StorePatchConstant {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_StorePatchConstant(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::StorePatchConstant);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (5 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_outputSigID = 1,
    arg_row = 2,
    arg_col = 3,
    arg_value = 4,
  };
  // Accessors
  llvm37::Value *get_outputSigID() const { return Instr->getOperand(1); }
  void set_outputSigID(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_row() const { return Instr->getOperand(2); }
  void set_row(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_col() const { return Instr->getOperand(3); }
  void set_col(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_value() const { return Instr->getOperand(4); }
  void set_value(llvm37::Value *val) { Instr->setOperand(4, val); }
};

/// This instruction OutputControlPointID
struct DxilInst_OutputControlPointID {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_OutputControlPointID(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::OutputControlPointID);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction PrimitiveID
struct DxilInst_PrimitiveID {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_PrimitiveID(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::PrimitiveID);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction CycleCounterLegacy
struct DxilInst_CycleCounterLegacy {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_CycleCounterLegacy(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::CycleCounterLegacy);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction returns 1 for the first lane in the wave
struct DxilInst_WaveIsFirstLane {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WaveIsFirstLane(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WaveIsFirstLane);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction returns the index of the current lane in the wave
struct DxilInst_WaveGetLaneIndex {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WaveGetLaneIndex(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WaveGetLaneIndex);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction returns the number of lanes in the wave
struct DxilInst_WaveGetLaneCount {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WaveGetLaneCount(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WaveGetLaneCount);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction returns 1 if any of the lane evaluates the value to true
struct DxilInst_WaveAnyTrue {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WaveAnyTrue(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WaveAnyTrue);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_cond = 1,
  };
  // Accessors
  llvm37::Value *get_cond() const { return Instr->getOperand(1); }
  void set_cond(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns 1 if all the lanes evaluate the value to true
struct DxilInst_WaveAllTrue {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WaveAllTrue(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WaveAllTrue);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_cond = 1,
  };
  // Accessors
  llvm37::Value *get_cond() const { return Instr->getOperand(1); }
  void set_cond(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns 1 if all the lanes have the same value
struct DxilInst_WaveActiveAllEqual {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WaveActiveAllEqual(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WaveActiveAllEqual);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns a struct with a bit set for each lane where the condition is true
struct DxilInst_WaveActiveBallot {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WaveActiveBallot(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WaveActiveBallot);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_cond = 1,
  };
  // Accessors
  llvm37::Value *get_cond() const { return Instr->getOperand(1); }
  void set_cond(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns the value from the specified lane
struct DxilInst_WaveReadLaneAt {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WaveReadLaneAt(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WaveReadLaneAt);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
    arg_lane = 2,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_lane() const { return Instr->getOperand(2); }
  void set_lane(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction returns the value from the first lane
struct DxilInst_WaveReadLaneFirst {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WaveReadLaneFirst(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WaveReadLaneFirst);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns the result the operation across waves
struct DxilInst_WaveActiveOp {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WaveActiveOp(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WaveActiveOp);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
    arg_op = 2,
    arg_sop = 3,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_op() const { return Instr->getOperand(2); }
  void set_op(llvm37::Value *val) { Instr->setOperand(2, val); }
  int8_t get_op_val() const { return (int8_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(2))->getZExtValue()); }
  void set_op_val(int8_t val) { Instr->setOperand(2, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 8), llvm37::APInt(8, (uint64_t)val))); }
  llvm37::Value *get_sop() const { return Instr->getOperand(3); }
  void set_sop(llvm37::Value *val) { Instr->setOperand(3, val); }
  int8_t get_sop_val() const { return (int8_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(3))->getZExtValue()); }
  void set_sop_val(int8_t val) { Instr->setOperand(3, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 8), llvm37::APInt(8, (uint64_t)val))); }
};

/// This instruction returns the result of the operation across all lanes
struct DxilInst_WaveActiveBit {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WaveActiveBit(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WaveActiveBit);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
    arg_op = 2,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_op() const { return Instr->getOperand(2); }
  void set_op(llvm37::Value *val) { Instr->setOperand(2, val); }
  int8_t get_op_val() const { return (int8_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(2))->getZExtValue()); }
  void set_op_val(int8_t val) { Instr->setOperand(2, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 8), llvm37::APInt(8, (uint64_t)val))); }
};

/// This instruction returns the result of the operation on prior lanes
struct DxilInst_WavePrefixOp {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WavePrefixOp(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WavePrefixOp);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
    arg_op = 2,
    arg_sop = 3,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_op() const { return Instr->getOperand(2); }
  void set_op(llvm37::Value *val) { Instr->setOperand(2, val); }
  int8_t get_op_val() const { return (int8_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(2))->getZExtValue()); }
  void set_op_val(int8_t val) { Instr->setOperand(2, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 8), llvm37::APInt(8, (uint64_t)val))); }
  llvm37::Value *get_sop() const { return Instr->getOperand(3); }
  void set_sop(llvm37::Value *val) { Instr->setOperand(3, val); }
  int8_t get_sop_val() const { return (int8_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(3))->getZExtValue()); }
  void set_sop_val(int8_t val) { Instr->setOperand(3, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 8), llvm37::APInt(8, (uint64_t)val))); }
};

/// This instruction reads from a lane in the quad
struct DxilInst_QuadReadLaneAt {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_QuadReadLaneAt(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::QuadReadLaneAt);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
    arg_quadLane = 2,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_quadLane() const { return Instr->getOperand(2); }
  void set_quadLane(llvm37::Value *val) { Instr->setOperand(2, val); }
  uint32_t get_quadLane_val() const { return (uint32_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(2))->getZExtValue()); }
  void set_quadLane_val(uint32_t val) { Instr->setOperand(2, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 32), llvm37::APInt(32, (uint64_t)val))); }
};

/// This instruction returns the result of a quad-level operation
struct DxilInst_QuadOp {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_QuadOp(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::QuadOp);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
    arg_op = 2,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_op() const { return Instr->getOperand(2); }
  void set_op(llvm37::Value *val) { Instr->setOperand(2, val); }
  int8_t get_op_val() const { return (int8_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(2))->getZExtValue()); }
  void set_op_val(int8_t val) { Instr->setOperand(2, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 8), llvm37::APInt(8, (uint64_t)val))); }
};

/// This instruction bitcast between different sizes
struct DxilInst_BitcastI16toF16 {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_BitcastI16toF16(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::BitcastI16toF16);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction bitcast between different sizes
struct DxilInst_BitcastF16toI16 {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_BitcastF16toI16(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::BitcastF16toI16);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction bitcast between different sizes
struct DxilInst_BitcastI32toF32 {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_BitcastI32toF32(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::BitcastI32toF32);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction bitcast between different sizes
struct DxilInst_BitcastF32toI32 {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_BitcastF32toI32(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::BitcastF32toI32);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction bitcast between different sizes
struct DxilInst_BitcastI64toF64 {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_BitcastI64toF64(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::BitcastI64toF64);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction bitcast between different sizes
struct DxilInst_BitcastF64toI64 {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_BitcastF64toI64(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::BitcastF64toI64);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction legacy fuction to convert float (f32) to half (f16) (this is not related to min-precision)
struct DxilInst_LegacyF32ToF16 {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_LegacyF32ToF16(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::LegacyF32ToF16);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction legacy fuction to convert half (f16) to float (f32) (this is not related to min-precision)
struct DxilInst_LegacyF16ToF32 {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_LegacyF16ToF32(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::LegacyF16ToF32);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction legacy fuction to convert double to float
struct DxilInst_LegacyDoubleToFloat {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_LegacyDoubleToFloat(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::LegacyDoubleToFloat);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction legacy fuction to convert double to int32
struct DxilInst_LegacyDoubleToSInt32 {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_LegacyDoubleToSInt32(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::LegacyDoubleToSInt32);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction legacy fuction to convert double to uint32
struct DxilInst_LegacyDoubleToUInt32 {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_LegacyDoubleToUInt32(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::LegacyDoubleToUInt32);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns the count of bits set to 1 across the wave
struct DxilInst_WaveAllBitCount {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WaveAllBitCount(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WaveAllBitCount);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns the count of bits set to 1 on prior lanes
struct DxilInst_WavePrefixBitCount {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WavePrefixBitCount(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WavePrefixBitCount);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns the values of the attributes at the vertex.
struct DxilInst_AttributeAtVertex {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_AttributeAtVertex(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::AttributeAtVertex);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (5 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_inputSigId = 1,
    arg_inputRowIndex = 2,
    arg_inputColIndex = 3,
    arg_VertexID = 4,
  };
  // Accessors
  llvm37::Value *get_inputSigId() const { return Instr->getOperand(1); }
  void set_inputSigId(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_inputRowIndex() const { return Instr->getOperand(2); }
  void set_inputRowIndex(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_inputColIndex() const { return Instr->getOperand(3); }
  void set_inputColIndex(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_VertexID() const { return Instr->getOperand(4); }
  void set_VertexID(llvm37::Value *val) { Instr->setOperand(4, val); }
};

/// This instruction returns the view index
struct DxilInst_ViewID {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_ViewID(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::ViewID);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction reads from a raw buffer and structured buffer
struct DxilInst_RawBufferLoad {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RawBufferLoad(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RawBufferLoad);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (6 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_srv = 1,
    arg_index = 2,
    arg_elementOffset = 3,
    arg_mask = 4,
    arg_alignment = 5,
  };
  // Accessors
  llvm37::Value *get_srv() const { return Instr->getOperand(1); }
  void set_srv(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_index() const { return Instr->getOperand(2); }
  void set_index(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_elementOffset() const { return Instr->getOperand(3); }
  void set_elementOffset(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_mask() const { return Instr->getOperand(4); }
  void set_mask(llvm37::Value *val) { Instr->setOperand(4, val); }
  int8_t get_mask_val() const { return (int8_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(4))->getZExtValue()); }
  void set_mask_val(int8_t val) { Instr->setOperand(4, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 8), llvm37::APInt(8, (uint64_t)val))); }
  llvm37::Value *get_alignment() const { return Instr->getOperand(5); }
  void set_alignment(llvm37::Value *val) { Instr->setOperand(5, val); }
  int32_t get_alignment_val() const { return (int32_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(5))->getZExtValue()); }
  void set_alignment_val(int32_t val) { Instr->setOperand(5, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 32), llvm37::APInt(32, (uint64_t)val))); }
};

/// This instruction writes to a RWByteAddressBuffer or RWStructuredBuffer
struct DxilInst_RawBufferStore {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RawBufferStore(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RawBufferStore);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (10 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_uav = 1,
    arg_index = 2,
    arg_elementOffset = 3,
    arg_value0 = 4,
    arg_value1 = 5,
    arg_value2 = 6,
    arg_value3 = 7,
    arg_mask = 8,
    arg_alignment = 9,
  };
  // Accessors
  llvm37::Value *get_uav() const { return Instr->getOperand(1); }
  void set_uav(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_index() const { return Instr->getOperand(2); }
  void set_index(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_elementOffset() const { return Instr->getOperand(3); }
  void set_elementOffset(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_value0() const { return Instr->getOperand(4); }
  void set_value0(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_value1() const { return Instr->getOperand(5); }
  void set_value1(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_value2() const { return Instr->getOperand(6); }
  void set_value2(llvm37::Value *val) { Instr->setOperand(6, val); }
  llvm37::Value *get_value3() const { return Instr->getOperand(7); }
  void set_value3(llvm37::Value *val) { Instr->setOperand(7, val); }
  llvm37::Value *get_mask() const { return Instr->getOperand(8); }
  void set_mask(llvm37::Value *val) { Instr->setOperand(8, val); }
  int8_t get_mask_val() const { return (int8_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(8))->getZExtValue()); }
  void set_mask_val(int8_t val) { Instr->setOperand(8, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 8), llvm37::APInt(8, (uint64_t)val))); }
  llvm37::Value *get_alignment() const { return Instr->getOperand(9); }
  void set_alignment(llvm37::Value *val) { Instr->setOperand(9, val); }
  int32_t get_alignment_val() const { return (int32_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(9))->getZExtValue()); }
  void set_alignment_val(int32_t val) { Instr->setOperand(9, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 32), llvm37::APInt(32, (uint64_t)val))); }
};

/// This instruction The user-provided InstanceID on the bottom-level acceleration structure instance within the top-level structure
struct DxilInst_InstanceID {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_InstanceID(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::InstanceID);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction The autogenerated index of the current instance in the top-level structure
struct DxilInst_InstanceIndex {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_InstanceIndex(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::InstanceIndex);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction Returns the value passed as HitKind in ReportIntersection().  If intersection was reported by fixed-function triangle intersection, HitKind will be one of HIT_KIND_TRIANGLE_FRONT_FACE or HIT_KIND_TRIANGLE_BACK_FACE.
struct DxilInst_HitKind {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_HitKind(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::HitKind);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction uint containing the current ray flags.
struct DxilInst_RayFlags {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayFlags(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayFlags);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction The current x and y location within the Width and Height
struct DxilInst_DispatchRaysIndex {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_DispatchRaysIndex(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::DispatchRaysIndex);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_col = 1,
  };
  // Accessors
  llvm37::Value *get_col() const { return Instr->getOperand(1); }
  void set_col(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction The Width and Height values from the D3D12_DISPATCH_RAYS_DESC structure provided to the originating DispatchRays() call.
struct DxilInst_DispatchRaysDimensions {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_DispatchRaysDimensions(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::DispatchRaysDimensions);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_col = 1,
  };
  // Accessors
  llvm37::Value *get_col() const { return Instr->getOperand(1); }
  void set_col(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction The world-space origin for the current ray.
struct DxilInst_WorldRayOrigin {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WorldRayOrigin(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WorldRayOrigin);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_col = 1,
  };
  // Accessors
  llvm37::Value *get_col() const { return Instr->getOperand(1); }
  void set_col(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction The world-space direction for the current ray.
struct DxilInst_WorldRayDirection {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WorldRayDirection(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WorldRayDirection);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_col = 1,
  };
  // Accessors
  llvm37::Value *get_col() const { return Instr->getOperand(1); }
  void set_col(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction Object-space origin for the current ray.
struct DxilInst_ObjectRayOrigin {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_ObjectRayOrigin(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::ObjectRayOrigin);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_col = 1,
  };
  // Accessors
  llvm37::Value *get_col() const { return Instr->getOperand(1); }
  void set_col(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction Object-space direction for the current ray.
struct DxilInst_ObjectRayDirection {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_ObjectRayDirection(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::ObjectRayDirection);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_col = 1,
  };
  // Accessors
  llvm37::Value *get_col() const { return Instr->getOperand(1); }
  void set_col(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction Matrix for transforming from object-space to world-space.
struct DxilInst_ObjectToWorld {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_ObjectToWorld(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::ObjectToWorld);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_row = 1,
    arg_col = 2,
  };
  // Accessors
  llvm37::Value *get_row() const { return Instr->getOperand(1); }
  void set_row(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_col() const { return Instr->getOperand(2); }
  void set_col(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction Matrix for transforming from world-space to object-space.
struct DxilInst_WorldToObject {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WorldToObject(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WorldToObject);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_row = 1,
    arg_col = 2,
  };
  // Accessors
  llvm37::Value *get_row() const { return Instr->getOperand(1); }
  void set_row(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_col() const { return Instr->getOperand(2); }
  void set_col(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction float representing the parametric starting point for the ray.
struct DxilInst_RayTMin {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayTMin(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayTMin);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction float representing the current parametric ending point for the ray
struct DxilInst_RayTCurrent {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayTCurrent(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayTCurrent);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction Used in an any hit shader to reject an intersection and terminate the shader
struct DxilInst_IgnoreHit {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_IgnoreHit(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::IgnoreHit);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction Used in an any hit shader to abort the ray query and the intersection shader (if any). The current hit is committed and execution passes to the closest hit shader with the closest hit recorded so far
struct DxilInst_AcceptHitAndEndSearch {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_AcceptHitAndEndSearch(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::AcceptHitAndEndSearch);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction initiates raytrace
struct DxilInst_TraceRay {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_TraceRay(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::TraceRay);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (16 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_AccelerationStructure = 1,
    arg_RayFlags = 2,
    arg_InstanceInclusionMask = 3,
    arg_RayContributionToHitGroupIndex = 4,
    arg_MultiplierForGeometryContributionToShaderIndex = 5,
    arg_MissShaderIndex = 6,
    arg_Origin_X = 7,
    arg_Origin_Y = 8,
    arg_Origin_Z = 9,
    arg_TMin = 10,
    arg_Direction_X = 11,
    arg_Direction_Y = 12,
    arg_Direction_Z = 13,
    arg_TMax = 14,
    arg_payload = 15,
  };
  // Accessors
  llvm37::Value *get_AccelerationStructure() const { return Instr->getOperand(1); }
  void set_AccelerationStructure(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_RayFlags() const { return Instr->getOperand(2); }
  void set_RayFlags(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_InstanceInclusionMask() const { return Instr->getOperand(3); }
  void set_InstanceInclusionMask(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_RayContributionToHitGroupIndex() const { return Instr->getOperand(4); }
  void set_RayContributionToHitGroupIndex(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_MultiplierForGeometryContributionToShaderIndex() const { return Instr->getOperand(5); }
  void set_MultiplierForGeometryContributionToShaderIndex(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_MissShaderIndex() const { return Instr->getOperand(6); }
  void set_MissShaderIndex(llvm37::Value *val) { Instr->setOperand(6, val); }
  llvm37::Value *get_Origin_X() const { return Instr->getOperand(7); }
  void set_Origin_X(llvm37::Value *val) { Instr->setOperand(7, val); }
  llvm37::Value *get_Origin_Y() const { return Instr->getOperand(8); }
  void set_Origin_Y(llvm37::Value *val) { Instr->setOperand(8, val); }
  llvm37::Value *get_Origin_Z() const { return Instr->getOperand(9); }
  void set_Origin_Z(llvm37::Value *val) { Instr->setOperand(9, val); }
  llvm37::Value *get_TMin() const { return Instr->getOperand(10); }
  void set_TMin(llvm37::Value *val) { Instr->setOperand(10, val); }
  llvm37::Value *get_Direction_X() const { return Instr->getOperand(11); }
  void set_Direction_X(llvm37::Value *val) { Instr->setOperand(11, val); }
  llvm37::Value *get_Direction_Y() const { return Instr->getOperand(12); }
  void set_Direction_Y(llvm37::Value *val) { Instr->setOperand(12, val); }
  llvm37::Value *get_Direction_Z() const { return Instr->getOperand(13); }
  void set_Direction_Z(llvm37::Value *val) { Instr->setOperand(13, val); }
  llvm37::Value *get_TMax() const { return Instr->getOperand(14); }
  void set_TMax(llvm37::Value *val) { Instr->setOperand(14, val); }
  llvm37::Value *get_payload() const { return Instr->getOperand(15); }
  void set_payload(llvm37::Value *val) { Instr->setOperand(15, val); }
};

/// This instruction returns true if hit was accepted
struct DxilInst_ReportHit {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_ReportHit(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::ReportHit);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_THit = 1,
    arg_HitKind = 2,
    arg_Attributes = 3,
  };
  // Accessors
  llvm37::Value *get_THit() const { return Instr->getOperand(1); }
  void set_THit(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_HitKind() const { return Instr->getOperand(2); }
  void set_HitKind(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_Attributes() const { return Instr->getOperand(3); }
  void set_Attributes(llvm37::Value *val) { Instr->setOperand(3, val); }
};

/// This instruction Call a shader in the callable shader table supplied through the DispatchRays() API
struct DxilInst_CallShader {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_CallShader(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::CallShader);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_ShaderIndex = 1,
    arg_Parameter = 2,
  };
  // Accessors
  llvm37::Value *get_ShaderIndex() const { return Instr->getOperand(1); }
  void set_ShaderIndex(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_Parameter() const { return Instr->getOperand(2); }
  void set_Parameter(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction create resource handle from resource struct for library
struct DxilInst_CreateHandleForLib {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_CreateHandleForLib(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::CreateHandleForLib);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_Resource = 1,
  };
  // Accessors
  llvm37::Value *get_Resource() const { return Instr->getOperand(1); }
  void set_Resource(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction PrimitiveIndex for raytracing shaders
struct DxilInst_PrimitiveIndex {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_PrimitiveIndex(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::PrimitiveIndex);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction 2D half dot product with accumulate to float
struct DxilInst_Dot2AddHalf {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Dot2AddHalf(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Dot2AddHalf);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (6 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_acc = 1,
    arg_ax = 2,
    arg_ay = 3,
    arg_bx = 4,
    arg_by = 5,
  };
  // Accessors
  llvm37::Value *get_acc() const { return Instr->getOperand(1); }
  void set_acc(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_ax() const { return Instr->getOperand(2); }
  void set_ax(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_ay() const { return Instr->getOperand(3); }
  void set_ay(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_bx() const { return Instr->getOperand(4); }
  void set_bx(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_by() const { return Instr->getOperand(5); }
  void set_by(llvm37::Value *val) { Instr->setOperand(5, val); }
};

/// This instruction signed dot product of 4 x i8 vectors packed into i32, with accumulate to i32
struct DxilInst_Dot4AddI8Packed {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Dot4AddI8Packed(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Dot4AddI8Packed);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_acc = 1,
    arg_a = 2,
    arg_b = 3,
  };
  // Accessors
  llvm37::Value *get_acc() const { return Instr->getOperand(1); }
  void set_acc(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_a() const { return Instr->getOperand(2); }
  void set_a(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_b() const { return Instr->getOperand(3); }
  void set_b(llvm37::Value *val) { Instr->setOperand(3, val); }
};

/// This instruction unsigned dot product of 4 x u8 vectors packed into i32, with accumulate to i32
struct DxilInst_Dot4AddU8Packed {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Dot4AddU8Packed(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Dot4AddU8Packed);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_acc = 1,
    arg_a = 2,
    arg_b = 3,
  };
  // Accessors
  llvm37::Value *get_acc() const { return Instr->getOperand(1); }
  void set_acc(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_a() const { return Instr->getOperand(2); }
  void set_a(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_b() const { return Instr->getOperand(3); }
  void set_b(llvm37::Value *val) { Instr->setOperand(3, val); }
};

/// This instruction returns the bitmask of active lanes that have the same value
struct DxilInst_WaveMatch {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WaveMatch(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WaveMatch);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns the result of the operation on groups of lanes identified by a bitmask
struct DxilInst_WaveMultiPrefixOp {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WaveMultiPrefixOp(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WaveMultiPrefixOp);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (8 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
    arg_mask0 = 2,
    arg_mask1 = 3,
    arg_mask2 = 4,
    arg_mask3 = 5,
    arg_op = 6,
    arg_sop = 7,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_mask0() const { return Instr->getOperand(2); }
  void set_mask0(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_mask1() const { return Instr->getOperand(3); }
  void set_mask1(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_mask2() const { return Instr->getOperand(4); }
  void set_mask2(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_mask3() const { return Instr->getOperand(5); }
  void set_mask3(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_op() const { return Instr->getOperand(6); }
  void set_op(llvm37::Value *val) { Instr->setOperand(6, val); }
  int8_t get_op_val() const { return (int8_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(6))->getZExtValue()); }
  void set_op_val(int8_t val) { Instr->setOperand(6, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 8), llvm37::APInt(8, (uint64_t)val))); }
  llvm37::Value *get_sop() const { return Instr->getOperand(7); }
  void set_sop(llvm37::Value *val) { Instr->setOperand(7, val); }
  int8_t get_sop_val() const { return (int8_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(7))->getZExtValue()); }
  void set_sop_val(int8_t val) { Instr->setOperand(7, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 8), llvm37::APInt(8, (uint64_t)val))); }
};

/// This instruction returns the count of bits set to 1 on groups of lanes identified by a bitmask
struct DxilInst_WaveMultiPrefixBitCount {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WaveMultiPrefixBitCount(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WaveMultiPrefixBitCount);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (6 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_value = 1,
    arg_mask0 = 2,
    arg_mask1 = 3,
    arg_mask2 = 4,
    arg_mask3 = 5,
  };
  // Accessors
  llvm37::Value *get_value() const { return Instr->getOperand(1); }
  void set_value(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_mask0() const { return Instr->getOperand(2); }
  void set_mask0(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_mask1() const { return Instr->getOperand(3); }
  void set_mask1(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_mask2() const { return Instr->getOperand(4); }
  void set_mask2(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_mask3() const { return Instr->getOperand(5); }
  void set_mask3(llvm37::Value *val) { Instr->setOperand(5, val); }
};

/// This instruction Mesh shader intrinsic SetMeshOutputCounts
struct DxilInst_SetMeshOutputCounts {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_SetMeshOutputCounts(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::SetMeshOutputCounts);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_numVertices = 1,
    arg_numPrimitives = 2,
  };
  // Accessors
  llvm37::Value *get_numVertices() const { return Instr->getOperand(1); }
  void set_numVertices(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_numPrimitives() const { return Instr->getOperand(2); }
  void set_numPrimitives(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction emit a primitive's vertex indices in a mesh shader
struct DxilInst_EmitIndices {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_EmitIndices(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::EmitIndices);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (5 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_PrimitiveIndex = 1,
    arg_VertexIndex0 = 2,
    arg_VertexIndex1 = 3,
    arg_VertexIndex2 = 4,
  };
  // Accessors
  llvm37::Value *get_PrimitiveIndex() const { return Instr->getOperand(1); }
  void set_PrimitiveIndex(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_VertexIndex0() const { return Instr->getOperand(2); }
  void set_VertexIndex0(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_VertexIndex1() const { return Instr->getOperand(3); }
  void set_VertexIndex1(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_VertexIndex2() const { return Instr->getOperand(4); }
  void set_VertexIndex2(llvm37::Value *val) { Instr->setOperand(4, val); }
};

/// This instruction get the mesh payload which is from amplification shader
struct DxilInst_GetMeshPayload {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_GetMeshPayload(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::GetMeshPayload);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction stores the value to mesh shader vertex output
struct DxilInst_StoreVertexOutput {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_StoreVertexOutput(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::StoreVertexOutput);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (6 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_outputSigId = 1,
    arg_rowIndex = 2,
    arg_colIndex = 3,
    arg_value = 4,
    arg_vertexIndex = 5,
  };
  // Accessors
  llvm37::Value *get_outputSigId() const { return Instr->getOperand(1); }
  void set_outputSigId(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_rowIndex() const { return Instr->getOperand(2); }
  void set_rowIndex(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_colIndex() const { return Instr->getOperand(3); }
  void set_colIndex(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_value() const { return Instr->getOperand(4); }
  void set_value(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_vertexIndex() const { return Instr->getOperand(5); }
  void set_vertexIndex(llvm37::Value *val) { Instr->setOperand(5, val); }
};

/// This instruction stores the value to mesh shader primitive output
struct DxilInst_StorePrimitiveOutput {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_StorePrimitiveOutput(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::StorePrimitiveOutput);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (6 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_outputSigId = 1,
    arg_rowIndex = 2,
    arg_colIndex = 3,
    arg_value = 4,
    arg_primitiveIndex = 5,
  };
  // Accessors
  llvm37::Value *get_outputSigId() const { return Instr->getOperand(1); }
  void set_outputSigId(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_rowIndex() const { return Instr->getOperand(2); }
  void set_rowIndex(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_colIndex() const { return Instr->getOperand(3); }
  void set_colIndex(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_value() const { return Instr->getOperand(4); }
  void set_value(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_primitiveIndex() const { return Instr->getOperand(5); }
  void set_primitiveIndex(llvm37::Value *val) { Instr->setOperand(5, val); }
};

/// This instruction Amplification shader intrinsic DispatchMesh
struct DxilInst_DispatchMesh {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_DispatchMesh(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::DispatchMesh);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (5 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_threadGroupCountX = 1,
    arg_threadGroupCountY = 2,
    arg_threadGroupCountZ = 3,
    arg_payload = 4,
  };
  // Accessors
  llvm37::Value *get_threadGroupCountX() const { return Instr->getOperand(1); }
  void set_threadGroupCountX(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_threadGroupCountY() const { return Instr->getOperand(2); }
  void set_threadGroupCountY(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_threadGroupCountZ() const { return Instr->getOperand(3); }
  void set_threadGroupCountZ(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_payload() const { return Instr->getOperand(4); }
  void set_payload(llvm37::Value *val) { Instr->setOperand(4, val); }
};

/// This instruction updates a feedback texture for a sampling operation
struct DxilInst_WriteSamplerFeedback {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WriteSamplerFeedback(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WriteSamplerFeedback);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (9 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_feedbackTex = 1,
    arg_sampledTex = 2,
    arg_sampler = 3,
    arg_c0 = 4,
    arg_c1 = 5,
    arg_c2 = 6,
    arg_c3 = 7,
    arg_clamp = 8,
  };
  // Accessors
  llvm37::Value *get_feedbackTex() const { return Instr->getOperand(1); }
  void set_feedbackTex(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_sampledTex() const { return Instr->getOperand(2); }
  void set_sampledTex(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_sampler() const { return Instr->getOperand(3); }
  void set_sampler(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_c0() const { return Instr->getOperand(4); }
  void set_c0(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_c1() const { return Instr->getOperand(5); }
  void set_c1(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_c2() const { return Instr->getOperand(6); }
  void set_c2(llvm37::Value *val) { Instr->setOperand(6, val); }
  llvm37::Value *get_c3() const { return Instr->getOperand(7); }
  void set_c3(llvm37::Value *val) { Instr->setOperand(7, val); }
  llvm37::Value *get_clamp() const { return Instr->getOperand(8); }
  void set_clamp(llvm37::Value *val) { Instr->setOperand(8, val); }
};

/// This instruction updates a feedback texture for a sampling operation with a bias on the mipmap level
struct DxilInst_WriteSamplerFeedbackBias {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WriteSamplerFeedbackBias(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WriteSamplerFeedbackBias);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (10 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_feedbackTex = 1,
    arg_sampledTex = 2,
    arg_sampler = 3,
    arg_c0 = 4,
    arg_c1 = 5,
    arg_c2 = 6,
    arg_c3 = 7,
    arg_bias = 8,
    arg_clamp = 9,
  };
  // Accessors
  llvm37::Value *get_feedbackTex() const { return Instr->getOperand(1); }
  void set_feedbackTex(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_sampledTex() const { return Instr->getOperand(2); }
  void set_sampledTex(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_sampler() const { return Instr->getOperand(3); }
  void set_sampler(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_c0() const { return Instr->getOperand(4); }
  void set_c0(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_c1() const { return Instr->getOperand(5); }
  void set_c1(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_c2() const { return Instr->getOperand(6); }
  void set_c2(llvm37::Value *val) { Instr->setOperand(6, val); }
  llvm37::Value *get_c3() const { return Instr->getOperand(7); }
  void set_c3(llvm37::Value *val) { Instr->setOperand(7, val); }
  llvm37::Value *get_bias() const { return Instr->getOperand(8); }
  void set_bias(llvm37::Value *val) { Instr->setOperand(8, val); }
  llvm37::Value *get_clamp() const { return Instr->getOperand(9); }
  void set_clamp(llvm37::Value *val) { Instr->setOperand(9, val); }
};

/// This instruction updates a feedback texture for a sampling operation with a mipmap-level offset
struct DxilInst_WriteSamplerFeedbackLevel {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WriteSamplerFeedbackLevel(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WriteSamplerFeedbackLevel);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (9 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_feedbackTex = 1,
    arg_sampledTex = 2,
    arg_sampler = 3,
    arg_c0 = 4,
    arg_c1 = 5,
    arg_c2 = 6,
    arg_c3 = 7,
    arg_lod = 8,
  };
  // Accessors
  llvm37::Value *get_feedbackTex() const { return Instr->getOperand(1); }
  void set_feedbackTex(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_sampledTex() const { return Instr->getOperand(2); }
  void set_sampledTex(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_sampler() const { return Instr->getOperand(3); }
  void set_sampler(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_c0() const { return Instr->getOperand(4); }
  void set_c0(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_c1() const { return Instr->getOperand(5); }
  void set_c1(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_c2() const { return Instr->getOperand(6); }
  void set_c2(llvm37::Value *val) { Instr->setOperand(6, val); }
  llvm37::Value *get_c3() const { return Instr->getOperand(7); }
  void set_c3(llvm37::Value *val) { Instr->setOperand(7, val); }
  llvm37::Value *get_lod() const { return Instr->getOperand(8); }
  void set_lod(llvm37::Value *val) { Instr->setOperand(8, val); }
};

/// This instruction updates a feedback texture for a sampling operation with explicit gradients
struct DxilInst_WriteSamplerFeedbackGrad {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_WriteSamplerFeedbackGrad(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::WriteSamplerFeedbackGrad);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (15 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_feedbackTex = 1,
    arg_sampledTex = 2,
    arg_sampler = 3,
    arg_c0 = 4,
    arg_c1 = 5,
    arg_c2 = 6,
    arg_c3 = 7,
    arg_ddx0 = 8,
    arg_ddx1 = 9,
    arg_ddx2 = 10,
    arg_ddy0 = 11,
    arg_ddy1 = 12,
    arg_ddy2 = 13,
    arg_clamp = 14,
  };
  // Accessors
  llvm37::Value *get_feedbackTex() const { return Instr->getOperand(1); }
  void set_feedbackTex(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_sampledTex() const { return Instr->getOperand(2); }
  void set_sampledTex(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_sampler() const { return Instr->getOperand(3); }
  void set_sampler(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_c0() const { return Instr->getOperand(4); }
  void set_c0(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_c1() const { return Instr->getOperand(5); }
  void set_c1(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_c2() const { return Instr->getOperand(6); }
  void set_c2(llvm37::Value *val) { Instr->setOperand(6, val); }
  llvm37::Value *get_c3() const { return Instr->getOperand(7); }
  void set_c3(llvm37::Value *val) { Instr->setOperand(7, val); }
  llvm37::Value *get_ddx0() const { return Instr->getOperand(8); }
  void set_ddx0(llvm37::Value *val) { Instr->setOperand(8, val); }
  llvm37::Value *get_ddx1() const { return Instr->getOperand(9); }
  void set_ddx1(llvm37::Value *val) { Instr->setOperand(9, val); }
  llvm37::Value *get_ddx2() const { return Instr->getOperand(10); }
  void set_ddx2(llvm37::Value *val) { Instr->setOperand(10, val); }
  llvm37::Value *get_ddy0() const { return Instr->getOperand(11); }
  void set_ddy0(llvm37::Value *val) { Instr->setOperand(11, val); }
  llvm37::Value *get_ddy1() const { return Instr->getOperand(12); }
  void set_ddy1(llvm37::Value *val) { Instr->setOperand(12, val); }
  llvm37::Value *get_ddy2() const { return Instr->getOperand(13); }
  void set_ddy2(llvm37::Value *val) { Instr->setOperand(13, val); }
  llvm37::Value *get_clamp() const { return Instr->getOperand(14); }
  void set_clamp(llvm37::Value *val) { Instr->setOperand(14, val); }
};

/// This instruction allocates space for RayQuery and return handle
struct DxilInst_AllocateRayQuery {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_AllocateRayQuery(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::AllocateRayQuery);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_constRayFlags = 1,
  };
  // Accessors
  llvm37::Value *get_constRayFlags() const { return Instr->getOperand(1); }
  void set_constRayFlags(llvm37::Value *val) { Instr->setOperand(1, val); }
  uint32_t get_constRayFlags_val() const { return (uint32_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(1))->getZExtValue()); }
  void set_constRayFlags_val(uint32_t val) { Instr->setOperand(1, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 32), llvm37::APInt(32, (uint64_t)val))); }
};

/// This instruction initializes RayQuery for raytrace
struct DxilInst_RayQuery_TraceRayInline {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_TraceRayInline(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_TraceRayInline);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (13 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
    arg_accelerationStructure = 2,
    arg_rayFlags = 3,
    arg_instanceInclusionMask = 4,
    arg_origin_X = 5,
    arg_origin_Y = 6,
    arg_origin_Z = 7,
    arg_tMin = 8,
    arg_direction_X = 9,
    arg_direction_Y = 10,
    arg_direction_Z = 11,
    arg_tMax = 12,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_accelerationStructure() const { return Instr->getOperand(2); }
  void set_accelerationStructure(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_rayFlags() const { return Instr->getOperand(3); }
  void set_rayFlags(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_instanceInclusionMask() const { return Instr->getOperand(4); }
  void set_instanceInclusionMask(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_origin_X() const { return Instr->getOperand(5); }
  void set_origin_X(llvm37::Value *val) { Instr->setOperand(5, val); }
  llvm37::Value *get_origin_Y() const { return Instr->getOperand(6); }
  void set_origin_Y(llvm37::Value *val) { Instr->setOperand(6, val); }
  llvm37::Value *get_origin_Z() const { return Instr->getOperand(7); }
  void set_origin_Z(llvm37::Value *val) { Instr->setOperand(7, val); }
  llvm37::Value *get_tMin() const { return Instr->getOperand(8); }
  void set_tMin(llvm37::Value *val) { Instr->setOperand(8, val); }
  llvm37::Value *get_direction_X() const { return Instr->getOperand(9); }
  void set_direction_X(llvm37::Value *val) { Instr->setOperand(9, val); }
  llvm37::Value *get_direction_Y() const { return Instr->getOperand(10); }
  void set_direction_Y(llvm37::Value *val) { Instr->setOperand(10, val); }
  llvm37::Value *get_direction_Z() const { return Instr->getOperand(11); }
  void set_direction_Z(llvm37::Value *val) { Instr->setOperand(11, val); }
  llvm37::Value *get_tMax() const { return Instr->getOperand(12); }
  void set_tMax(llvm37::Value *val) { Instr->setOperand(12, val); }
};

/// This instruction advances a ray query
struct DxilInst_RayQuery_Proceed {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_Proceed(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_Proceed);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction aborts a ray query
struct DxilInst_RayQuery_Abort {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_Abort(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_Abort);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction commits a non opaque triangle hit
struct DxilInst_RayQuery_CommitNonOpaqueTriangleHit {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CommitNonOpaqueTriangleHit(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CommitNonOpaqueTriangleHit);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction commits a procedural primitive hit
struct DxilInst_RayQuery_CommitProceduralPrimitiveHit {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CommitProceduralPrimitiveHit(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CommitProceduralPrimitiveHit);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
    arg_t = 2,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_t() const { return Instr->getOperand(2); }
  void set_t(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction returns uint status (COMMITTED_STATUS) of the committed hit in a ray query
struct DxilInst_RayQuery_CommittedStatus {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CommittedStatus(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CommittedStatus);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns uint candidate type (CANDIDATE_TYPE) of the current hit candidate in a ray query, after Proceed() has returned true
struct DxilInst_RayQuery_CandidateType {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CandidateType(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CandidateType);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns matrix for transforming from object-space to world-space for a candidate hit.
struct DxilInst_RayQuery_CandidateObjectToWorld3x4 {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CandidateObjectToWorld3x4(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CandidateObjectToWorld3x4);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
    arg_row = 2,
    arg_col = 3,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_row() const { return Instr->getOperand(2); }
  void set_row(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_col() const { return Instr->getOperand(3); }
  void set_col(llvm37::Value *val) { Instr->setOperand(3, val); }
};

/// This instruction returns matrix for transforming from world-space to object-space for a candidate hit.
struct DxilInst_RayQuery_CandidateWorldToObject3x4 {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CandidateWorldToObject3x4(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CandidateWorldToObject3x4);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
    arg_row = 2,
    arg_col = 3,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_row() const { return Instr->getOperand(2); }
  void set_row(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_col() const { return Instr->getOperand(3); }
  void set_col(llvm37::Value *val) { Instr->setOperand(3, val); }
};

/// This instruction returns matrix for transforming from object-space to world-space for a Committed hit.
struct DxilInst_RayQuery_CommittedObjectToWorld3x4 {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CommittedObjectToWorld3x4(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CommittedObjectToWorld3x4);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
    arg_row = 2,
    arg_col = 3,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_row() const { return Instr->getOperand(2); }
  void set_row(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_col() const { return Instr->getOperand(3); }
  void set_col(llvm37::Value *val) { Instr->setOperand(3, val); }
};

/// This instruction returns matrix for transforming from world-space to object-space for a Committed hit.
struct DxilInst_RayQuery_CommittedWorldToObject3x4 {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CommittedWorldToObject3x4(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CommittedWorldToObject3x4);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
    arg_row = 2,
    arg_col = 3,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_row() const { return Instr->getOperand(2); }
  void set_row(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_col() const { return Instr->getOperand(3); }
  void set_col(llvm37::Value *val) { Instr->setOperand(3, val); }
};

/// This instruction returns if current candidate procedural primitive is non opaque
struct DxilInst_RayQuery_CandidateProceduralPrimitiveNonOpaque {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CandidateProceduralPrimitiveNonOpaque(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CandidateProceduralPrimitiveNonOpaque);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns if current candidate triangle is front facing
struct DxilInst_RayQuery_CandidateTriangleFrontFace {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CandidateTriangleFrontFace(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CandidateTriangleFrontFace);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns if current committed triangle is front facing
struct DxilInst_RayQuery_CommittedTriangleFrontFace {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CommittedTriangleFrontFace(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CommittedTriangleFrontFace);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns candidate triangle hit barycentrics
struct DxilInst_RayQuery_CandidateTriangleBarycentrics {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CandidateTriangleBarycentrics(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CandidateTriangleBarycentrics);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
    arg_component = 2,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_component() const { return Instr->getOperand(2); }
  void set_component(llvm37::Value *val) { Instr->setOperand(2, val); }
  int8_t get_component_val() const { return (int8_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(2))->getZExtValue()); }
  void set_component_val(int8_t val) { Instr->setOperand(2, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 8), llvm37::APInt(8, (uint64_t)val))); }
};

/// This instruction returns committed triangle hit barycentrics
struct DxilInst_RayQuery_CommittedTriangleBarycentrics {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CommittedTriangleBarycentrics(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CommittedTriangleBarycentrics);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
    arg_component = 2,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_component() const { return Instr->getOperand(2); }
  void set_component(llvm37::Value *val) { Instr->setOperand(2, val); }
  int8_t get_component_val() const { return (int8_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(2))->getZExtValue()); }
  void set_component_val(int8_t val) { Instr->setOperand(2, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 8), llvm37::APInt(8, (uint64_t)val))); }
};

/// This instruction returns ray flags
struct DxilInst_RayQuery_RayFlags {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_RayFlags(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_RayFlags);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns world ray origin
struct DxilInst_RayQuery_WorldRayOrigin {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_WorldRayOrigin(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_WorldRayOrigin);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
    arg_component = 2,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_component() const { return Instr->getOperand(2); }
  void set_component(llvm37::Value *val) { Instr->setOperand(2, val); }
  int8_t get_component_val() const { return (int8_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(2))->getZExtValue()); }
  void set_component_val(int8_t val) { Instr->setOperand(2, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 8), llvm37::APInt(8, (uint64_t)val))); }
};

/// This instruction returns world ray direction
struct DxilInst_RayQuery_WorldRayDirection {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_WorldRayDirection(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_WorldRayDirection);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
    arg_component = 2,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_component() const { return Instr->getOperand(2); }
  void set_component(llvm37::Value *val) { Instr->setOperand(2, val); }
  int8_t get_component_val() const { return (int8_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(2))->getZExtValue()); }
  void set_component_val(int8_t val) { Instr->setOperand(2, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 8), llvm37::APInt(8, (uint64_t)val))); }
};

/// This instruction returns float representing the parametric starting point for the ray.
struct DxilInst_RayQuery_RayTMin {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_RayTMin(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_RayTMin);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns float representing the parametric point on the ray for the current candidate triangle hit.
struct DxilInst_RayQuery_CandidateTriangleRayT {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CandidateTriangleRayT(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CandidateTriangleRayT);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns float representing the parametric point on the ray for the current committed hit.
struct DxilInst_RayQuery_CommittedRayT {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CommittedRayT(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CommittedRayT);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns candidate hit instance index
struct DxilInst_RayQuery_CandidateInstanceIndex {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CandidateInstanceIndex(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CandidateInstanceIndex);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns candidate hit instance ID
struct DxilInst_RayQuery_CandidateInstanceID {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CandidateInstanceID(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CandidateInstanceID);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns candidate hit geometry index
struct DxilInst_RayQuery_CandidateGeometryIndex {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CandidateGeometryIndex(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CandidateGeometryIndex);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns candidate hit geometry index
struct DxilInst_RayQuery_CandidatePrimitiveIndex {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CandidatePrimitiveIndex(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CandidatePrimitiveIndex);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns candidate hit object ray origin
struct DxilInst_RayQuery_CandidateObjectRayOrigin {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CandidateObjectRayOrigin(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CandidateObjectRayOrigin);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
    arg_component = 2,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_component() const { return Instr->getOperand(2); }
  void set_component(llvm37::Value *val) { Instr->setOperand(2, val); }
  int8_t get_component_val() const { return (int8_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(2))->getZExtValue()); }
  void set_component_val(int8_t val) { Instr->setOperand(2, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 8), llvm37::APInt(8, (uint64_t)val))); }
};

/// This instruction returns candidate object ray direction
struct DxilInst_RayQuery_CandidateObjectRayDirection {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CandidateObjectRayDirection(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CandidateObjectRayDirection);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
    arg_component = 2,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_component() const { return Instr->getOperand(2); }
  void set_component(llvm37::Value *val) { Instr->setOperand(2, val); }
  int8_t get_component_val() const { return (int8_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(2))->getZExtValue()); }
  void set_component_val(int8_t val) { Instr->setOperand(2, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 8), llvm37::APInt(8, (uint64_t)val))); }
};

/// This instruction returns committed hit instance index
struct DxilInst_RayQuery_CommittedInstanceIndex {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CommittedInstanceIndex(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CommittedInstanceIndex);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns committed hit instance ID
struct DxilInst_RayQuery_CommittedInstanceID {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CommittedInstanceID(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CommittedInstanceID);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns committed hit geometry index
struct DxilInst_RayQuery_CommittedGeometryIndex {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CommittedGeometryIndex(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CommittedGeometryIndex);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns committed hit geometry index
struct DxilInst_RayQuery_CommittedPrimitiveIndex {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CommittedPrimitiveIndex(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CommittedPrimitiveIndex);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns committed hit object ray origin
struct DxilInst_RayQuery_CommittedObjectRayOrigin {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CommittedObjectRayOrigin(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CommittedObjectRayOrigin);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
    arg_component = 2,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_component() const { return Instr->getOperand(2); }
  void set_component(llvm37::Value *val) { Instr->setOperand(2, val); }
  int8_t get_component_val() const { return (int8_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(2))->getZExtValue()); }
  void set_component_val(int8_t val) { Instr->setOperand(2, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 8), llvm37::APInt(8, (uint64_t)val))); }
};

/// This instruction returns committed object ray direction
struct DxilInst_RayQuery_CommittedObjectRayDirection {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CommittedObjectRayDirection(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CommittedObjectRayDirection);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
    arg_component = 2,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_component() const { return Instr->getOperand(2); }
  void set_component(llvm37::Value *val) { Instr->setOperand(2, val); }
  int8_t get_component_val() const { return (int8_t)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(2))->getZExtValue()); }
  void set_component_val(int8_t val) { Instr->setOperand(2, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 8), llvm37::APInt(8, (uint64_t)val))); }
};

/// This instruction The autogenerated index of the current geometry in the bottom-level structure
struct DxilInst_GeometryIndex {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_GeometryIndex(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::GeometryIndex);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (1 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
};

/// This instruction returns candidate hit InstanceContributionToHitGroupIndex
struct DxilInst_RayQuery_CandidateInstanceContributionToHitGroupIndex {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CandidateInstanceContributionToHitGroupIndex(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CandidateInstanceContributionToHitGroupIndex);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction returns committed hit InstanceContributionToHitGroupIndex
struct DxilInst_RayQuery_CommittedInstanceContributionToHitGroupIndex {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_RayQuery_CommittedInstanceContributionToHitGroupIndex(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::RayQuery_CommittedInstanceContributionToHitGroupIndex);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (2 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_rayQueryHandle = 1,
  };
  // Accessors
  llvm37::Value *get_rayQueryHandle() const { return Instr->getOperand(1); }
  void set_rayQueryHandle(llvm37::Value *val) { Instr->setOperand(1, val); }
};

/// This instruction annotate handle with resource properties
struct DxilInst_AnnotateHandle {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_AnnotateHandle(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::AnnotateHandle);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_res = 1,
    arg_props = 2,
  };
  // Accessors
  llvm37::Value *get_res() const { return Instr->getOperand(1); }
  void set_res(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_props() const { return Instr->getOperand(2); }
  void set_props(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction create resource handle from binding
struct DxilInst_CreateHandleFromBinding {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_CreateHandleFromBinding(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::CreateHandleFromBinding);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_bind = 1,
    arg_index = 2,
    arg_nonUniformIndex = 3,
  };
  // Accessors
  llvm37::Value *get_bind() const { return Instr->getOperand(1); }
  void set_bind(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_index() const { return Instr->getOperand(2); }
  void set_index(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_nonUniformIndex() const { return Instr->getOperand(3); }
  void set_nonUniformIndex(llvm37::Value *val) { Instr->setOperand(3, val); }
  bool get_nonUniformIndex_val() const { return (bool)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(3))->getZExtValue()); }
  void set_nonUniformIndex_val(bool val) { Instr->setOperand(3, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 1), llvm37::APInt(1, (uint64_t)val))); }
};

/// This instruction create resource handle from heap
struct DxilInst_CreateHandleFromHeap {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_CreateHandleFromHeap(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::CreateHandleFromHeap);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (4 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_index = 1,
    arg_samplerHeap = 2,
    arg_nonUniformIndex = 3,
  };
  // Accessors
  llvm37::Value *get_index() const { return Instr->getOperand(1); }
  void set_index(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_samplerHeap() const { return Instr->getOperand(2); }
  void set_samplerHeap(llvm37::Value *val) { Instr->setOperand(2, val); }
  bool get_samplerHeap_val() const { return (bool)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(2))->getZExtValue()); }
  void set_samplerHeap_val(bool val) { Instr->setOperand(2, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 1), llvm37::APInt(1, (uint64_t)val))); }
  llvm37::Value *get_nonUniformIndex() const { return Instr->getOperand(3); }
  void set_nonUniformIndex(llvm37::Value *val) { Instr->setOperand(3, val); }
  bool get_nonUniformIndex_val() const { return (bool)(llvm37::dyn_cast<llvm37::ConstantInt>(Instr->getOperand(3))->getZExtValue()); }
  void set_nonUniformIndex_val(bool val) { Instr->setOperand(3, llvm37::Constant::getIntegerValue(llvm37::IntegerType::get(Instr->getContext(), 1), llvm37::APInt(1, (uint64_t)val))); }
};

/// This instruction unpacks 4 8-bit signed or unsigned values into int32 or int16 vector
struct DxilInst_Unpack4x8 {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Unpack4x8(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Unpack4x8);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (3 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_unpackMode = 1,
    arg_pk = 2,
  };
  // Accessors
  llvm37::Value *get_unpackMode() const { return Instr->getOperand(1); }
  void set_unpackMode(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_pk() const { return Instr->getOperand(2); }
  void set_pk(llvm37::Value *val) { Instr->setOperand(2, val); }
};

/// This instruction packs vector of 4 signed or unsigned values into a packed datatype, drops or clamps unused bits
struct DxilInst_Pack4x8 {
  llvm37::Instruction *Instr;
  // Construction and identification
  DxilInst_Pack4x8(llvm37::Instruction *pInstr) : Instr(pInstr) {}
  operator bool() const {
    return hlsl::OP::IsDxilOpFuncCallInst(Instr, hlsl::OP::OpCode::Pack4x8);
  }
  // Validation support
  bool isAllowed() const { return true; }
  bool isArgumentListValid() const {
    if (6 != llvm37::dyn_cast<llvm37::CallInst>(Instr)->getNumArgOperands()) return false;
    return true;
  }
  // Metadata
  bool requiresUniformInputs() const { return false; }
  // Operand indexes
  enum OperandIdx {
    arg_packMode = 1,
    arg_x = 2,
    arg_y = 3,
    arg_z = 4,
    arg_w = 5,
  };
  // Accessors
  llvm37::Value *get_packMode() const { return Instr->getOperand(1); }
  void set_packMode(llvm37::Value *val) { Instr->setOperand(1, val); }
  llvm37::Value *get_x() const { return Instr->getOperand(2); }
  void set_x(llvm37::Value *val) { Instr->setOperand(2, val); }
  llvm37::Value *get_y() const { return Instr->getOperand(3); }
  void set_y(llvm37::Value *val) { Instr->setOperand(3, val); }
  llvm37::Value *get_z() const { return Instr->getOperand(4); }
  void set_z(llvm37::Value *val) { Instr->setOperand(4, val); }
  llvm37::Value *get_w() const { return Instr->getOperand(5); }
  void set_w(llvm37::Value *val) { Instr->setOperand(5, val); }
};
// INSTR-HELPER:END
} // namespace hlsl
