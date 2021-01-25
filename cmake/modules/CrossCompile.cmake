function(llvm_create_cross_target_internal target_name toochain buildtype)

  if(NOT DEFINED LLVM37_${target_name}_BUILD)
    set(LLVM37_${target_name}_BUILD "${CMAKE_BINARY_DIR}/${target_name}")
    set(LLVM37_${target_name}_BUILD ${LLVM37_${target_name}_BUILD} PARENT_SCOPE)
    message(STATUS "Setting native build dir to " ${LLVM37_${target_name}_BUILD})
  endif(NOT DEFINED LLVM37_${target_name}_BUILD)

  if (EXISTS ${LLVM37_MAIN_SRC_DIR}/cmake/platforms/${toolchain}.cmake)
    set(CROSS_TOOLCHAIN_FLAGS_${target_name} 
        -DCMAKE_TOOLCHAIN_FILE=\"${LLVM37_MAIN_SRC_DIR}/cmake/platforms/${toolchain}.cmake\"
        CACHE STRING "Toolchain file for ${target_name}")
  endif()

  add_custom_command(OUTPUT ${LLVM37_${target_name}_BUILD}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LLVM37_${target_name}_BUILD}
    COMMENT "Creating ${LLVM37_${target_name}_BUILD}...")

  add_custom_command(OUTPUT ${LLVM37_${target_name}_BUILD}/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}"
        ${CROSS_TOOLCHAIN_FLAGS_${target_name}} ${CMAKE_SOURCE_DIR}
    WORKING_DIRECTORY ${LLVM37_${target_name}_BUILD}
    DEPENDS ${LLVM37_${target_name}_BUILD}
    COMMENT "Configuring ${target_name} LLVM...")

  add_custom_target(CONFIGURE_LLVM37_${target_name}
                    DEPENDS ${LLVM37_${target_name}_BUILD}/CMakeCache.txt)

  set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
                                      ${LLVM37_${target_name}_BUILD})

  if(NOT IS_DIRECTORY ${LLVM37_${target_name}_BUILD})
    

    message(STATUS "Configuring ${target_name} build...")
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory
      ${LLVM37_${target_name}_BUILD} )

    message(STATUS "Configuring ${target_name} targets...")
    if (buildtype)
      set(build_type_flags "-DCMAKE_BUILD_TYPE=${buildtype}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} ${build_type_flags}
        -G "${CMAKE_GENERATOR}" -DLLVM37_TARGETS_TO_BUILD=${LLVM37_TARGETS_TO_BUILD}
        ${CROSS_TOOLCHAIN_FLAGS_${target_name}} ${CMAKE_SOURCE_DIR}
      WORKING_DIRECTORY ${LLVM37_${target_name}_BUILD} )
  endif(NOT IS_DIRECTORY ${LLVM37_${target_name}_BUILD})

endfunction()

function(llvm_create_cross_target target_name sysroot)
  llvm_create_cross_target_internal(${target_name} ${sysroot} ${CMAKE_BUILD_TYPE})
endfunction()

llvm_create_cross_target_internal(NATIVE "" Release)
