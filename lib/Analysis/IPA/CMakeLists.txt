add_llvm_library(LLVM37ipa
  CallGraph.cpp
  CallGraphSCCPass.cpp
  CallPrinter.cpp
  GlobalsModRef.cpp
  IPA.cpp
  InlineCost.cpp
  )

add_dependencies(LLVM37ipa intrinsics_gen)
