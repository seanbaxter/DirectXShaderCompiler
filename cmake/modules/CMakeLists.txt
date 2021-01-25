set(LLVM37_INSTALL_PACKAGE_DIR share/llvm/cmake)
set(llvm_cmake_builddir "${LLVM37_BINARY_DIR}/${LLVM37_INSTALL_PACKAGE_DIR}")

get_property(LLVM37_EXPORTS GLOBAL PROPERTY LLVM37_EXPORTS)
export(TARGETS ${LLVM37_EXPORTS}
  FILE ${llvm_cmake_builddir}/LLVMExports.cmake)

get_property(LLVM37_AVAILABLE_LIBS GLOBAL PROPERTY LLVM37_LIBS)

foreach(lib ${LLVM37_AVAILABLE_LIBS})
  get_property(llvm_lib_deps GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_${lib})
  set(all_llvm_lib_deps
    "${all_llvm_lib_deps}\nset_property(GLOBAL PROPERTY LLVMBUILD_LIB_DEPS_${lib} ${llvm_lib_deps})")
endforeach(lib)

# Generate LLVMConfig.cmake for the build tree.
set(LLVM37_CONFIG_CODE "
# LLVM37_BUILD_* values available only from LLVM37 build tree.
set(LLVM37_BUILD_BINARY_DIR \"${LLVM37_BINARY_DIR}\")
set(LLVM37_BUILD_LIBRARY_DIR \"${LLVM37_LIBRARY_DIR}\")
set(LLVM37_BUILD_MAIN_INCLUDE_DIR \"${LLVM37_MAIN_INCLUDE_DIR}\")
set(LLVM37_BUILD_MAIN_SRC_DIR \"${LLVM37_MAIN_SRC_DIR}\")
")
set(LLVM37_CONFIG_INCLUDE_DIRS
  "${LLVM37_MAIN_INCLUDE_DIR}"
  "${LLVM37_INCLUDE_DIR}"
  )
set(LLVM37_CONFIG_LIBRARY_DIRS
  "${LLVM37_LIBRARY_DIR}"
  )
set(LLVM37_CONFIG_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(LLVM37_CONFIG_TOOLS_BINARY_DIR "${LLVM37_TOOLS_BINARY_DIR}")
set(LLVM37_CONFIG_EXPORTS_FILE "${llvm_cmake_builddir}/LLVMExports.cmake")
configure_file(
  LLVMConfig.cmake.in
  ${llvm_cmake_builddir}/LLVMConfig.cmake
  @ONLY)

# For compatibility with projects that include(LLVMConfig)
# via CMAKE_MODULE_PATH, place API modules next to it.
# This should be removed in the future.
file(COPY .
  DESTINATION ${llvm_cmake_builddir}
  FILES_MATCHING PATTERN *.cmake
  PATTERN .svn EXCLUDE
  PATTERN CMakeFiles EXCLUDE
  )

# Generate LLVMConfig.cmake for the install tree.
set(LLVM37_CONFIG_CODE "
# Compute the installation prefix from this LLVMConfig.cmake file location.
get_filename_component(LLVM37_INSTALL_PREFIX \"\${CMAKE_CURRENT_LIST_FILE}\" PATH)")
# Construct the proper number of get_filename_component(... PATH)
# calls to compute the installation prefix.
string(REGEX REPLACE "/" ";" _count "${LLVM37_INSTALL_PACKAGE_DIR}")
foreach(p ${_count})
  set(LLVM37_CONFIG_CODE "${LLVM37_CONFIG_CODE}
get_filename_component(LLVM37_INSTALL_PREFIX \"\${LLVM37_INSTALL_PREFIX}\" PATH)")
endforeach(p)
set(LLVM37_CONFIG_INCLUDE_DIRS "\${LLVM37_INSTALL_PREFIX}/include")
set(LLVM37_CONFIG_LIBRARY_DIRS "\${LLVM37_INSTALL_PREFIX}/lib\${LLVM37_LIBDIR_SUFFIX}")
set(LLVM37_CONFIG_CMAKE_DIR "\${LLVM37_INSTALL_PREFIX}/${LLVM37_INSTALL_PACKAGE_DIR}")
set(LLVM37_CONFIG_TOOLS_BINARY_DIR "\${LLVM37_INSTALL_PREFIX}/bin")
set(LLVM37_CONFIG_EXPORTS_FILE "\${LLVM37_CMAKE_DIR}/LLVMExports.cmake")
configure_file(
  LLVMConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/LLVMConfig.cmake
  @ONLY)

# Generate LLVMConfigVersion.cmake for build and install tree.
configure_file(
  LLVMConfigVersion.cmake.in
  ${llvm_cmake_builddir}/LLVMConfigVersion.cmake
  @ONLY)

if (NOT LLVM37_INSTALL_TOOLCHAIN_ONLY)
  install(EXPORT LLVMExports DESTINATION ${LLVM37_INSTALL_PACKAGE_DIR})

  install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/LLVMConfig.cmake
    ${llvm_cmake_builddir}/LLVMConfigVersion.cmake
    LLVM37-Config.cmake
    DESTINATION ${LLVM37_INSTALL_PACKAGE_DIR})

  install(DIRECTORY .
    DESTINATION ${LLVM37_INSTALL_PACKAGE_DIR}
    FILES_MATCHING PATTERN *.cmake
    PATTERN .svn EXCLUDE
    PATTERN LLVMConfig.cmake EXCLUDE
    PATTERN LLVMConfigVersion.cmake EXCLUDE
    PATTERN LLVM37-Config.cmake EXCLUDE
    PATTERN GetHostTriple.cmake EXCLUDE
    PATTERN VersionFromVCS.cmake EXCLUDE
    PATTERN CheckAtomic.cmake EXCLUDE)
endif()
