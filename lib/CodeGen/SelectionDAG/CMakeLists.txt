set(LLVM37_OPTIONAL_SOURCES legalizevectorops.cpp) # HLSL Change - ignore file

add_llvm_library(LLVM37SelectionDAG
  DAGCombiner.cpp
  FastISel.cpp
  FunctionLoweringInfo.cpp
  InstrEmitter.cpp
  LegalizeDAG.cpp
  LegalizeFloatTypes.cpp
  LegalizeIntegerTypes.cpp
  LegalizeTypes.cpp
  LegalizeTypesGeneric.cpp
  LegalizeVectorOps.cpp
  LegalizeVectorTypes.cpp
  ResourcePriorityQueue.cpp
  ScheduleDAGFast.cpp
  ScheduleDAGRRList.cpp
  ScheduleDAGSDNodes.cpp
  SelectionDAG.cpp
  SelectionDAGBuilder.cpp
  SelectionDAGDumper.cpp
  SelectionDAGISel.cpp
  SelectionDAGPrinter.cpp
  StatepointLowering.cpp
  ScheduleDAGVLIW.cpp
  TargetLowering.cpp
  TargetSelectionDAGInfo.cpp
  )

add_dependencies(LLVM37SelectionDAG intrinsics_gen)
