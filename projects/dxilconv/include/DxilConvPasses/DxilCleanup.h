///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilCleanup.cpp                                                           //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// Optimization of DXIL after conversion from DXBC.                          //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once

namespace llvm37 {
class PassRegistry;
class ModulePass;

extern char &DxilCleanupID;

llvm37::ModulePass *createDxilCleanupPass();

void initializeDxilCleanupPass(llvm37::PassRegistry&);

}
