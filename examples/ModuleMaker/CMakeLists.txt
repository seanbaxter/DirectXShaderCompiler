set(LLVM37_LINK_COMPONENTS
  BitWriter
  Core
  Support
  )

add_llvm_example(ModuleMaker
  ModuleMaker.cpp
  )
