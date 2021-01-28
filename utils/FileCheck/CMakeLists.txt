add_llvm37_utility(FileCheck
  FileCheck.cpp
  )

target_link_libraries(FileCheck LLVM37Support LLVM37MSSupport)
