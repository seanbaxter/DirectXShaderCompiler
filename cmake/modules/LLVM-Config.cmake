function(get_system_libs return_var)
  message(AUTHOR_WARNING "get_system_libs no longer needed")
  set(${return_var} "" PARENT_SCOPE)
endfunction()


function(link_system_libs target)
  message(AUTHOR_WARNING "link_system_libs no longer needed")
endfunction()


function(is_llvm37_target_library library return_var)
  # Sets variable `return_var' to ON if `library' corresponds to a
  # LLVM37 supported target. To OFF if it doesn't.
  set(${return_var} OFF PARENT_SCOPE)
  string(TOUPPER "${library}" capitalized_lib)
  string(TOUPPER "${LLVM37_ALL_TARGETS}" targets)
  foreach(t ${targets})
    if( capitalized_lib STREQUAL t OR
        capitalized_lib STREQUAL "LLVM37${t}" OR
        capitalized_lib STREQUAL "LLVM37${t}CODEGEN" OR
        capitalized_lib STREQUAL "LLVM37${t}ASMPARSER" OR
        capitalized_lib STREQUAL "LLVM37${t}ASMPRINTER" OR
        capitalized_lib STREQUAL "LLVM37${t}DISASSEMBLER" OR
        capitalized_lib STREQUAL "LLVM37${t}INFO" )
      set(${return_var} ON PARENT_SCOPE)
      break()
    endif()
  endforeach()
endfunction(is_llvm37_target_library)


macro(llvm37_config executable)
  explicit_llvm37_config(${executable} ${ARGN})
endmacro(llvm37_config)


function(explicit_llvm37_config executable)
  set( link_components ${ARGN} )

  llvm37_map_components_to_libnames(LIBRARIES ${link_components})
  get_target_property(t ${executable} TYPE)
  if("x${t}" STREQUAL "xSTATIC_LIBRARY")
    target_link_libraries(${executable} INTERFACE ${LIBRARIES})
  elseif("x${t}" STREQUAL "xSHARED_LIBRARY" OR "x${t}" STREQUAL "xMODULE_LIBRARY")
    target_link_libraries(${executable} PRIVATE ${LIBRARIES})
  else()
    # Use plain form for legacy user.
    target_link_libraries(${executable} ${LIBRARIES})
  endif()
endfunction(explicit_llvm37_config)


# This is Deprecated
function(llvm37_map_components_to_libraries OUT_VAR)
  message(AUTHOR_WARNING "Using llvm37_map_components_to_libraries() is deprecated. Use llvm37_map_components_to_libnames() instead")
  explicit_map_components_to_libraries(result ${ARGN})
  set( ${OUT_VAR} ${result} ${sys_result} PARENT_SCOPE )
endfunction(llvm37_map_components_to_libraries)

# This is a variant intended for the final user:
# Map LINK_COMPONENTS to actual libnames.
function(llvm37_map_components_to_libnames out_libs)
  set( link_components ${ARGN} )
  if(NOT LLVM37_AVAILABLE_LIBS)
    # Inside LLVM37 itself available libs are in a global property.
    get_property(LLVM37_AVAILABLE_LIBS GLOBAL PROPERTY LLVM37_LIBS)
  endif()
  string(TOUPPER "${LLVM37_AVAILABLE_LIBS}" capitalized_libs)

  # Expand some keywords:
  list(FIND LLVM37_TARGETS_TO_BUILD "${LLVM37_NATIVE_ARCH}" have_native_backend)
  list(FIND link_components "engine" engine_required)
  if( NOT engine_required EQUAL -1 )
    list(FIND LLVM37_TARGETS_WITH_JIT "${LLVM37_NATIVE_ARCH}" have_jit)
    if( NOT have_native_backend EQUAL -1 AND NOT have_jit EQUAL -1 )
      list(APPEND link_components "jit")
      list(APPEND link_components "native")
    else()
      list(APPEND link_components "interpreter")
    endif()
  endif()
  list(FIND link_components "native" native_required)
  if( NOT native_required EQUAL -1 )
    if( NOT have_native_backend EQUAL -1 )
      list(APPEND link_components ${LLVM37_NATIVE_ARCH})
    endif()
  endif()

  # Translate symbolic component names to real libraries:
  foreach(c ${link_components})
    # add codegen, asmprinter, asmparser, disassembler
    list(FIND LLVM37_TARGETS_TO_BUILD ${c} idx)
    if( NOT idx LESS 0 )
      if( TARGET LLVM37${c}CodeGen )
        list(APPEND expanded_components "LLVM37${c}CodeGen")
      else()
        if( TARGET LLVM37${c} )
          list(APPEND expanded_components "LLVM37${c}")
        elseif( NOT c STREQUAL "None" ) # HLSL Change
          message(FATAL_ERROR "Target ${c} is not in the set of libraries.")
        endif()
      endif()
      if( TARGET LLVM37${c}AsmPrinter )
        list(APPEND expanded_components "LLVM37${c}AsmPrinter")
      endif()
      if( TARGET LLVM37${c}AsmParser )
        list(APPEND expanded_components "LLVM37${c}AsmParser")
      endif()
      if( TARGET LLVM37${c}Desc )
        list(APPEND expanded_components "LLVM37${c}Desc")
      endif()
      if( TARGET LLVM37${c}Info )
        list(APPEND expanded_components "LLVM37${c}Info")
      endif()
      if( TARGET LLVM37${c}Disassembler )
        list(APPEND expanded_components "LLVM37${c}Disassembler")
      endif()
    elseif( c STREQUAL "native" )
      # already processed
    elseif( c STREQUAL "nativecodegen" )
      list(APPEND expanded_components "LLVM37${LLVM37_NATIVE_ARCH}CodeGen")
      if( TARGET LLVM37${LLVM37_NATIVE_ARCH}Desc )
        list(APPEND expanded_components "LLVM37${LLVM37_NATIVE_ARCH}Desc")
      endif()
      if( TARGET LLVM37${LLVM37_NATIVE_ARCH}Info )
        list(APPEND expanded_components "LLVM37${LLVM37_NATIVE_ARCH}Info")
      endif()
    elseif( c STREQUAL "backend" )
      # same case as in `native'.
    elseif( c STREQUAL "engine" )
      # already processed
    elseif( c STREQUAL "all" )
      list(APPEND expanded_components ${LLVM37_AVAILABLE_LIBS})
    elseif( c STREQUAL "AllTargetsAsmPrinters" )
      # Link all the asm printers from all the targets
      foreach(t ${LLVM37_TARGETS_TO_BUILD})
        if( TARGET LLVM37${t}AsmPrinter )
          list(APPEND expanded_components "LLVM37${t}AsmPrinter")
        endif()
      endforeach(t)
    elseif( c STREQUAL "AllTargetsAsmParsers" )
      # Link all the asm parsers from all the targets
      foreach(t ${LLVM37_TARGETS_TO_BUILD})
        if( TARGET LLVM37${t}AsmParser )
          list(APPEND expanded_components "LLVM37${t}AsmParser")
        endif()
      endforeach(t)
    elseif( c STREQUAL "AllTargetsDescs" )
      # Link all the descs from all the targets
      foreach(t ${LLVM37_TARGETS_TO_BUILD})
        if( TARGET LLVM37${t}Desc )
          list(APPEND expanded_components "LLVM37${t}Desc")
        endif()
      endforeach(t)
    elseif( c STREQUAL "AllTargetsDisassemblers" )
      # Link all the disassemblers from all the targets
      foreach(t ${LLVM37_TARGETS_TO_BUILD})
        if( TARGET LLVM37${t}Disassembler )
          list(APPEND expanded_components "LLVM37${t}Disassembler")
        endif()
      endforeach(t)
    elseif( c STREQUAL "AllTargetsInfos" )
      # Link all the infos from all the targets
      foreach(t ${LLVM37_TARGETS_TO_BUILD})
        if( TARGET LLVM37${t}Info )
          list(APPEND expanded_components "LLVM37${t}Info")
        endif()
      endforeach(t)
    else( NOT idx LESS 0 )
      # Canonize the component name:
      string(TOUPPER "${c}" capitalized)
      list(FIND capitalized_libs LLVM37${capitalized} lib_idx)
      if( lib_idx LESS 0 )
        # The component is unknown. Maybe is an omitted target?
        is_llvm37_target_library(${c} iltl_result)
        if( NOT iltl_result )
          message(FATAL_ERROR "Library `${c}' not found in list of llvm37 libraries.")
        endif()
      else( lib_idx LESS 0 )
        list(GET LLVM37_AVAILABLE_LIBS ${lib_idx} canonical_lib)
        list(APPEND expanded_components ${canonical_lib})
      endif( lib_idx LESS 0 )
    endif( NOT idx LESS 0 )
  endforeach(c)

  set(${out_libs} ${expanded_components} PARENT_SCOPE)
endfunction()

# Perform a post-order traversal of the dependency graph.
# This duplicates the algorithm used by llvm-config, originally
# in tools/llvm-config/llvm-config.cpp, function ComputeLibsForComponents.
function(expand_topologically name required_libs visited_libs)
  list(FIND visited_libs ${name} found)
  if( found LESS 0 )
    list(APPEND visited_libs ${name})
    set(visited_libs ${visited_libs} PARENT_SCOPE)

    get_property(lib_deps GLOBAL PROPERTY LLVM37BUILD_LIB_DEPS_${name})
    foreach( lib_dep ${lib_deps} )
      expand_topologically(${lib_dep} "${required_libs}" "${visited_libs}")
      set(required_libs ${required_libs} PARENT_SCOPE)
      set(visited_libs ${visited_libs} PARENT_SCOPE)
    endforeach()

    list(APPEND required_libs ${name})
    set(required_libs ${required_libs} PARENT_SCOPE)
  endif()
endfunction()

# Expand dependencies while topologically sorting the list of libraries:
function(llvm37_expand_dependencies out_libs)
  set(expanded_components ${ARGN})

  set(required_libs)
  set(visited_libs)
  foreach( lib ${expanded_components} )
    expand_topologically(${lib} "${required_libs}" "${visited_libs}")
  endforeach()

  list(REVERSE required_libs)
  set(${out_libs} ${required_libs} PARENT_SCOPE)
endfunction()

function(explicit_map_components_to_libraries out_libs)
  llvm37_map_components_to_libnames(link_libs ${ARGN})
  llvm37_expand_dependencies(expanded_components ${link_libs})
  # Return just the libraries included in this build:
  set(result)
  foreach(c ${expanded_components})
    if( TARGET ${c} )
      set(result ${result} ${c})
    endif()
  endforeach(c)
  set(${out_libs} ${result} PARENT_SCOPE)
endfunction(explicit_map_components_to_libraries)
