add_llvm_library(LLVM37AsmPrinter
  AddressPool.cpp
  ARMException.cpp
  AsmPrinter.cpp
  AsmPrinterDwarf.cpp
  AsmPrinterInlineAsm.cpp
  DbgValueHistoryCalculator.cpp
  DebugLocStream.cpp
  DIE.cpp
  DIEHash.cpp
  DwarfAccelTable.cpp
  DwarfCFIException.cpp
  DwarfCompileUnit.cpp
  DwarfDebug.cpp
  DwarfExpression.cpp
  DwarfFile.cpp
  DwarfStringPool.cpp
  DwarfUnit.cpp
  EHStreamer.cpp
  ErlangGCPrinter.cpp
  OcamlGCPrinter.cpp
  WinException.cpp
  WinCodeViewLineTables.cpp
  )

add_dependencies(LLVM37AsmPrinter intrinsics_gen)
