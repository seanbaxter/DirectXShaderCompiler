
# Build polly before the tools: the tools link against polly when
# LINK_POLLY_INTO_TOOLS is set.
if(WITH_POLLY)
  add_llvm37_external_project(polly)
else(WITH_POLLY)
  list(APPEND LLVM37_IMPLICIT_PROJECT_IGNORE "${LLVM37_MAIN_SRC_DIR}/tools/polly")
endif(WITH_POLLY)

if( LLVM37_BUILD_LLVM37_DYLIB )
  add_llvm37_tool_subdirectory(llvm37-shlib)
else()
  ignore_llvm37_tool_subdirectory(llvm37-shlib)
endif()

if (HLSL_BUILD_DXILCONV) # HLSL Change
add_llvm37_tool_subdirectory(opt) # HLSL Change
endif()

# add_llvm37_tool_subdirectory(llvm37-as) # HLSL Change
# add_llvm37_tool_subdirectory(llvm37-dis) # HLSL Change
# add_llvm37_tool_subdirectory(llvm37-mc) # HLSL Change

# HLSL Change Begins
if (WIN32)
# This target can currently only be built on Windows.
add_llvm37_tool_subdirectory(dxexp)
endif (WIN32)
# HLSL Change ends

# add_llvm37_tool_subdirectory(llc) # HLSL Change
# add_llvm37_tool_subdirectory(llvm37-ar) # HLSL Change
# add_llvm37_tool_subdirectory(llvm37-nm) # HLSL Change
# add_llvm37_tool_subdirectory(llvm37-size) # HLSL Change

# add_llvm37_tool_subdirectory(llvm37-cov) # HLSL Change
# add_llvm37_tool_subdirectory(llvm37-profdata) # HLSL Change
# add_llvm37_tool_subdirectory(llvm37-link) # HLSL Change
# add_llvm37_tool_subdirectory(lli) # HLSL Change

# add_llvm37_tool_subdirectory(llvm37-extract) # HLSL Change
# add_llvm37_tool_subdirectory(llvm37-diff) # HLSL Change
# add_llvm37_tool_subdirectory(macho-dump) # HLSL Change
# add_llvm37_tool_subdirectory(llvm37-objdump) # HLSL Change
# add_llvm37_tool_subdirectory(llvm37-readobj) # HLSL Change
# add_llvm37_tool_subdirectory(llvm37-rtdyld) # HLSL Change
# add_llvm37_tool_subdirectory(llvm37-dwarfdump) # HLSL Change
# add_llvm37_tool_subdirectory(dsymutil) # HLSL Change
# add_llvm37_tool_subdirectory(llvm37-cxxdump) # HLSL Change
# HLSL Change - remove llvm37-jitlistener conditional on LLVM37_USE_INTEL_JITEVENTS

# add_llvm37_tool_subdirectory(bugpoint) # HLSL Change
# add_llvm37_tool_subdirectory(bugpoint-passes) # HLSL Change
# add_llvm37_tool_subdirectory(llvm37-bcanalyzer) # HLSL Change
# add_llvm37_tool_subdirectory(llvm37-stress) # HLSL Change
# add_llvm37_tool_subdirectory(llvm37-mcmarkup) # HLSL Change

# add_llvm37_tool_subdirectory(verify-uselistorder) # HLSL Change

# add_llvm37_tool_subdirectory(llvm37-symbolizer) # HLSL Change

# add_llvm37_tool_subdirectory(llvm37-c-test) # HLSL Change

# add_llvm37_tool_subdirectory(obj2yaml) # HLSL Change
# add_llvm37_tool_subdirectory(yaml2obj) # HLSL Change

# add_llvm37_tool_subdirectory(llvm37-go) # HLSL Change

# add_llvm37_tool_subdirectory(llvm37-pdbdump) # HLSL Change

if(NOT CYGWIN AND LLVM37_ENABLE_PIC)
#   add_llvm37_tool_subdirectory(lto) # HLSL Change
#   add_llvm37_tool_subdirectory(llvm37-lto) # HLSL Change
else()
  ignore_llvm37_tool_subdirectory(lto)
  ignore_llvm37_tool_subdirectory(llvm37-lto)
endif()

# add_llvm37_tool_subdirectory(gold) # HLSL Change

add_llvm37_external_project(clang)
# add_llvm37_external_project(llgo) # HLSL Change
# add_llvm37_external_project(lld) # HLSL Change
# add_llvm37_external_project(lldb) # HLSL Change

# Automatically add remaining sub-directories containing a 'CMakeLists.txt'
# file as external projects.
# add_llvm37_implicit_external_projects() # HLSL Change

set(LLVM37_COMMON_DEPENDS ${LLVM37_COMMON_DEPENDS} PARENT_SCOPE)
