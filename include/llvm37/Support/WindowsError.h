//===-- WindowsError.h - Support for mapping windows errors to posix-------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_SUPPORT_WINDOWSERROR_H
#define LLVM37_SUPPORT_WINDOWSERROR_H

#include <system_error>

namespace llvm37 {
std::error_code mapWindowsError(unsigned EV);
}

#endif
