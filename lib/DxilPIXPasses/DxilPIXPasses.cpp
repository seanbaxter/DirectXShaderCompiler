///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilPIXPasses.cpp                                                         //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// Provides SetupRegistryPassForPIX.                                         //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "dxc/DxilPIXPasses/DxilPIXPasses.h"
#include "dxc/Support/Global.h"
#include "dxc/Support/WinIncludes.h"

#include "llvm37/IR/LegacyPassManager.h"
#include "llvm37/Pass.h"
#include "llvm37/PassInfo.h"
#include "llvm37/Support/SourceMgr.h"
#include "llvm37/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm37;
using namespace hlsl;

namespace hlsl {

HRESULT SetupRegistryPassForPIX() {
  try {
    PassRegistry &Registry = *PassRegistry::getPassRegistry();
    /* <py::lines('INIT-PASSES')>hctdb_instrhelp.get_init_passes(set(["pix"]))</py>*/
    // INIT-PASSES:BEGIN
    initializeDxilAddPixelHitInstrumentationPass(Registry);
    initializeDxilAnnotateWithVirtualRegisterPass(Registry);
    initializeDxilDbgValueToDbgDeclarePass(Registry);
    initializeDxilDebugInstrumentationPass(Registry);
    initializeDxilForceEarlyZPass(Registry);
    initializeDxilOutputColorBecomesConstantPass(Registry);
    initializeDxilPIXMeshShaderOutputInstrumentationPass(Registry);
    initializeDxilReduceMSAAToSingleSamplePass(Registry);
    initializeDxilRemoveDiscardsPass(Registry);
    initializeDxilShaderAccessTrackingPass(Registry);
    // INIT-PASSES:END
  }
  CATCH_CPP_RETURN_HRESULT();
  return S_OK;
}

} // namespace hlsl
