# LLVM37_TARGET_DEFINITIONS must contain the name of the .td file to process.
# Extra parameters for `tblgen' may come after `ofn' parameter.
# Adds the name of the generated file to TABLEGEN_OUTPUT.

function(tablegen project ofn)
  # Validate calling context.
  foreach(v
      ${project}_TABLEGEN_EXE
      LLVM37_MAIN_SRC_DIR
      LLVM37_MAIN_INCLUDE_DIR
      )
    if(NOT ${v})
      message(FATAL_ERROR "${v} not set")
    endif()
  endforeach()

  file(GLOB local_tds "*.td")
  file(GLOB_RECURSE global_tds "${LLVM37_MAIN_INCLUDE_DIR}/llvm/*.td")

  if (IS_ABSOLUTE ${LLVM37_TARGET_DEFINITIONS})
    set(LLVM37_TARGET_DEFINITIONS_ABSOLUTE ${LLVM37_TARGET_DEFINITIONS})
  else()
    set(LLVM37_TARGET_DEFINITIONS_ABSOLUTE
      ${CMAKE_CURRENT_SOURCE_DIR}/${LLVM37_TARGET_DEFINITIONS})
  endif()
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ofn}.tmp
    # Generate tablegen output in a temporary file.
    COMMAND ${${project}_TABLEGEN_EXE} ${ARGN} -I ${CMAKE_CURRENT_SOURCE_DIR}
    -I ${LLVM37_MAIN_SRC_DIR}/lib/Target -I ${LLVM37_MAIN_INCLUDE_DIR}
    ${LLVM37_TARGET_DEFINITIONS_ABSOLUTE}
    -o ${CMAKE_CURRENT_BINARY_DIR}/${ofn}.tmp
    # The file in LLVM37_TARGET_DEFINITIONS may be not in the current
    # directory and local_tds may not contain it, so we must
    # explicitly list it here:
    DEPENDS ${${project}_TABLEGEN_TARGET} ${local_tds} ${global_tds}
    ${LLVM37_TARGET_DEFINITIONS_ABSOLUTE}
    COMMENT "Building ${ofn}..."
    )
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ofn}
    # Only update the real output file if there are any differences.
    # This prevents recompilation of all the files depending on it if there
    # aren't any.
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/${ofn}.tmp
        ${CMAKE_CURRENT_BINARY_DIR}/${ofn}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${ofn}.tmp
    COMMENT "Updating ${ofn}..."
    )

  # `make clean' must remove all those generated files:
  set_property(DIRECTORY APPEND
    PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${ofn}.tmp ${ofn})

  set(TABLEGEN_OUTPUT ${TABLEGEN_OUTPUT} ${CMAKE_CURRENT_BINARY_DIR}/${ofn} PARENT_SCOPE)
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${ofn} PROPERTIES
    GENERATED 1)
endfunction()

# Creates a target for publicly exporting tablegen dependencies.
function(add_public_tablegen_target target)
  if(NOT TABLEGEN_OUTPUT)
    message(FATAL_ERROR "Requires tablegen() definitions as TABLEGEN_OUTPUT.")
  endif()
  add_custom_target(${target}
    DEPENDS ${TABLEGEN_OUTPUT})
  if(LLVM37_COMMON_DEPENDS)
    add_dependencies(${target} ${LLVM37_COMMON_DEPENDS})
  endif()
  set_target_properties(${target} PROPERTIES FOLDER "Tablegenning")
  set(LLVM37_COMMON_DEPENDS ${LLVM37_COMMON_DEPENDS} ${target} PARENT_SCOPE)
endfunction()

macro(add_tablegen target project)
  set(${target}_OLD_LLVM37_LINK_COMPONENTS ${LLVM37_LINK_COMPONENTS})
  set(LLVM37_LINK_COMPONENTS ${LLVM37_LINK_COMPONENTS} TableGen)
  add_llvm_utility(${target} ${ARGN})
  set(LLVM37_LINK_COMPONENTS ${${target}_OLD_LLVM37_LINK_COMPONENTS})

  set(${project}_TABLEGEN "${target}" CACHE
      STRING "Native TableGen executable. Saves building one when cross-compiling.")

  # Upgrade existing LLVM37_TABLEGEN setting.
  if(${project} STREQUAL LLVM)
    if(${LLVM37_TABLEGEN} STREQUAL tblgen)
      set(LLVM37_TABLEGEN "${target}" CACHE
          STRING "Native TableGen executable. Saves building one when cross-compiling."
          FORCE)
    endif()
  endif()

  # Effective tblgen executable to be used:
  set(${project}_TABLEGEN_EXE ${${project}_TABLEGEN} PARENT_SCOPE)
  set(${project}_TABLEGEN_TARGET ${${project}_TABLEGEN} PARENT_SCOPE)

  if(LLVM37_USE_HOST_TOOLS)
    if( ${${project}_TABLEGEN} STREQUAL "${target}" )
      if (NOT CMAKE_CONFIGURATION_TYPES)
        set(${project}_TABLEGEN_EXE "${LLVM37_NATIVE_BUILD}/bin/${target}")
      else()
        set(${project}_TABLEGEN_EXE "${LLVM37_NATIVE_BUILD}/Release/bin/${target}")
      endif()
      set(${project}_TABLEGEN_EXE ${${project}_TABLEGEN_EXE} PARENT_SCOPE)

      add_custom_command(OUTPUT ${${project}_TABLEGEN_EXE}
        COMMAND ${CMAKE_COMMAND} --build . --target ${target} --config Release
        DEPENDS CONFIGURE_LLVM37_NATIVE ${target}
        WORKING_DIRECTORY ${LLVM37_NATIVE_BUILD}
        COMMENT "Building native TableGen...")
      add_custom_target(${project}-tablegen-host DEPENDS ${${project}_TABLEGEN_EXE})
      set(${project}_TABLEGEN_TARGET ${project}-tablegen-host PARENT_SCOPE)
    endif()
  endif()

  if( MINGW )
    if(CMAKE_SIZEOF_VOID_P MATCHES "8")
      set_target_properties(${target} PROPERTIES LINK_FLAGS -Wl,--stack,16777216)
    endif(CMAKE_SIZEOF_VOID_P MATCHES "8")
  endif( MINGW )
  if (${project} STREQUAL LLVM AND NOT LLVM37_INSTALL_TOOLCHAIN_ONLY)
    install(TARGETS ${target}
            EXPORT LLVMExports
            RUNTIME DESTINATION bin)
  endif()
  set_property(GLOBAL APPEND PROPERTY LLVM37_EXPORTS ${target})
endmacro()
