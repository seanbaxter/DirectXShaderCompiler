set(LLVM37_LINK_COMPONENTS
  Core
  Support
  IPO
  )

add_llvm37_unittest(IPOTests
  LowerBitSets.cpp
  )
