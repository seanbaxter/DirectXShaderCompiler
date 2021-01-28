set(LLVM37_LINK_COMPONENTS
  AsmParser
  BitReader
  BitWriter
  Core
  Support
  )

add_llvm37_unittest(BitcodeTests
  BitReaderTest.cpp
  BitstreamReaderTest.cpp
  )
