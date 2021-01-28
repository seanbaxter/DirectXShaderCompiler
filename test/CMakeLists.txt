configure_lit_site_cfg(
  ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.in
  ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg
  )
configure_lit_site_cfg(
  ${CMAKE_CURRENT_SOURCE_DIR}/Unit/lit.site.cfg.in
  ${CMAKE_CURRENT_BINARY_DIR}/Unit/lit.site.cfg
  )

# Don't include check-llvm37 into check-all without LLVM37_BUILD_TOOLS.
if(NOT LLVM37_BUILD_TOOLS)
  set(EXCLUDE_FROM_ALL ON)
endif()

# Set the depends list as a variable so that it can grow conditionally.
# NOTE: Sync the substitutions in test/lit.cfg when adding to this list.
set(LLVM37_TEST_DEPENDS
          llvm-config
          UnitTests
          BugpointPasses
          LLVM37Hello
          bugpoint
          llc
          lli
          lli-child-target
          llvm37-ar
          llvm37-as
          llvm37-bcanalyzer
          llvm37-c-test
          llvm37-cov
          llvm37-cxxdump
          llvm37-diff
          llvm37-dis
          llvm37-dsymutil
          llvm37-dwarfdump
          llvm37-extract
          llvm37-lib
          llvm37-link
          llvm37-lto
          llvm37-mc
          llvm37-mcmarkup
          llvm37-nm
          llvm37-objdump
          llvm37-profdata
          llvm37-ranlib
          llvm37-readobj
          llvm37-rtdyld
          llvm37-size
          llvm37-symbolizer
          llvm37-tblgen
          macho-dump
          opt
          FileCheck
          count
          not
          yaml-bench
          yaml2obj
          obj2yaml
          verify-uselistorder
        )

# If Intel JIT events are supported, depend on a tool that tests the listener.
if( LLVM37_USE_INTEL_JITEVENTS )
  set(LLVM37_TEST_DEPENDS ${LLVM37_TEST_DEPENDS} llvm37-jitlistener)
endif( LLVM37_USE_INTEL_JITEVENTS )

if(TARGET LLVM37gold)
  set(LLVM37_TEST_DEPENDS ${LLVM37_TEST_DEPENDS} LLVM37gold)
endif()

if(TARGET llvm37-go)
  set(LLVM37_TEST_DEPENDS ${LLVM37_TEST_DEPENDS} llvm37-go)
endif()

if(APPLE)
  set(LLVM37_TEST_DEPENDS ${LLVM37_TEST_DEPENDS} LTO)
endif()

if(TARGET ocaml_llvm37)
  set(LLVM37_TEST_DEPENDS ${LLVM37_TEST_DEPENDS}
          ocaml_llvm37
          ocaml_llvm37_all_backends
          ocaml_llvm37_analysis
          ocaml_llvm37_bitreader
          ocaml_llvm37_bitwriter
          ocaml_llvm37_executionengine
          ocaml_llvm37_irreader
          ocaml_llvm37_linker
          ocaml_llvm37_target
          ocaml_llvm37_ipo
          ocaml_llvm37_passmgr_builder
          ocaml_llvm37_scalar_opts
          ocaml_llvm37_transform_utils
          ocaml_llvm37_vectorize
        )
endif()

add_lit_testsuite(check-llvm37 "Running the LLVM37 regression tests"
  ${CMAKE_CURRENT_BINARY_DIR}
  PARAMS llvm37_site_config=${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg
         llvm37_unit_site_config=${CMAKE_CURRENT_BINARY_DIR}/Unit/lit.site.cfg
  DEPENDS ${LLVM37_TEST_DEPENDS}
  )
set_target_properties(check-llvm37 PROPERTIES FOLDER "Tests")

add_lit_testsuites(LLVM37 ${CMAKE_CURRENT_SOURCE_DIR}
  PARAMS llvm37_site_config=${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg
         llvm37_unit_site_config=${CMAKE_CURRENT_BINARY_DIR}/Unit/lit.site.cfg
  DEPENDS ${LLVM37_TEST_DEPENDS}
  )

# Setup a legacy alias for 'check-llvm37'. This will likely change to be an
# alias for 'check-all' at some point in the future.
add_custom_target(check)
add_dependencies(check check-llvm37)
set_target_properties(check PROPERTIES FOLDER "Tests")
