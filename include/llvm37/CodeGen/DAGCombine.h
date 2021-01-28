//===-- llvm37/CodeGen/DAGCombine.h  ------- SelectionDAG Nodes ---*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//

#ifndef LLVM37_CODEGEN_DAGCOMBINE_H
#define LLVM37_CODEGEN_DAGCOMBINE_H

namespace llvm37 {

enum CombineLevel {
  BeforeLegalizeTypes,
  AfterLegalizeTypes,
  AfterLegalizeVectorOps,
  AfterLegalizeDAG
};

} // end llvm37 namespace

#endif
