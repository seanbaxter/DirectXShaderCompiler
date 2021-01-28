add_llvm_library(LLVM37RuntimeDyld
  RTDyldMemoryManager.cpp
  RuntimeDyld.cpp
  RuntimeDyldChecker.cpp
  RuntimeDyldCOFF.cpp
  RuntimeDyldELF.cpp
  RuntimeDyldMachO.cpp

  DEPENDS
  intrinsics_gen
  )
