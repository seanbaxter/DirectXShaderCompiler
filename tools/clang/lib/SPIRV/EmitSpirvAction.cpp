//===--- EmitSpirvAction.cpp - EmitSPIRVAction implementation -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/SPIRV/EmitSpirvAction.h"

#include "SpirvEmitter.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm37/ADT/STLExtras.h"

namespace clang {

std::unique_ptr<ASTConsumer>
EmitSpirvAction::CreateASTConsumer(CompilerInstance &CI, StringRef InFile) {
  return llvm37::make_unique<spirv::SpirvEmitter>(CI);
}
} // end namespace clang
