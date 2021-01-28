add_llvm_library(LLVM37ipo
  ArgumentPromotion.cpp
  BarrierNoopPass.cpp
  ConstantMerge.cpp
  DeadArgumentElimination.cpp
  ElimAvailExtern.cpp
  ExtractGV.cpp
  FunctionAttrs.cpp
  GlobalDCE.cpp
  GlobalOpt.cpp
  IPConstantPropagation.cpp
  IPO.cpp
  InlineAlways.cpp
  InlineSimple.cpp
  Inliner.cpp
  Internalize.cpp
  LoopExtractor.cpp
  LowerBitSets.cpp
  MergeFunctions.cpp
  PartialInlining.cpp
  PassManagerBuilder.cpp
  PruneEH.cpp
  StripDeadPrototypes.cpp
  StripSymbols.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/Transforms
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/Transforms/IPO
  )

add_dependencies(LLVM37ipo intrinsics_gen)
