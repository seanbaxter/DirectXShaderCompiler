set(LLVM37_LINK_COMPONENTS
  Core
  ProfileData
  Support
  )

add_llvm37_tool(llvm37-profdata
  llvm37-profdata.cpp
  )
