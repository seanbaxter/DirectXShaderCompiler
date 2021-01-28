set(LLVM37_LINK_COMPONENTS core support object profiledata)

add_llvm37_tool(llvm37-cov
  llvm37-cov.cpp
  gcov.cpp
  CodeCoverage.cpp
  CoverageFilters.cpp
  CoverageReport.cpp
  CoverageSummaryInfo.cpp
  SourceCoverageView.cpp
  TestingSupport.cpp
  )
