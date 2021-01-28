add_llvm_library(LLVM37ObjCARCOpts
  ObjCARC.cpp
  ObjCARCOpts.cpp
  ObjCARCExpand.cpp
  ObjCARCAPElim.cpp
  ObjCARCAliasAnalysis.cpp
  ARCInstKind.cpp
  ObjCARCContract.cpp
  DependencyAnalysis.cpp
  ProvenanceAnalysis.cpp
  ProvenanceAnalysisEvaluator.cpp
  PtrState.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/Transforms
  )

add_dependencies(LLVM37ObjCARCOpts intrinsics_gen)
