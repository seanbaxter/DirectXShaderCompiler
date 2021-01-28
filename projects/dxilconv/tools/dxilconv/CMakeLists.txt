# Build dxilconv.dll

set(SHARED_LIBRARY TRUE)

add_dxilconv_project_library(dxilconv SHARED
  dxilconv.cpp
  dxilconv.def
)

target_link_libraries(dxilconv PRIVATE
  DxbcConverter
  LLVM37BitWriter
  LLVM37DXIL
  LLVM37DxilContainer
  LLVM37DxilRootSignature
  LLVM37HLSL  
  LLVM37MSSupport
  LLVM37ScalarOpts
)

set_target_properties(dxilconv
  PROPERTIES
  OUTPUT_NAME "dxilconv"
)

add_dependencies(dxilconv DxcEtw)