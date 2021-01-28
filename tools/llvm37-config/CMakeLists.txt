set(LLVM37_LINK_COMPONENTS support)

set(BUILDVARIABLES_SRCPATH ${CMAKE_CURRENT_SOURCE_DIR}/BuildVariables.inc.in)
set(BUILDVARIABLES_OBJPATH ${CMAKE_CURRENT_BINARY_DIR}/BuildVariables.inc)

# Add the llvm-config tool.
add_llvm37_tool(llvm-config
  llvm-config.cpp
  )

# Compute the substitution values for various items.
get_property(LLVM37_SYSTEM_LIBS_LIST TARGET LLVM37Support PROPERTY LLVM37_SYSTEM_LIBS)
foreach(l ${LLVM37_SYSTEM_LIBS_LIST})
  set(SYSTEM_LIBS ${SYSTEM_LIBS} "-l${l}")
endforeach()
string(REPLACE ";" " " SYSTEM_LIBS "${SYSTEM_LIBS}")

# Fetch target specific compile options, e.g. RTTI option
get_property(COMPILE_FLAGS TARGET llvm-config PROPERTY COMPILE_FLAGS)

# Use configure_file to create BuildVariables.inc.
set(LLVM37_SRC_ROOT ${LLVM37_MAIN_SRC_DIR})
set(LLVM37_OBJ_ROOT ${LLVM37_BINARY_DIR})
set(LLVM37_CPPFLAGS "${CMAKE_CPP_FLAGS} ${CMAKE_CPP_FLAGS_${uppercase_CMAKE_BUILD_TYPE}} ${LLVM37_DEFINITIONS}")
set(LLVM37_CFLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${uppercase_CMAKE_BUILD_TYPE}} ${LLVM37_DEFINITIONS}")
set(LLVM37_CXXFLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${uppercase_CMAKE_BUILD_TYPE}} ${COMPILE_FLAGS} ${LLVM37_DEFINITIONS}")
# Use the C++ link flags, since they should be a superset of C link flags.
set(LLVM37_LDFLAGS "${CMAKE_CXX_LINK_FLAGS}")
set(LLVM37_BUILDMODE ${CMAKE_BUILD_TYPE})
set(LLVM37_SYSTEM_LIBS ${SYSTEM_LIBS})
string(REPLACE ";" " " LLVM37_TARGETS_BUILT "${LLVM37_TARGETS_TO_BUILD}")
configure_file(${BUILDVARIABLES_SRCPATH} ${BUILDVARIABLES_OBJPATH} @ONLY)

# Set build-time environment(s).
add_definitions(-DCMAKE_CFG_INTDIR="${CMAKE_CFG_INTDIR}")

# Add the dependency on the generation step.
add_file_dependencies(${CMAKE_CURRENT_SOURCE_DIR}/llvm-config.cpp ${BUILDVARIABLES_OBJPATH})

if(CMAKE_CROSSCOMPILING)
  set(${project}_LLVM37_CONFIG_EXE "${LLVM37_NATIVE_BUILD}/bin/llvm-config")
  set(${project}_LLVM37_CONFIG_EXE ${${project}_LLVM37_CONFIG_EXE} PARENT_SCOPE)

  add_custom_command(OUTPUT "${${project}_LLVM37_CONFIG_EXE}"
    COMMAND ${CMAKE_COMMAND} --build . --target llvm-config --config $<CONFIGURATION>
    DEPENDS ${LLVM37_NATIVE_BUILD}/CMakeCache.txt
    WORKING_DIRECTORY ${LLVM37_NATIVE_BUILD}
    COMMENT "Building native llvm-config...")
  add_custom_target(${project}NativeLLVMConfig DEPENDS ${${project}_LLVM37_CONFIG_EXE})
  add_dependencies(${project}NativeLLVMConfig CONFIGURE_LLVM37_NATIVE)

  add_dependencies(llvm-config ${project}NativeLLVMConfig)
endif(CMAKE_CROSSCOMPILING)
