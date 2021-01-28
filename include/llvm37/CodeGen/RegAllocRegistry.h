//===-- llvm37/CodeGen/RegAllocRegistry.h -------------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the implementation for register allocator function
// pass registry (RegisterRegAlloc).
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CODEGEN_REGALLOCREGISTRY_H
#define LLVM37_CODEGEN_REGALLOCREGISTRY_H

#include "llvm37/CodeGen/MachinePassRegistry.h"

namespace llvm37 {

//===----------------------------------------------------------------------===//
///
/// RegisterRegAlloc class - Track the registration of register allocators.
///
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
class RegisterRegAlloc : public MachinePassRegistryNode {

public:

  typedef FunctionPass *(*FunctionPassCtor)();

  static MachinePassRegistry Registry;

  RegisterRegAlloc(const char *N, const char *D, FunctionPassCtor C)
  : MachinePassRegistryNode(N, D, (MachinePassCtor)C)
  { 
     Registry.Add(this); 
  }
  ~RegisterRegAlloc() { Registry.Remove(this); }
  

  // Accessors.
  //
  RegisterRegAlloc *getNext() const {
    return (RegisterRegAlloc *)MachinePassRegistryNode::getNext();
  }
  static RegisterRegAlloc *getList() {
    return (RegisterRegAlloc *)Registry.getList();
  }
  static FunctionPassCtor getDefault() {
    return (FunctionPassCtor)Registry.getDefault();
  }
  static void setDefault(FunctionPassCtor C) {
    Registry.setDefault((MachinePassCtor)C);
  }
  static void setListener(MachinePassRegistryListener *L) {
    Registry.setListener(L);
  }
  
};

} // end namespace llvm37


#endif
