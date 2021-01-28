///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilGenerationPass.h                                                      //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// This file provides a DXIL Generation pass.                                //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once

namespace llvm37 {
class Module;
class ModulePass;
class Function;
class FunctionPass;
class Instruction;
class PassRegistry;
class StringRef;
struct PostDominatorTree;
}

namespace hlsl {
class DxilResourceBase;
class WaveSensitivityAnalysis {
public:
  static WaveSensitivityAnalysis* create(llvm37::PostDominatorTree &PDT);
  virtual ~WaveSensitivityAnalysis() { }
  virtual void Analyze(llvm37::Function *F) = 0;
  virtual bool IsWaveSensitive(llvm37::Instruction *op) = 0;
};

class HLSLExtensionsCodegenHelper;

// Pause/resume support.
bool ClearPauseResumePasses(llvm37::Module &M); // true if modified; false if missing
void GetPauseResumePasses(llvm37::Module &M, llvm37::StringRef &pause, llvm37::StringRef &resume);
void SetPauseResumePasses(llvm37::Module &M, llvm37::StringRef pause, llvm37::StringRef resume);
}

namespace llvm37 {

/// \brief Create and return a pass that tranform the module into a DXIL module
/// Note that this pass is designed for use with the legacy pass manager.
ModulePass *createDxilLowerCreateHandleForLibPass();
ModulePass *createDxilAllocateResourcesForLibPass();
ModulePass *createDxilEliminateOutputDynamicIndexingPass();
ModulePass *createDxilGenerationPass(bool NotOptimized, hlsl::HLSLExtensionsCodegenHelper *extensionsHelper);
ModulePass *createHLEmitMetadataPass();
ModulePass *createHLEnsureMetadataPass();
ModulePass *createDxilFinalizeModulePass();
ModulePass *createDxilEmitMetadataPass();
FunctionPass *createDxilExpandTrigIntrinsicsPass();
ModulePass *createDxilConvergentMarkPass();
ModulePass *createDxilConvergentClearPass();
ModulePass *createDxilDeadFunctionEliminationPass();
ModulePass *createHLDeadFunctionEliminationPass();
ModulePass *createHLPreprocessPass();
ModulePass *createDxilPrecisePropagatePass();
FunctionPass *createDxilPreserveAllOutputsPass();
FunctionPass *createDxilPromoteLocalResources();
ModulePass *createDxilPromoteStaticResources();
ModulePass *createDxilLegalizeResources();
ModulePass *createDxilLegalizeEvalOperationsPass();
FunctionPass *createDxilLegalizeSampleOffsetPass();
FunctionPass *createDxilSimpleGVNHoistPass();
ModulePass *createInvalidateUndefResourcesPass();
FunctionPass *createSimplifyInstPass();
ModulePass *createDxilTranslateRawBuffer();
ModulePass *createNoPausePassesPass();
ModulePass *createPausePassesPass();
ModulePass *createResumePassesPass();
FunctionPass *createMatrixBitcastLowerPass();
ModulePass *createDxilCleanupAddrSpaceCastPass();
ModulePass *createDxilRenameResourcesPass();

void initializeDxilLowerCreateHandleForLibPass(llvm37::PassRegistry&);
void initializeDxilAllocateResourcesForLibPass(llvm37::PassRegistry&);
void initializeDxilEliminateOutputDynamicIndexingPass(llvm37::PassRegistry&);
void initializeDxilGenerationPassPass(llvm37::PassRegistry&);
void initializeHLEnsureMetadataPass(llvm37::PassRegistry&);
void initializeHLEmitMetadataPass(llvm37::PassRegistry&);
void initializeDxilFinalizeModulePass(llvm37::PassRegistry&);
void initializeDxilEmitMetadataPass(llvm37::PassRegistry&);
void initializeDxilEraseDeadRegionPass(llvm37::PassRegistry&);
void initializeDxilExpandTrigIntrinsicsPass(llvm37::PassRegistry&);
void initializeDxilDeadFunctionEliminationPass(llvm37::PassRegistry&);
void initializeHLDeadFunctionEliminationPass(llvm37::PassRegistry&);
void initializeHLPreprocessPass(llvm37::PassRegistry&);
void initializeDxilConvergentMarkPass(llvm37::PassRegistry&);
void initializeDxilConvergentClearPass(llvm37::PassRegistry&);
void initializeDxilPrecisePropagatePassPass(llvm37::PassRegistry&);
void initializeDxilPreserveAllOutputsPass(llvm37::PassRegistry&);
void initializeDxilPromoteLocalResourcesPass(llvm37::PassRegistry&);
void initializeDxilPromoteStaticResourcesPass(llvm37::PassRegistry&);
void initializeDxilLegalizeResourcesPass(llvm37::PassRegistry&);
void initializeDxilLegalizeEvalOperationsPass(llvm37::PassRegistry&);
void initializeDxilLegalizeSampleOffsetPassPass(llvm37::PassRegistry&);
void initializeDxilSimpleGVNHoistPass(llvm37::PassRegistry&);
void initializeInvalidateUndefResourcesPass(llvm37::PassRegistry&);
void initializeSimplifyInstPass(llvm37::PassRegistry&);
void initializeDxilTranslateRawBufferPass(llvm37::PassRegistry&);
void initializeNoPausePassesPass(llvm37::PassRegistry&);
void initializePausePassesPass(llvm37::PassRegistry&);
void initializeResumePassesPass(llvm37::PassRegistry&);
void initializeMatrixBitcastLowerPassPass(llvm37::PassRegistry&);
void initializeDxilCleanupAddrSpaceCastPass(llvm37::PassRegistry&);
void initializeDxilRenameResourcesPass(llvm37::PassRegistry&);

ModulePass *createDxilValidateWaveSensitivityPass();
void initializeDxilValidateWaveSensitivityPass(llvm37::PassRegistry&);

FunctionPass *createCleanupDxBreakPass();
void initializeCleanupDxBreakPass(llvm37::PassRegistry&);

FunctionPass *createDxilLoopDeletionPass();
void initializeDxilLoopDeletionPass(llvm37::PassRegistry &);

ModulePass *createHLLegalizeParameter();
void initializeHLLegalizeParameterPass(llvm37::PassRegistry &);

bool AreDxilResourcesDense(llvm37::Module *M, hlsl::DxilResourceBase **ppNonDense);

ModulePass *createDxilNoOptLegalizePass();
void initializeDxilNoOptLegalizePass(llvm37::PassRegistry&);

ModulePass *createDxilNoOptSimplifyInstructionsPass();
void initializeDxilNoOptSimplifyInstructionsPass(llvm37::PassRegistry&);

ModulePass *createDxilMutateResourceToHandlePass();
void initializeDxilMutateResourceToHandlePass(llvm37::PassRegistry&);

}
