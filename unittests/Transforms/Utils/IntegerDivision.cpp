//===- IntegerDivision.cpp - Unit tests for the integer division code -----===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Transforms/Utils/IntegerDivision.h"
#include "llvm37/IR/BasicBlock.h"
#include "llvm37/IR/Function.h"
#include "llvm37/IR/GlobalValue.h"
#include "llvm37/IR/IRBuilder.h"
#include "llvm37/IR/Module.h"
#include "gtest/gtest.h"

using namespace llvm37;

namespace {


TEST(IntegerDivision, SDiv) {
  LLVM37Context &C(getGlobalContext());
  Module M("test division", C);
  IRBuilder<> Builder(C);

  SmallVector<Type*, 2> ArgTys(2, Builder.getInt32Ty());
  Function *F = Function::Create(FunctionType::get(Builder.getInt32Ty(),
                                                   ArgTys, false),
                                 GlobalValue::ExternalLinkage, "F", &M);
  assert(F->getArgumentList().size() == 2);

  BasicBlock *BB = BasicBlock::Create(C, "", F);
  Builder.SetInsertPoint(BB);

  Function::arg_iterator AI = F->arg_begin();
  Value *A = AI++;
  Value *B = AI++;

  Value *Div = Builder.CreateSDiv(A, B);
  EXPECT_TRUE(BB->front().getOpcode() == Instruction::SDiv);

  Value *Ret = Builder.CreateRet(Div);

  expandDivision(cast<BinaryOperator>(Div));
  EXPECT_TRUE(BB->front().getOpcode() == Instruction::AShr);

  Instruction* Quotient = dyn_cast<Instruction>(cast<User>(Ret)->getOperand(0));
  EXPECT_TRUE(Quotient && Quotient->getOpcode() == Instruction::Sub);
}

TEST(IntegerDivision, UDiv) {
  LLVM37Context &C(getGlobalContext());
  Module M("test division", C);
  IRBuilder<> Builder(C);

  SmallVector<Type*, 2> ArgTys(2, Builder.getInt32Ty());
  Function *F = Function::Create(FunctionType::get(Builder.getInt32Ty(),
                                                   ArgTys, false),
                                 GlobalValue::ExternalLinkage, "F", &M);
  assert(F->getArgumentList().size() == 2);

  BasicBlock *BB = BasicBlock::Create(C, "", F);
  Builder.SetInsertPoint(BB);

  Function::arg_iterator AI = F->arg_begin();
  Value *A = AI++;
  Value *B = AI++;

  Value *Div = Builder.CreateUDiv(A, B);
  EXPECT_TRUE(BB->front().getOpcode() == Instruction::UDiv);

  Value *Ret = Builder.CreateRet(Div);

  expandDivision(cast<BinaryOperator>(Div));
  EXPECT_TRUE(BB->front().getOpcode() == Instruction::ICmp);

  Instruction* Quotient = dyn_cast<Instruction>(cast<User>(Ret)->getOperand(0));
  EXPECT_TRUE(Quotient && Quotient->getOpcode() == Instruction::PHI);
}

TEST(IntegerDivision, SRem) {
  LLVM37Context &C(getGlobalContext());
  Module M("test remainder", C);
  IRBuilder<> Builder(C);

  SmallVector<Type*, 2> ArgTys(2, Builder.getInt32Ty());
  Function *F = Function::Create(FunctionType::get(Builder.getInt32Ty(),
                                                   ArgTys, false),
                                 GlobalValue::ExternalLinkage, "F", &M);
  assert(F->getArgumentList().size() == 2);

  BasicBlock *BB = BasicBlock::Create(C, "", F);
  Builder.SetInsertPoint(BB);

  Function::arg_iterator AI = F->arg_begin();
  Value *A = AI++;
  Value *B = AI++;

  Value *Rem = Builder.CreateSRem(A, B);
  EXPECT_TRUE(BB->front().getOpcode() == Instruction::SRem);

  Value *Ret = Builder.CreateRet(Rem);

  expandRemainder(cast<BinaryOperator>(Rem));
  EXPECT_TRUE(BB->front().getOpcode() == Instruction::AShr);

  Instruction* Remainder = dyn_cast<Instruction>(cast<User>(Ret)->getOperand(0));
  EXPECT_TRUE(Remainder && Remainder->getOpcode() == Instruction::Sub);
}

TEST(IntegerDivision, URem) {
  LLVM37Context &C(getGlobalContext());
  Module M("test remainder", C);
  IRBuilder<> Builder(C);

  SmallVector<Type*, 2> ArgTys(2, Builder.getInt32Ty());
  Function *F = Function::Create(FunctionType::get(Builder.getInt32Ty(),
                                                   ArgTys, false),
                                 GlobalValue::ExternalLinkage, "F", &M);
  assert(F->getArgumentList().size() == 2);

  BasicBlock *BB = BasicBlock::Create(C, "", F);
  Builder.SetInsertPoint(BB);

  Function::arg_iterator AI = F->arg_begin();
  Value *A = AI++;
  Value *B = AI++;

  Value *Rem = Builder.CreateURem(A, B);
  EXPECT_TRUE(BB->front().getOpcode() == Instruction::URem);

  Value *Ret = Builder.CreateRet(Rem);

  expandRemainder(cast<BinaryOperator>(Rem));
  EXPECT_TRUE(BB->front().getOpcode() == Instruction::ICmp);

  Instruction* Remainder = dyn_cast<Instruction>(cast<User>(Ret)->getOperand(0));
  EXPECT_TRUE(Remainder && Remainder->getOpcode() == Instruction::Sub);
}


TEST(IntegerDivision, SDiv64) {
  LLVM37Context &C(getGlobalContext());
  Module M("test division", C);
  IRBuilder<> Builder(C);

  SmallVector<Type*, 2> ArgTys(2, Builder.getInt64Ty());
  Function *F = Function::Create(FunctionType::get(Builder.getInt64Ty(),
                                                   ArgTys, false),
                                 GlobalValue::ExternalLinkage, "F", &M);
  assert(F->getArgumentList().size() == 2);

  BasicBlock *BB = BasicBlock::Create(C, "", F);
  Builder.SetInsertPoint(BB);

  Function::arg_iterator AI = F->arg_begin();
  Value *A = AI++;
  Value *B = AI++;

  Value *Div = Builder.CreateSDiv(A, B);
  EXPECT_TRUE(BB->front().getOpcode() == Instruction::SDiv);

  Value *Ret = Builder.CreateRet(Div);

  expandDivision(cast<BinaryOperator>(Div));
  EXPECT_TRUE(BB->front().getOpcode() == Instruction::AShr);

  Instruction* Quotient = dyn_cast<Instruction>(cast<User>(Ret)->getOperand(0));
  EXPECT_TRUE(Quotient && Quotient->getOpcode() == Instruction::Sub);
}

TEST(IntegerDivision, UDiv64) {
  LLVM37Context &C(getGlobalContext());
  Module M("test division", C);
  IRBuilder<> Builder(C);

  SmallVector<Type*, 2> ArgTys(2, Builder.getInt64Ty());
  Function *F = Function::Create(FunctionType::get(Builder.getInt64Ty(),
                                                   ArgTys, false),
                                 GlobalValue::ExternalLinkage, "F", &M);
  assert(F->getArgumentList().size() == 2);

  BasicBlock *BB = BasicBlock::Create(C, "", F);
  Builder.SetInsertPoint(BB);

  Function::arg_iterator AI = F->arg_begin();
  Value *A = AI++;
  Value *B = AI++;

  Value *Div = Builder.CreateUDiv(A, B);
  EXPECT_TRUE(BB->front().getOpcode() == Instruction::UDiv);

  Value *Ret = Builder.CreateRet(Div);

  expandDivision(cast<BinaryOperator>(Div));
  EXPECT_TRUE(BB->front().getOpcode() == Instruction::ICmp);

  Instruction* Quotient = dyn_cast<Instruction>(cast<User>(Ret)->getOperand(0));
  EXPECT_TRUE(Quotient && Quotient->getOpcode() == Instruction::PHI);
}

TEST(IntegerDivision, SRem64) {
  LLVM37Context &C(getGlobalContext());
  Module M("test remainder", C);
  IRBuilder<> Builder(C);

  SmallVector<Type*, 2> ArgTys(2, Builder.getInt64Ty());
  Function *F = Function::Create(FunctionType::get(Builder.getInt64Ty(),
                                                   ArgTys, false),
                                 GlobalValue::ExternalLinkage, "F", &M);
  assert(F->getArgumentList().size() == 2);

  BasicBlock *BB = BasicBlock::Create(C, "", F);
  Builder.SetInsertPoint(BB);

  Function::arg_iterator AI = F->arg_begin();
  Value *A = AI++;
  Value *B = AI++;

  Value *Rem = Builder.CreateSRem(A, B);
  EXPECT_TRUE(BB->front().getOpcode() == Instruction::SRem);

  Value *Ret = Builder.CreateRet(Rem);

  expandRemainder(cast<BinaryOperator>(Rem));
  EXPECT_TRUE(BB->front().getOpcode() == Instruction::AShr);

  Instruction* Remainder = dyn_cast<Instruction>(cast<User>(Ret)->getOperand(0));
  EXPECT_TRUE(Remainder && Remainder->getOpcode() == Instruction::Sub);
}

TEST(IntegerDivision, URem64) {
  LLVM37Context &C(getGlobalContext());
  Module M("test remainder", C);
  IRBuilder<> Builder(C);

  SmallVector<Type*, 2> ArgTys(2, Builder.getInt64Ty());
  Function *F = Function::Create(FunctionType::get(Builder.getInt64Ty(),
                                                   ArgTys, false),
                                 GlobalValue::ExternalLinkage, "F", &M);
  assert(F->getArgumentList().size() == 2);

  BasicBlock *BB = BasicBlock::Create(C, "", F);
  Builder.SetInsertPoint(BB);

  Function::arg_iterator AI = F->arg_begin();
  Value *A = AI++;
  Value *B = AI++;

  Value *Rem = Builder.CreateURem(A, B);
  EXPECT_TRUE(BB->front().getOpcode() == Instruction::URem);

  Value *Ret = Builder.CreateRet(Rem);

  expandRemainder(cast<BinaryOperator>(Rem));
  EXPECT_TRUE(BB->front().getOpcode() == Instruction::ICmp);

  Instruction* Remainder = dyn_cast<Instruction>(cast<User>(Ret)->getOperand(0));
  EXPECT_TRUE(Remainder && Remainder->getOpcode() == Instruction::Sub);
}

}
