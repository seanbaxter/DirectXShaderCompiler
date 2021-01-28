add_llvm_library(LLVM37ProfileData
  InstrProf.cpp
  InstrProfReader.cpp
  InstrProfWriter.cpp
  CoverageMapping.cpp
  CoverageMappingWriter.cpp
  CoverageMappingReader.cpp
  SampleProf.cpp
  SampleProfReader.cpp
  SampleProfWriter.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/ProfileData

  DEPENDS
  intrinsics_gen
  )
