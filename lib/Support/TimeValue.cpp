//===-- TimeValue.cpp - Implement OS TimeValue Concept ----------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file implements the operating system TimeValue concept.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Support/TimeValue.h"
#include "llvm37/Config/config.h"

namespace llvm37 {
using namespace sys;

const TimeValue::SecondsType
  TimeValue::PosixZeroTimeSeconds = -946684800;
const TimeValue::SecondsType
  TimeValue::Win32ZeroTimeSeconds = -12591158400ULL;

void
TimeValue::normalize( void ) {
  if ( nanos_ >= NANOSECONDS_PER_SECOND ) {
    do {
      seconds_++;
      nanos_ -= NANOSECONDS_PER_SECOND;
    } while ( nanos_ >= NANOSECONDS_PER_SECOND );
  } else if (nanos_ <= -NANOSECONDS_PER_SECOND ) {
    do {
      seconds_--;
      nanos_ += NANOSECONDS_PER_SECOND;
    } while (nanos_ <= -NANOSECONDS_PER_SECOND);
  }

  if (seconds_ >= 1 && nanos_ < 0) {
    seconds_--;
    nanos_ += NANOSECONDS_PER_SECOND;
  } else if (seconds_ < 0 && nanos_ > 0) {
    seconds_++;
    nanos_ -= NANOSECONDS_PER_SECOND;
  }
}

}

/// Include the platform-specific portion of TimeValue class
#ifdef LLVM37_ON_UNIX
#include "Unix/TimeValue.inc"
#endif
#ifdef LLVM37_ON_WIN32
#include "Windows/TimeValue.inc"
#endif
