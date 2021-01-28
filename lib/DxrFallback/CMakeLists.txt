add_llvm_library(LLVM37DxrFallback
  DxrFallbackCompiler.cpp
  FunctionBuilder.h
  LiveValues.cpp
  LiveValues.h
  LLVMUtils.cpp
  LLVMUtils.h
  Reducibility.h
  Reducibility.cpp
  StateFunctionTransform.cpp
  StateFunctionTransform.h
)

add_dependencies(LLVM37DxrFallback intrinsics_gen)
