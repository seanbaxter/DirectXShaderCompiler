///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilPixPasses.h                                                           //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// This file provides a DXIL passes to support PIX.                          //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once

namespace llvm37 {
class ModulePass;
class PassRegistry;

ModulePass *createDxilAddPixelHitInstrumentationPass();
ModulePass *createDxilDbgValueToDbgDeclarePass();
ModulePass *createDxilAnnotateWithVirtualRegisterPass();
ModulePass *createDxilOutputColorBecomesConstantPass();
ModulePass *createDxilDxilPIXMeshShaderOutputInstrumentation();
ModulePass *createDxilRemoveDiscardsPass();
ModulePass *createDxilReduceMSAAToSingleSamplePass();
ModulePass *createDxilForceEarlyZPass();
ModulePass *createDxilDebugInstrumentationPass();
ModulePass *createDxilShaderAccessTrackingPass();

void initializeDxilAddPixelHitInstrumentationPass(llvm37::PassRegistry&);
void initializeDxilDbgValueToDbgDeclarePass(llvm37::PassRegistry&);
void initializeDxilAnnotateWithVirtualRegisterPass(llvm37::PassRegistry&);
void initializeDxilOutputColorBecomesConstantPass(llvm37::PassRegistry&);
void initializeDxilPIXMeshShaderOutputInstrumentationPass(llvm37::PassRegistry &);
void initializeDxilRemoveDiscardsPass(llvm37::PassRegistry&);
void initializeDxilReduceMSAAToSingleSamplePass(llvm37::PassRegistry&);
void initializeDxilForceEarlyZPass(llvm37::PassRegistry&);
void initializeDxilDebugInstrumentationPass(llvm37::PassRegistry&);
void initializeDxilShaderAccessTrackingPass(llvm37::PassRegistry&);

}
