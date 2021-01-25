

add_llvm_library(LLVMExecutionEngine
  ExecutionEngine.cpp
  ExecutionEngineBindings.cpp
  GDBRegistrationListener.cpp
  SectionMemoryManager.cpp
  TargetSelect.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm/ExecutionEngine

  DEPENDS
  intrinsics_gen
  )

add_subdirectory(Interpreter)
# add_subdirectory(MCJIT) # HLSL Change
# add_subdirectory(Orc) # HLSL Change
# add_subdirectory(RuntimeDyld) # HLSL Change

if( LLVM37_USE_OPROFILE )
  add_subdirectory(OProfileJIT)
endif( LLVM37_USE_OPROFILE )

if( LLVM37_USE_INTEL_JITEVENTS )
  add_subdirectory(IntelJITEvents)
endif( LLVM37_USE_INTEL_JITEVENTS )
