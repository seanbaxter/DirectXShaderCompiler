set(LLVM37_LINK_COMPONENTS support)

add_llvm37_executable(lli-child-target
  ChildTarget.cpp
  ../RemoteTarget.cpp
)

set_target_properties(lli-child-target PROPERTIES FOLDER "Misc")
