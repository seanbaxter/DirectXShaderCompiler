//===-- CGBuilder.h - Choose IRBuilder implementation  ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_LIB_CODEGEN_CGBUILDER_H
#define LLVM37_CLANG_LIB_CODEGEN_CGBUILDER_H

#include "llvm37/IR/IRBuilder.h"

namespace clang {
namespace CodeGen {

class CodeGenFunction;

/// \brief This is an IRBuilder insertion helper that forwards to
/// CodeGenFunction::InsertHelper, which adds necessary metadata to
/// instructions.
template <bool PreserveNames>
class CGBuilderInserter
  : protected llvm37::IRBuilderDefaultInserter<PreserveNames> {
public:
  CGBuilderInserter() : CGF(nullptr) {}
  explicit CGBuilderInserter(CodeGenFunction *CGF) : CGF(CGF) {}

protected:
  /// \brief This forwards to CodeGenFunction::InsertHelper.
  void InsertHelper(llvm37::Instruction *I, const llvm37::Twine &Name,
                    llvm37::BasicBlock *BB,
                    llvm37::BasicBlock::iterator InsertPt) const;
private:
  void operator=(const CGBuilderInserter &) = delete;

  CodeGenFunction *CGF;
};

// Don't preserve names on values in an optimized build.
#ifdef NDEBUG
#define PreserveNames false
#else
#define PreserveNames true
#endif
typedef CGBuilderInserter<PreserveNames> CGBuilderInserterTy;
typedef llvm37::IRBuilder<PreserveNames, llvm37::ConstantFolder,
                        CGBuilderInserterTy> CGBuilderTy;
#undef PreserveNames

}  // end namespace CodeGen
}  // end namespace clang

#endif
