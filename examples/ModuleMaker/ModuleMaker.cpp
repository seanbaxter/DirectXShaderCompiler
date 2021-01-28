//===- examples/ModuleMaker/ModuleMaker.cpp - Example project ---*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This programs is a simple example that creates an LLVM37 module "from scratch",
// emitting it as a bitcode file to standard out.  This is just to show how
// LLVM37 projects work and to demonstrate some of the LLVM37 APIs.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Bitcode/ReaderWriter.h"
#include "llvm37/IR/Constants.h"
#include "llvm37/IR/DerivedTypes.h"
#include "llvm37/IR/Instructions.h"
#include "llvm37/IR/LLVMContext.h"
#include "llvm37/IR/Module.h"
#include "llvm37/Support/raw_ostream.h"
using namespace llvm37;

int main() {
  LLVM37Context Context;

  // Create the "module" or "program" or "translation unit" to hold the
  // function
  Module *M = new Module("test", Context);

  // Create the main function: first create the type 'int ()'
  FunctionType *FT =
    FunctionType::get(Type::getInt32Ty(Context), /*not vararg*/false);

  // By passing a module as the last parameter to the Function constructor,
  // it automatically gets appended to the Module.
  Function *F = Function::Create(FT, Function::ExternalLinkage, "main", M);

  // Add a basic block to the function... again, it automatically inserts
  // because of the last argument.
  BasicBlock *BB = BasicBlock::Create(Context, "EntryBlock", F);

  // Get pointers to the constant integers...
  Value *Two = ConstantInt::get(Type::getInt32Ty(Context), 2);
  Value *Three = ConstantInt::get(Type::getInt32Ty(Context), 3);

  // Create the add instruction... does not insert...
  Instruction *Add = BinaryOperator::Create(Instruction::Add, Two, Three,
                                            "addresult");

  // explicitly insert it into the basic block...
  BB->getInstList().push_back(Add);

  // Create the return instruction and add it to the basic block
  BB->getInstList().push_back(ReturnInst::Create(Context, Add));

  // Output the bitcode file to stdout
  WriteBitcodeToFile(M, outs());

  // Delete the module and all of its contents.
  delete M;
  return 0;
}
