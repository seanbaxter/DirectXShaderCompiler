
if (DOXYGEN_FOUND)
if (LLVM37_ENABLE_DOXYGEN)
  set(abs_top_srcdir ${LLVM37_MAIN_SRC_DIR})
  set(abs_top_builddir ${LLVM37_BINARY_DIR})
  
  if (HAVE_DOT)
    set(DOT ${LLVM37_PATH_DOT})
  endif()

  if (LLVM37_DOXYGEN_EXTERNAL_SEARCH)
    set(enable_searchengine "YES")
    set(searchengine_url "${LLVM37_DOXYGEN_SEARCHENGINE_URL}")
    set(enable_server_based_search "YES")
    set(enable_external_search "YES")
    set(extra_search_mappings "${LLVM37_DOXYGEN_SEARCH_MAPPINGS}")
  else()
    set(enable_searchengine "NO")
    set(searchengine_url "")
    set(enable_server_based_search "NO")
    set(enable_external_search "NO")
    set(extra_search_mappings "")
  endif()
  
  # If asked, configure doxygen for the creation of a Qt Compressed Help file.
  option(LLVM37_ENABLE_DOXYGEN_QT_HELP
    "Generate a Qt Compressed Help file." OFF)
  if (LLVM37_ENABLE_DOXYGEN_QT_HELP)
    set(LLVM37_DOXYGEN_QCH_FILENAME "org.llvm37.qch" CACHE STRING
      "Filename of the Qt Compressed help file")
    set(LLVM37_DOXYGEN_QHP_NAMESPACE "org.llvm37" CACHE STRING 
      "Namespace under which the intermediate Qt Help Project file lives")
    set(LLVM37_DOXYGEN_QHP_CUST_FILTER_NAME "${PACKAGE_STRING}" CACHE STRING
      "See http://qt-project.org/doc/qt-4.8/qthelpproject.html#custom-filters")
    set(LLVM37_DOXYGEN_QHP_CUST_FILTER_ATTRS "${PACKAGE_NAME},${PACKAGE_VERSION}" CACHE STRING
      "See http://qt-project.org/doc/qt-4.8/qthelpproject.html#filter-attributes")
    find_program(LLVM37_DOXYGEN_QHELPGENERATOR_PATH qhelpgenerator 
      DOC "Path to the qhelpgenerator binary")
    if (NOT LLVM37_DOXYGEN_QHELPGENERATOR_PATH)
      message(FATAL_ERROR "Failed to find qhelpgenerator binary")
    endif()

    set(llvm37_doxygen_generate_qhp "YES")
    set(llvm37_doxygen_qch_filename "${LLVM37_DOXYGEN_QCH_FILENAME}")
    set(llvm37_doxygen_qhp_namespace "${LLVM37_DOXYGEN_QHP_NAMESPACE}")
    set(llvm37_doxygen_qhelpgenerator_path "${LLVM37_DOXYGEN_QHELPGENERATOR_PATH}")
    set(llvm37_doxygen_qhp_cust_filter_name "${LLVM37_DOXYGEN_QHP_CUST_FILTER_NAME}")
    set(llvm37_doxygen_qhp_cust_filter_attrs "${LLVM37_DOXYGEN_QHP_CUST_FILTER_ATTRS}")

  else()
    set(llvm37_doxygen_generate_qhp "NO")
    set(llvm37_doxygen_qch_filename "")
    set(llvm37_doxygen_qhp_namespace "")
    set(llvm37_doxygen_qhelpgenerator_path "")
    set(llvm37_doxygen_qhp_cust_filter_name "")
    set(llvm37_doxygen_qhp_cust_filter_attrs "")
  endif()
  
  option(LLVM37_DOXYGEN_SVG
    "Use svg instead of png files for doxygen graphs." OFF)
  if (LLVM37_DOXYGEN_SVG)
    set(DOT_IMAGE_FORMAT "svg")
  else()
    set(DOT_IMAGE_FORMAT "png")
  endif()

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen.cfg.in
    ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg @ONLY)

  set(abs_top_srcdir)
  set(abs_top_builddir)
  set(DOT)
  set(enable_searchengine)
  set(searchengine_url)
  set(enable_server_based_search)
  set(enable_external_search)
  set(extra_search_mappings)
  set(llvm37_doxygen_generate_qhp)
  set(llvm37_doxygen_qch_filename)
  set(llvm37_doxygen_qhp_namespace)
  set(llvm37_doxygen_qhelpgenerator_path)
  set(llvm37_doxygen_qhp_cust_filter_name)
  set(llvm37_doxygen_qhp_cust_filter_attrs)
  set(DOT_IMAGE_FORMAT)

  add_custom_target(doxygen-llvm37
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating llvm37 doxygen documentation." VERBATIM)

  if (LLVM37_BUILD_DOCS)
    add_dependencies(doxygen doxygen-llvm37)
  endif()

  if (NOT LLVM37_INSTALL_TOOLCHAIN_ONLY)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen/html
      DESTINATION docs/html)
  endif()
endif()
endif()

if (LLVM37_ENABLE_SPHINX)
  if (SPHINX_FOUND)
    include(AddSphinxTarget)
    if (${SPHINX_OUTPUT_HTML})
      add_sphinx_target(html llvm37)
    endif()


    if (${SPHINX_OUTPUT_MAN})
      add_sphinx_target(man llvm37)
    endif()

  endif()
endif()

list(FIND LLVM37_BINDINGS_LIST ocaml uses_ocaml)
if( NOT uses_ocaml LESS 0 )
  set(doc_targets
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

  foreach(llvm37_target ${LLVM37_TARGETS_TO_BUILD})
    list(APPEND doc_targets ocaml_llvm37_${llvm37_target})
  endforeach()

  set(odoc_files)
  foreach( doc_target ${doc_targets} )
    get_target_property(odoc_file ${doc_target} OCAML_ODOC)
    list(APPEND odoc_files -load ${odoc_file})
  endforeach()

  add_custom_target(ocaml_doc
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/ocamldoc/html
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/ocamldoc/html
    COMMAND ${OCAMLFIND} ocamldoc -d ${CMAKE_CURRENT_BINARY_DIR}/ocamldoc/html
                                  -sort -colorize-code -html ${odoc_files})

  add_dependencies(ocaml_doc ${doc_targets})

  if (NOT LLVM37_INSTALL_TOOLCHAIN_ONLY)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/ocamldoc/html
      DESTINATION docs/ocaml/html)
  endif()
endif()
