add_subdirectory(ChildTarget)

set(LLVM37_LINK_COMPONENTS
  CodeGen
  Core
  ExecutionEngine
  IRReader
  Instrumentation
  Interpreter
#  MC     # HLSL Change
#  MCJIT  # HLSL Change
  Object
  OrcJIT
  RuntimeDyld
  SelectionDAG
  Support
  Target
  TransformUtils
  native
  )

if( LLVM37_USE_OPROFILE )
  set(LLVM37_LINK_COMPONENTS
    ${LLVM37_LINK_COMPONENTS}
    OProfileJIT
    )
endif( LLVM37_USE_OPROFILE )

if( LLVM37_USE_INTEL_JITEVENTS )
  set(LLVM37_LINK_COMPONENTS
    ${LLVM37_LINK_COMPONENTS}
    DebugInfoDWARF
    IntelJITEvents
    Object
    )
endif( LLVM37_USE_INTEL_JITEVENTS )

add_llvm_tool(lli
  lli.cpp
  OrcLazyJIT.cpp
  RemoteMemoryManager.cpp
  RemoteTarget.cpp
  RemoteTargetExternal.cpp
  )
export_executable_symbols(lli)
