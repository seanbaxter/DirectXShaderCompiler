add_llvm_library(LLVM37InstCombine
  InstructionCombining.cpp
  InstCombineAddSub.cpp
  InstCombineAndOrXor.cpp
  InstCombineCalls.cpp
  InstCombineCasts.cpp
  InstCombineCompares.cpp
  InstCombineLoadStoreAlloca.cpp
  InstCombineMulDivRem.cpp
  InstCombinePHI.cpp
  InstCombineSelect.cpp
  InstCombineShifts.cpp
  InstCombineSimplifyDemanded.cpp
  InstCombineVectorOps.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/Transforms
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/Transforms/InstCombine
  )

add_dependencies(LLVM37InstCombine intrinsics_gen)
