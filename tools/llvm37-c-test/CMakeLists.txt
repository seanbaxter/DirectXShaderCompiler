set(LLVM37_LINK_COMPONENTS
  ${LLVM37_TARGETS_TO_BUILD}
  BitReader
  Core
  MCDisassembler
  Object
  Target
  )

# We should only have llvm37-c-test use libLLVM37 if libLLVM37 is built with the
# default list of components. Using libLLVM37 with custom components can result in
# build failures.

set (USE_LLVM37_DYLIB FALSE)

if (TARGET LLVM37)
  set (USE_LLVM37_DYLIB TRUE)
  if (DEFINED LLVM37_DYLIB_COMPONENTS)
    foreach(c in ${LLVM37_LINK_COMPONENTS})
      list(FIND LLVM37_DYLIB_COMPONENTS ${c} C_IDX)
      if (C_IDX EQUAL -1)
        set(USE_LLVM37_DYLIB FALSE)
        break()
      endif()
    endforeach()
  endif()
endif()

if(USE_LLVM37_DYLIB)
  set(LLVM37_LINK_COMPONENTS)
endif()

if (LLVM37_COMPILER_IS_GCC_COMPATIBLE)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wstrict-prototypes")
endif ()

add_llvm37_tool(llvm37-c-test
  calc.c
  disassemble.c
  helpers.c
  include-all.c
  main.c
  module.c
  metadata.c
  object.c
  targets.c
  )

if(USE_LLVM37_DYLIB)
  target_link_libraries(llvm37-c-test LLVM37)
endif()
