set(LLVM37_LINK_COMPONENTS
  Core
  Support
  TransformUtils
  )

add_llvm37_unittest(UtilsTests
  ASanStackFrameLayoutTest.cpp
  Cloning.cpp
  IntegerDivision.cpp
  Local.cpp
  ValueMapperTest.cpp
  )
