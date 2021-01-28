# This tool creates a shared library from the LLVM37 libraries. Generating this
# library is enabled by setting LLVM37_BUILD_LLVM37_DYLIB=yes on the CMake
# commandline. By default the shared library only exports the LLVM37 C API.


# You can configure which libraries from LLVM37 you want to include in the shared
# library by setting LLVM37_DYLIB_COMPONENTS to a semi-colon delimited list of
# LLVM37 components. All compoenent names handled by llvm-config are valid.

if(NOT DEFINED LLVM37_DYLIB_COMPONENTS)
  set(LLVM37_DYLIB_COMPONENTS
    ${LLVM37_TARGETS_TO_BUILD}
    Analysis
    BitReader
    BitWriter
    CodeGen
    Core
    DebugInfoDWARF
    DebugInfoPDB
    ExecutionEngine
    IPA
    IPO
    IRReader
    InstCombine
    Instrumentation
    Interpreter
    Linker
    MCDisassembler
    MCJIT
    ObjCARCOpts
    Object
    ScalarOpts
    Support
    Target
    TransformUtils
    Vectorize
    native
    )
endif()

add_definitions( -DLLVM37_VERSION_INFO=\"${PACKAGE_VERSION}\" )

set(SOURCES
  libllvm37.cpp
  )

llvm37_map_components_to_libnames(LIB_NAMES ${LLVM37_DYLIB_COMPONENTS})

if(NOT DEFINED LLVM37_DYLIB_EXPORTED_SYMBOL_FILE)

  if( WIN32 AND NOT CYGWIN )
    message(FATAL_ERROR "Auto-generation not implemented for Win32 without GNU utils. Please specify LLVM37_EXPORTED_SYMBOL_FILE.")
  endif()

  # To get the export list for a single llvm37 library:
  # nm ${LIB_PATH} | awk "/T _LLVM37/ { print $3 }" | sort -u | sed -e "s/^_//g" > ${LIB_PATH}.exports

  set(LLVM37_EXPORTED_SYMBOL_FILE ${CMAKE_BINARY_DIR}/libllvm37.exports)

  if (NOT LLVM37_DYLIB_EXPORT_ALL)
    foreach (lib ${LIB_NAMES})
      set(LIB_DIR ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/lib${LLVM37_LIBDIR_SUFFIX})
      set(LIB_NAME ${LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${lib})
      set(LIB_PATH ${LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX})
      set(LIB_EXPORTS_PATH ${LIB_NAME}.exports)
      list(APPEND LLVM37_DYLIB_REQUIRED_EXPORTS ${LIB_EXPORTS_PATH})

      
      add_custom_command(OUTPUT ${LIB_EXPORTS_PATH}
        COMMAND nm ${LIB_PATH} | awk "/T _LLVM37/ || /T LLVM37/ { print $3 }" | sort -u | sed -e "s/^_//g" > ${LIB_EXPORTS_PATH}
        WORKING_DIRECTORY ${LIB_DIR}
        DEPENDS ${lib}
        COMMENT "Generating Export list for ${lib}..."
        VERBATIM )
    endforeach ()
  endif()

  if (LLVM37_DYLIB_EXPORT_ALL)
    add_custom_command(OUTPUT ${LLVM37_EXPORTED_SYMBOL_FILE}
      COMMAND echo \"LLVM37*\" > ${LLVM37_EXPORTED_SYMBOL_FILE} && echo \"_Z*llvm37*\" >> ${LLVM37_EXPORTED_SYMBOL_FILE}
      WORKING_DIRECTORY ${LIB_DIR}
      DEPENDS ${LLVM37_DYLIB_REQUIRED_EXPORTS}
      COMMENT "Generating combined export list...")
  else()
    add_custom_command(OUTPUT ${LLVM37_EXPORTED_SYMBOL_FILE}
      COMMAND cat ${LLVM37_DYLIB_REQUIRED_EXPORTS} > ${LLVM37_EXPORTED_SYMBOL_FILE}
      WORKING_DIRECTORY ${LIB_DIR}
      DEPENDS ${LLVM37_DYLIB_REQUIRED_EXPORTS}
      COMMENT "Generating combined export list...")
  endif()

  add_custom_target(libLLVM37Exports DEPENDS ${LLVM37_EXPORTED_SYMBOL_FILE})
else()
  set(LLVM37_EXPORTED_SYMBOL_FILE ${LLVM37_DYLIB_EXPORTED_SYMBOL_FILE})
  add_custom_target(libLLVM37Exports DEPENDS ${LLVM37_EXPORTED_SYMBOL_FILE})
endif()

add_llvm_library(LLVM37 SHARED ${SOURCES})

list(REMOVE_DUPLICATES LIB_NAMES)
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux") # FIXME: It should be "GNU ld for elf"
  # GNU ld doesn't resolve symbols in the version script.
  set(LIB_NAMES -Wl,--whole-archive ${LIB_NAMES} -Wl,--no-whole-archive)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set(LIB_NAMES -Wl,-all_load ${LIB_NAMES})
endif()

target_link_libraries(LLVM37 PRIVATE ${LIB_NAMES})

add_dependencies(LLVM37 libLLVM37Exports)

if (APPLE)
  set_property(TARGET LLVM37 APPEND_STRING PROPERTY
              LINK_FLAGS
              " -compatibility_version ${LLVM37_VERSION_MAJOR}.${LLVM37_VERSION_MINOR} -current_version ${LLVM37_VERSION_MAJOR}.${LLVM37_VERSION_MINOR}")
endif()

