# FIXME: As we plan to execute llvm37-symbolizer binary from compiler-rt
# libraries, it has to be compiled for all supported targets (x86_64, i386 etc).
# This means that we need LLVM37 libraries to be compiled for these
# targets as well. Currently, there is no support for such a build strategy.

set(LLVM37_LINK_COMPONENTS
  DebugInfoDWARF
  DebugInfoPDB
  Object
  Support
  )

add_llvm37_tool(llvm37-symbolizer
  LLVM37Symbolize.cpp
  llvm37-symbolizer.cpp
  )
