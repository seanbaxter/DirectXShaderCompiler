set(LLVM37_LINK_COMPONENTS
  BitReader
  Core
  Support
  MSSupport # HLSL Change
  )

add_llvm37_tool(llvm37-dis
  llvm37-dis.cpp
  )

# HLSL Change Starts
if (NOT HLSL_OPTIONAL_PROJS_IN_DEFAULT)
  set_target_properties(llvm37-dis PROPERTIES EXCLUDE_FROM_ALL ON EXCLUDE_FROM_DEFAULT_BUILD ON)
endif ()
# HLSL Change Ends
