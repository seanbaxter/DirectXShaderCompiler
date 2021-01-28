# If we don't need RTTI or EH, there's no reason to export anything
# from the hello plugin.
if( NOT LLVM37_REQUIRES_RTTI )
  if( NOT LLVM37_REQUIRES_EH )
    set(LLVM37_EXPORTED_SYMBOL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Hello.exports)
  endif()
endif()

if(WIN32 OR CYGWIN)
  set(LLVM37_LINK_COMPONENTS Core Support)
endif()

add_llvm37_loadable_module( LLVM37Hello
  Hello.cpp

  DEPENDS
  intrinsics_gen
  )
