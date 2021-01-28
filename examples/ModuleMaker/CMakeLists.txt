set(LLVM37_LINK_COMPONENTS
  BitWriter
  Core
  Support
  )

add_llvm37_example(ModuleMaker
  ModuleMaker.cpp
  )
