set(HLSL_IGNORE_SOURCES BBVectorize.cpp LoopVectorize.cpp SLPVectorizer.cpp)

add_llvm_library(LLVM37Vectorize
  Vectorize.cpp
#  BBVectorize.cpp
#  LoopVectorize.cpp
#  SLPVectorizer.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/Transforms
  )

add_dependencies(LLVM37Vectorize intrinsics_gen)
