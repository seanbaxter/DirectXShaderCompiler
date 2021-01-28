set(LLVM37_LINK_COMPONENTS
  Core
  IRReader
  Support
  )

add_llvm37_tool(llvm37-diff
  llvm37-diff.cpp
  DiffConsumer.cpp
  DiffLog.cpp
  DifferenceEngine.cpp
  )
