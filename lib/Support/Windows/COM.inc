//===- llvm37/Support/Windows/COM.inc - Windows COM Implementation *- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Windows portion of COM support.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//=== WARNING: Implementation here must contain only Windows code.
//===----------------------------------------------------------------------===//

#include <objbase.h>

namespace llvm37 {
namespace sys {

InitializeCOMRAII::InitializeCOMRAII(COMThreadingMode Threading,
                                     bool SpeedOverMemory) {
  DWORD Coinit = 0;
  if (Threading == COMThreadingMode::SingleThreaded)
    Coinit |= COINIT_APARTMENTTHREADED;
  else
    Coinit |= COINIT_MULTITHREADED;
  if (SpeedOverMemory)
    Coinit |= COINIT_SPEED_OVER_MEMORY;
  ::CoInitializeEx(nullptr, Coinit);
}

InitializeCOMRAII::~InitializeCOMRAII() { ::CoUninitialize(); }
}
}
