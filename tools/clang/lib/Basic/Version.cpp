//===- Version.cpp - Clang Version Number -----------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines several version-related utility functions for Clang.
//
//===----------------------------------------------------------------------===//

#include "clang/Basic/Version.h"
#include "clang/Basic/LLVM.h"
#include "clang/Config/config.h"
#include "llvm37/Support/raw_ostream.h"
#include <cstdlib>
#include <cstring>

#ifdef HAVE_SVN_VERSION_INC
#  include "SVNVersion.inc"
#endif

namespace clang {

std::string getClangRepositoryPath() {
#ifdef HLSL_FIXED_VER // HLSL Change Starts
  return std::string();
#else
#if defined(CLANG_REPOSITORY_STRING)
  return CLANG_REPOSITORY_STRING;
#else
#ifdef SVN_REPOSITORY
  StringRef URL(SVN_REPOSITORY);
#else
  StringRef URL("");
#endif

  // If the SVN_REPOSITORY is empty, try to use the SVN keyword. This helps us
  // pick up a tag in an SVN export, for example.
  StringRef SVNRepository("$URL: https://llvm37.org/svn/llvm37-project/cfe/tags/RELEASE_370/final/lib/Basic/Version.cpp $");
  if (URL.empty()) {
    URL = SVNRepository.slice(SVNRepository.find(':'),
                              SVNRepository.find("/lib/Basic"));
  }

  // Strip off version from a build from an integration branch.
  URL = URL.slice(0, URL.find("/src/tools/clang"));

  // Trim path prefix off, assuming path came from standard cfe path.
  size_t Start = URL.find("cfe/");
  if (Start != StringRef::npos)
    URL = URL.substr(Start + 4);

  return URL;
#endif
#endif // HLSL Change Ends
}

std::string getLLVM37RepositoryPath() {
#ifdef HLSL_FIXED_VER // HLSL Change Starts
  return std::string();
#else
#ifdef LLVM37_REPOSITORY
  StringRef URL(LLVM37_REPOSITORY);
#else
  StringRef URL("");
#endif

  // Trim path prefix off, assuming path came from standard llvm37 path.
  // Leave "llvm37/" prefix to distinguish the following llvm37 revision from the
  // clang revision.
  size_t Start = URL.find("llvm37/");
  if (Start != StringRef::npos)
    URL = URL.substr(Start);

  return URL;
#endif // HLSL Change Ends
}

std::string getClangRevision() {
#ifdef HLSL_FIXED_VER // HLSL Change Starts
  return std::string();
#else
#ifdef SVN_REVISION
  return SVN_REVISION;
#else
  return "";
#endif
#endif // HLSL Change Ends
}

std::string getLLVM37Revision() {
#ifdef HLSL_FIXED_VER // HLSL Change Starts
  return std::string();
#else
#ifdef LLVM37_REVISION
  return LLVM37_REVISION;
#else
  return "";
#endif
#endif // HLSL Change Ends
}

std::string getClangFullRepositoryVersion() {
#ifdef HLSL_FIXED_VER // HLSL Change Starts
  return std::string();
#else
  std::string buf;
  llvm37::raw_string_ostream OS(buf);
  std::string Path = getClangRepositoryPath();
  std::string Revision = getClangRevision();
  if (!Path.empty() || !Revision.empty()) {
    OS << '(';
    if (!Path.empty())
      OS << Path;
    if (!Revision.empty()) {
      if (!Path.empty())
        OS << ' ';
      OS << Revision;
    }
    OS << ')';
  }
  // Support LLVM37 in a separate repository.
  std::string LLVM37Rev = getLLVM37Revision();
  if (!LLVM37Rev.empty() && LLVM37Rev != Revision) {
    OS << " (";
    std::string LLVM37Repo = getLLVM37RepositoryPath();
    if (!LLVM37Repo.empty())
      OS << LLVM37Repo << ' ';
    OS << LLVM37Rev << ')';
  }
  return OS.str();
#endif
}

std::string getClangFullVersion() {
  return getClangToolFullVersion("clang");
}

std::string getClangToolFullVersion(StringRef ToolName) {
#ifdef HLSL_FIXED_VER // HLSL Change Starts
  // We fix a specific version for builds that are released;
  // this allows tools to pick a known version for a given !llvm37.ident value.
  return std::string(HLSL_FIXED_VER);
#else
  std::string buf;
  llvm37::raw_string_ostream OS(buf);
#ifdef CLANG_VENDOR
  OS << CLANG_VENDOR;
#endif
  OS << ToolName << " version " CLANG_VERSION_STRING " "
     << getClangFullRepositoryVersion();

  // If vendor supplied, include the base LLVM37 version as well.
#ifdef CLANG_VENDOR
  OS << " (based on " << BACKEND_PACKAGE_STRING << ")";
#endif

  return OS.str();
#endif // HLSL Change Ends
}

std::string getClangFullCPPVersion() {
#ifdef HLSL_FIXED_VER // HLSL Change Starts
  // We fix a specific version for builds that are released;
  // this allows tools to pick a known version for a given !llvm37.ident value.
  return std::string(HLSL_FIXED_VER);
#else
  // The version string we report in __VERSION__ is just a compacted version of
  // the one we report on the command line.
  std::string buf;
  llvm37::raw_string_ostream OS(buf);
#ifdef CLANG_VENDOR
  OS << CLANG_VENDOR;
#endif
  OS << "unofficial";
  return OS.str();
#endif // HLSL Change Ends
}

// HLSL Change Starts
#ifdef SUPPORT_QUERY_GIT_COMMIT_INFO
#include "GitCommitInfo.inc"
uint32_t getGitCommitCount() { return kGitCommitCount; }
const char *getGitCommitHash() { return kGitCommitHash; }
#endif // SUPPORT_QUERY_GIT_COMMIT_INFO
// HLSL Change Ends

} // end namespace clang
