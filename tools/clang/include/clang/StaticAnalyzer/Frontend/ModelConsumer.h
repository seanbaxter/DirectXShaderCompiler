//===-- ModelConsumer.h -----------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file implements clang::ento::ModelConsumer which is an
/// ASTConsumer for model files.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_GR_MODELCONSUMER_H
#define LLVM37_CLANG_GR_MODELCONSUMER_H

#include "clang/AST/ASTConsumer.h"
#include "llvm37/ADT/StringMap.h"

namespace clang {

class Stmt;

namespace ento {

/// \brief ASTConsumer to consume model files' AST.
///
/// This consumer collects the bodies of function definitions into a StringMap
/// from a model file.
class ModelConsumer : public ASTConsumer {
public:
  ModelConsumer(llvm37::StringMap<Stmt *> &Bodies);

  bool HandleTopLevelDecl(DeclGroupRef D) override;

private:
  llvm37::StringMap<Stmt *> &Bodies;
};
}
}

#endif
