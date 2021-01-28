//===- llvm37/Support/Unix/Host.inc -------------------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the UNIX Host support.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//=== WARNING: Implementation here must contain only generic UNIX code that
//===          is guaranteed to work on *all* UNIX variants.
//===----------------------------------------------------------------------===//

#include "Unix.h"
#include "llvm37/ADT/StringRef.h"
#include "llvm37/Config/config.h"
#include <cctype>
#include <string>
#include <sys/utsname.h>

using namespace llvm37;

static std::string getOSVersion() {
  struct utsname info;

  if (uname(&info))
    return "";

  return info.release;
}

std::string sys::getDefaultTargetTriple() {
  std::string TargetTripleString(LLVM37_DEFAULT_TARGET_TRIPLE);

  // On darwin, we want to update the version to match that of the
  // target.
  std::string::size_type DarwinDashIdx = TargetTripleString.find("-darwin");
  if (DarwinDashIdx != std::string::npos) {
    TargetTripleString.resize(DarwinDashIdx + strlen("-darwin"));
    TargetTripleString += getOSVersion();
  }

  return Triple::normalize(TargetTripleString);
}
