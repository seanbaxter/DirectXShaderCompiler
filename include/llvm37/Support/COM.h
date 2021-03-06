//===- llvm37/Support/COM.h ---------------------------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
/// \file
///
/// Provides a library for accessing COM functionality of the Host OS.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM37_SUPPORT_COM_H
#define LLVM37_SUPPORT_COM_H

namespace llvm37 {
namespace sys {

enum class COMThreadingMode { SingleThreaded, MultiThreaded };

class InitializeCOMRAII {
public:
  explicit InitializeCOMRAII(COMThreadingMode Threading,
                             bool SpeedOverMemory = false);
  ~InitializeCOMRAII();

private:
  InitializeCOMRAII(const InitializeCOMRAII &) = delete;
  void operator=(const InitializeCOMRAII &) = delete;
};
}
}

#endif
