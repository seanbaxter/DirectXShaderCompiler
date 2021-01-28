add_llvm_library(LLVM37ScalarOpts
  ADCE.cpp
  AlignmentFromAssumptions.cpp
  BDCE.cpp
  ConstantHoisting.cpp
  ConstantProp.cpp
  CorrelatedValuePropagation.cpp
  DCE.cpp
  DeadStoreElimination.cpp
  EarlyCSE.cpp
  FlattenCFGPass.cpp
  Float2Int.cpp
  GVN.cpp
  HoistConstantArray.cpp
  InductiveRangeCheckElimination.cpp
  IndVarSimplify.cpp
  JumpThreading.cpp
  LICM.cpp
  LoadCombine.cpp
  LoopDeletion.cpp
  LoopDistribute.cpp
  LoopIdiomRecognize.cpp
  LoopInstSimplify.cpp
  LoopInterchange.cpp
  LoopRerollPass.cpp
  LoopRotation.cpp
  LoopStrengthReduce.cpp
  LoopUnrollPass.cpp
  LoopUnswitch.cpp
  LowerAtomic.cpp
  LowerExpectIntrinsic.cpp
  LowerTypePasses.cpp
  MemCpyOptimizer.cpp
  MergedLoadStoreMotion.cpp
  NaryReassociate.cpp
  PartiallyInlineLibCalls.cpp
  PlaceSafepoints.cpp
  Reassociate.cpp
  Reg2Mem.cpp
  Reg2MemHLSL.cpp
  RewriteStatepointsForGC.cpp
  SCCP.cpp
  SROA.cpp
  SampleProfile.cpp
  Scalar.cpp
  ScalarReplAggregates.cpp
  ScalarReplAggregatesHLSL.cpp  # HLSL Change
  DxilLoopUnroll.cpp # HLSL Change
  DxilRemoveDeadBlocks.cpp # HLSL Change
  DxilEraseDeadRegion.cpp # HLSL Change
  DxilFixConstArrayInitializer.cpp # HLSL Change
  DxilEliminateVector.cpp # HLSL Change
  DxilConditionalMem2Reg.cpp # HLSL Change
  DxilRemoveUnstructuredLoopExits.cpp # HLSL Change
  Scalarizer.cpp
  SeparateConstOffsetFromGEP.cpp
  SimplifyCFGPass.cpp
  Sink.cpp
  SpeculativeExecution.cpp
  StraightLineStrengthReduce.cpp
  StructurizeCFG.cpp
  TailRecursionElimination.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/Transforms
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/Transforms/Scalar
  )

add_dependencies(LLVM37ScalarOpts intrinsics_gen)
