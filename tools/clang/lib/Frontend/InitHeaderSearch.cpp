//===--- InitHeaderSearch.cpp - Initialize header search paths ------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the InitHeaderSearch class.
//
//===----------------------------------------------------------------------===//

#include "clang/Frontend/Utils.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Config/config.h" // C_INCLUDE_DIRS
#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/HeaderSearchOptions.h"
#include "llvm37/ADT/SmallPtrSet.h"
#include "llvm37/ADT/SmallString.h"
#include "llvm37/ADT/SmallVector.h"
#include "llvm37/ADT/StringExtras.h"
#include "llvm37/ADT/Triple.h"
#include "llvm37/ADT/Twine.h"
#include "llvm37/Support/ErrorHandling.h"
#include "llvm37/Support/FileSystem.h"
#include "llvm37/Support/Path.h"
#include "llvm37/Support/raw_ostream.h"

using namespace clang;
using namespace clang::frontend;

namespace {

/// InitHeaderSearch - This class makes it easier to set the search paths of
///  a HeaderSearch object. InitHeaderSearch stores several search path lists
///  internally, which can be sent to a HeaderSearch object in one swoop.
class InitHeaderSearch {
  std::vector<std::pair<IncludeDirGroup, DirectoryLookup> > IncludePath;
  typedef std::vector<std::pair<IncludeDirGroup,
                      DirectoryLookup> >::const_iterator path_iterator;
  std::vector<std::pair<std::string, bool> > SystemHeaderPrefixes;
  HeaderSearch &Headers;
  bool Verbose;
  std::string IncludeSysroot;
  bool HasSysroot;

public:

  InitHeaderSearch(HeaderSearch &HS, bool verbose, StringRef sysroot)
    : Headers(HS), Verbose(verbose), IncludeSysroot(sysroot),
      HasSysroot(!(sysroot.empty() || sysroot == "/")) {
  }

  /// AddPath - Add the specified path to the specified group list, prefixing
  /// the sysroot if used.
  void AddPath(const Twine &Path, IncludeDirGroup Group, bool isFramework);

  /// AddUnmappedPath - Add the specified path to the specified group list,
  /// without performing any sysroot remapping.
  void AddUnmappedPath(const Twine &Path, IncludeDirGroup Group,
                       bool isFramework);

  /// AddSystemHeaderPrefix - Add the specified prefix to the system header
  /// prefix list.
  void AddSystemHeaderPrefix(StringRef Prefix, bool IsSystemHeader) {
    SystemHeaderPrefixes.emplace_back(Prefix, IsSystemHeader);
  }

  /// AddGnuCPlusPlusIncludePaths - Add the necessary paths to support a gnu
  ///  libstdc++.
  void AddGnuCPlusPlusIncludePaths(StringRef Base,
                                   StringRef ArchDir,
                                   StringRef Dir32,
                                   StringRef Dir64,
                                   const llvm37::Triple &triple);

  /// AddMinGWCPlusPlusIncludePaths - Add the necessary paths to support a MinGW
  ///  libstdc++.
  void AddMinGWCPlusPlusIncludePaths(StringRef Base,
                                     StringRef Arch,
                                     StringRef Version);

  // AddDefaultCIncludePaths - Add paths that should always be searched.
  void AddDefaultCIncludePaths(const llvm37::Triple &triple,
                               const HeaderSearchOptions &HSOpts);

  // AddDefaultCPlusPlusIncludePaths -  Add paths that should be searched when
  //  compiling c++.
  void AddDefaultCPlusPlusIncludePaths(const llvm37::Triple &triple,
                                       const HeaderSearchOptions &HSOpts);

  /// AddDefaultSystemIncludePaths - Adds the default system include paths so
  ///  that e.g. stdio.h is found.
  void AddDefaultIncludePaths(const LangOptions &Lang,
                              const llvm37::Triple &triple,
                              const HeaderSearchOptions &HSOpts);

  /// Realize - Merges all search path lists into one list and send it to
  /// HeaderSearch.
  void Realize(const LangOptions &Lang);
};

}  // end anonymous namespace.

static bool CanPrefixSysroot(StringRef Path) {
#if defined(LLVM37_ON_WIN32)
  return !Path.empty() && llvm37::sys::path::is_separator(Path[0]);
#else
  return llvm37::sys::path::is_absolute(Path);
#endif
}

void InitHeaderSearch::AddPath(const Twine &Path, IncludeDirGroup Group,
                               bool isFramework) {
  // Add the path with sysroot prepended, if desired and this is a system header
  // group.
  if (HasSysroot) {
    SmallString<256> MappedPathStorage;
    StringRef MappedPathStr = Path.toStringRef(MappedPathStorage);
    if (CanPrefixSysroot(MappedPathStr)) {
      AddUnmappedPath(IncludeSysroot + Path, Group, isFramework);
      return;
    }
  }

  AddUnmappedPath(Path, Group, isFramework);
}

void InitHeaderSearch::AddUnmappedPath(const Twine &Path, IncludeDirGroup Group,
                                       bool isFramework) {
  assert(!Path.isTriviallyEmpty() && "can't handle empty path here");

  FileManager &FM = Headers.getFileMgr();
  SmallString<256> MappedPathStorage;
  StringRef MappedPathStr = Path.toStringRef(MappedPathStorage);

#if 1 // HLSL Change Starts - simplify mapping based on actual usage
  assert(Group == Angled);
  assert(!isFramework);

  // If the directory exists, add it.
  if (const DirectoryEntry *DE = FM.getDirectory(MappedPathStr)) {
    IncludePath.push_back(
      std::make_pair(Angled, DirectoryLookup(DE, SrcMgr::C_User, false)));
    return;
  }
#else
  // Compute the DirectoryLookup type.
  SrcMgr::CharacteristicKind Type;
  if (Group == Quoted || Group == Angled || Group == IndexHeaderMap) {
    Type = SrcMgr::C_User;
  } else if (Group == ExternCSystem) {
    Type = SrcMgr::C_ExternCSystem;
  } else {
    Type = SrcMgr::C_System;
  }

  // If the directory exists, add it.
  if (const DirectoryEntry *DE = FM.getDirectory(MappedPathStr)) {
    IncludePath.push_back(
      std::make_pair(Group, DirectoryLookup(DE, Type, isFramework)));
    return;
  }

  // Check to see if this is an apple-style headermap (which are not allowed to
  // be frameworks).
  if (!isFramework) {
    if (const FileEntry *FE = FM.getFile(MappedPathStr)) {
      if (const HeaderMap *HM = Headers.CreateHeaderMap(FE)) {
        // It is a headermap, add it to the search path.
        IncludePath.push_back(
          std::make_pair(Group,
                         DirectoryLookup(HM, Type, Group == IndexHeaderMap)));
        return;
      }
    }
  }

  if (Verbose)
    llvm37::errs() << "ignoring nonexistent directory \""
                 << MappedPathStr << "\"\n";
#endif // HLSL Change Ends - simplify mapping based on actual usage
}

void InitHeaderSearch::AddGnuCPlusPlusIncludePaths(StringRef Base,
                                                   StringRef ArchDir,
                                                   StringRef Dir32,
                                                   StringRef Dir64,
                                                   const llvm37::Triple &triple) {
  // Add the base dir
  AddPath(Base, CXXSystem, false);

  // Add the multilib dirs
  llvm37::Triple::ArchType arch = triple.getArch();
  bool is64bit = arch == llvm37::Triple::ppc64 || arch == llvm37::Triple::x86_64;
  if (is64bit)
    AddPath(Base + "/" + ArchDir + "/" + Dir64, CXXSystem, false);
  else
    AddPath(Base + "/" + ArchDir + "/" + Dir32, CXXSystem, false);

  // Add the backward dir
  AddPath(Base + "/backward", CXXSystem, false);
}

void InitHeaderSearch::AddMinGWCPlusPlusIncludePaths(StringRef Base,
                                                     StringRef Arch,
                                                     StringRef Version) {
  AddPath(Base + "/" + Arch + "/" + Version + "/include/c++",
          CXXSystem, false);
  AddPath(Base + "/" + Arch + "/" + Version + "/include/c++/" + Arch,
          CXXSystem, false);
  AddPath(Base + "/" + Arch + "/" + Version + "/include/c++/backward",
          CXXSystem, false);
}

void InitHeaderSearch::AddDefaultCIncludePaths(const llvm37::Triple &triple,
                                            const HeaderSearchOptions &HSOpts) {
  // HLSL Change Starts - no standard include files for the HLSL/DXIL
#if 1
  return;
#else
  llvm37::Triple::OSType os = triple.getOS();

  if (HSOpts.UseStandardSystemIncludes) {
    switch (os) {
    case llvm37::Triple::CloudABI:
    case llvm37::Triple::FreeBSD:
    case llvm37::Triple::NetBSD:
    case llvm37::Triple::OpenBSD:
    case llvm37::Triple::Bitrig:
    case llvm37::Triple::NaCl:
      break;
    case llvm37::Triple::Win32:
      if (triple.getEnvironment() != llvm37::Triple::Cygnus)
        break;
    default:
      // FIXME: temporary hack: hard-coded paths.
      AddPath("/usr/local/include", System, false);
      break;
    }
  }

  // Builtin includes use #include_next directives and should be positioned
  // just prior C include dirs.
  if (HSOpts.UseBuiltinIncludes) {
    // Ignore the sys root, we *always* look for clang headers relative to
    // supplied path.
    SmallString<128> P = StringRef(HSOpts.ResourceDir);
    llvm37::sys::path::append(P, "include");
    AddUnmappedPath(P, ExternCSystem, false);
  }

  // All remaining additions are for system include directories, early exit if
  // we aren't using them.
  if (!HSOpts.UseStandardSystemIncludes)
    return;

  // Add dirs specified via 'configure --with-c-include-dirs'.
  StringRef CIncludeDirs(C_INCLUDE_DIRS);
  if (CIncludeDirs != "") {
    SmallVector<StringRef, 5> dirs;
    CIncludeDirs.split(dirs, ":");
    for (SmallVectorImpl<StringRef>::iterator i = dirs.begin();
         i != dirs.end();
         ++i)
      AddPath(*i, ExternCSystem, false);
    return;
  }

  switch (os) {
  case llvm37::Triple::Linux:
    llvm37_unreachable("Include management is handled in the driver.");

  case llvm37::Triple::CloudABI: {
    // <sysroot>/<triple>/include
    SmallString<128> P = StringRef(HSOpts.ResourceDir);
    llvm37::sys::path::append(P, "../../..", triple.str(), "include");
    AddPath(P, System, false);
    break;
  }

  case llvm37::Triple::Haiku:
    AddPath("/boot/common/include", System, false);
    AddPath("/boot/develop/headers/os", System, false);
    AddPath("/boot/develop/headers/os/app", System, false);
    AddPath("/boot/develop/headers/os/arch", System, false);
    AddPath("/boot/develop/headers/os/device", System, false);
    AddPath("/boot/develop/headers/os/drivers", System, false);
    AddPath("/boot/develop/headers/os/game", System, false);
    AddPath("/boot/develop/headers/os/interface", System, false);
    AddPath("/boot/develop/headers/os/kernel", System, false);
    AddPath("/boot/develop/headers/os/locale", System, false);
    AddPath("/boot/develop/headers/os/mail", System, false);
    AddPath("/boot/develop/headers/os/media", System, false);
    AddPath("/boot/develop/headers/os/midi", System, false);
    AddPath("/boot/develop/headers/os/midi2", System, false);
    AddPath("/boot/develop/headers/os/net", System, false);
    AddPath("/boot/develop/headers/os/storage", System, false);
    AddPath("/boot/develop/headers/os/support", System, false);
    AddPath("/boot/develop/headers/os/translation", System, false);
    AddPath("/boot/develop/headers/os/add-ons/graphics", System, false);
    AddPath("/boot/develop/headers/os/add-ons/input_server", System, false);
    AddPath("/boot/develop/headers/os/add-ons/screen_saver", System, false);
    AddPath("/boot/develop/headers/os/add-ons/tracker", System, false);
    AddPath("/boot/develop/headers/os/be_apps/Deskbar", System, false);
    AddPath("/boot/develop/headers/os/be_apps/NetPositive", System, false);
    AddPath("/boot/develop/headers/os/be_apps/Tracker", System, false);
    AddPath("/boot/develop/headers/cpp", System, false);
    AddPath("/boot/develop/headers/cpp/i586-pc-haiku", System, false);
    AddPath("/boot/develop/headers/3rdparty", System, false);
    AddPath("/boot/develop/headers/bsd", System, false);
    AddPath("/boot/develop/headers/glibc", System, false);
    AddPath("/boot/develop/headers/posix", System, false);
    AddPath("/boot/develop/headers",  System, false);
    break;
  case llvm37::Triple::RTEMS:
    break;
  case llvm37::Triple::Win32:
    switch (triple.getEnvironment()) {
    default: llvm37_unreachable("Include management is handled in the driver.");
    case llvm37::Triple::Cygnus:
      AddPath("/usr/include/w32api", System, false);
      break;
    case llvm37::Triple::GNU:
      break;
    }
    break;
  default:
    break;
  }

  switch (os) {
  case llvm37::Triple::CloudABI:
  case llvm37::Triple::RTEMS:
  case llvm37::Triple::NaCl:
    break;
  default:
    AddPath("/usr/include", ExternCSystem, false);
    break;
  }
#endif // HLSL Change Ends
}

void InitHeaderSearch::
AddDefaultCPlusPlusIncludePaths(const llvm37::Triple &triple, const HeaderSearchOptions &HSOpts) {
  // HLSL Change Starts - no standard include files for the HLSL/DXIL
#if 1
  return;
#else
  llvm37::Triple::OSType os = triple.getOS();
  // FIXME: temporary hack: hard-coded paths.

  if (triple.isOSDarwin()) {
    switch (triple.getArch()) {
    default: break;

    case llvm37::Triple::ppc:
    case llvm37::Triple::ppc64:
      AddGnuCPlusPlusIncludePaths("/usr/include/c++/4.2.1",
                                  "powerpc-apple-darwin10", "", "ppc64",
                                  triple);
      AddGnuCPlusPlusIncludePaths("/usr/include/c++/4.0.0",
                                  "powerpc-apple-darwin10", "", "ppc64",
                                  triple);
      break;

    case llvm37::Triple::x86:
    case llvm37::Triple::x86_64:
      AddGnuCPlusPlusIncludePaths("/usr/include/c++/4.2.1",
                                  "i686-apple-darwin10", "", "x86_64", triple);
      AddGnuCPlusPlusIncludePaths("/usr/include/c++/4.0.0",
                                  "i686-apple-darwin8", "", "", triple);
      break;

    case llvm37::Triple::arm:
    case llvm37::Triple::thumb:
      AddGnuCPlusPlusIncludePaths("/usr/include/c++/4.2.1",
                                  "arm-apple-darwin10", "v7", "", triple);
      AddGnuCPlusPlusIncludePaths("/usr/include/c++/4.2.1",
                                  "arm-apple-darwin10", "v6", "", triple);
      break;

    case llvm37::Triple::aarch64:
      AddGnuCPlusPlusIncludePaths("/usr/include/c++/4.2.1",
                                  "arm64-apple-darwin10", "", "", triple);
      break;
    }
    return;
  }

  switch (os) {
  case llvm37::Triple::Linux:
    llvm37_unreachable("Include management is handled in the driver.");
    break;
  case llvm37::Triple::Win32:
    switch (triple.getEnvironment()) {
    default: llvm37_unreachable("Include management is handled in the driver.");
    case llvm37::Triple::Cygnus:
      // Cygwin-1.7
      AddMinGWCPlusPlusIncludePaths("/usr/lib/gcc", "i686-pc-cygwin", "4.7.3");
      AddMinGWCPlusPlusIncludePaths("/usr/lib/gcc", "i686-pc-cygwin", "4.5.3");
      AddMinGWCPlusPlusIncludePaths("/usr/lib/gcc", "i686-pc-cygwin", "4.3.4");
      // g++-4 / Cygwin-1.5
      AddMinGWCPlusPlusIncludePaths("/usr/lib/gcc", "i686-pc-cygwin", "4.3.2");
      break;
    }
    break;
  case llvm37::Triple::DragonFly:
    if (llvm37::sys::fs::exists("/usr/lib/gcc47"))
      AddPath("/usr/include/c++/4.7", CXXSystem, false);
    else
      AddPath("/usr/include/c++/4.4", CXXSystem, false);
    break;
  case llvm37::Triple::OpenBSD: {
    std::string t = triple.getTriple();
    if (t.substr(0, 6) == "x86_64")
      t.replace(0, 6, "amd64");
    AddGnuCPlusPlusIncludePaths("/usr/include/g++",
                                t, "", "", triple);
    break;
  }
  case llvm37::Triple::Minix:
    AddGnuCPlusPlusIncludePaths("/usr/gnu/include/c++/4.4.3",
                                "", "", "", triple);
    break;
  case llvm37::Triple::Solaris:
    AddGnuCPlusPlusIncludePaths("/usr/gcc/4.5/include/c++/4.5.2/",
                                "i386-pc-solaris2.11", "", "", triple);
    break;
  default:
    break;
  }
#endif // HLSL Change Ends
}

void InitHeaderSearch::AddDefaultIncludePaths(const LangOptions &Lang,
                                              const llvm37::Triple &triple,
                                            const HeaderSearchOptions &HSOpts) {
#if 1 // HLSL Change Starts
  return;
#else
  // NB: This code path is going away. All of the logic is moving into the
  // driver which has the information necessary to do target-specific
  // selections of default include paths. Each target which moves there will be
  // exempted from this logic here until we can delete the entire pile of code.
  switch (triple.getOS()) {
  default:
    break; // Everything else continues to use this routine's logic.

  case llvm37::Triple::Linux:
    return;

  case llvm37::Triple::Win32:
    if (triple.getEnvironment() != llvm37::Triple::Cygnus ||
        triple.isOSBinFormatMachO())
      return;
    break;
  }

  if (Lang.CPlusPlus && HSOpts.UseStandardCXXIncludes &&
      HSOpts.UseStandardSystemIncludes) {
    if (HSOpts.UseLibcxx) {
      if (triple.isOSDarwin()) {
        // On Darwin, libc++ may be installed alongside the compiler in
        // include/c++/v1.
        if (!HSOpts.ResourceDir.empty()) {
          // Remove version from foo/lib/clang/version
          StringRef NoVer = llvm37::sys::path::parent_path(HSOpts.ResourceDir);
          // Remove clang from foo/lib/clang
          StringRef Lib = llvm37::sys::path::parent_path(NoVer);
          // Remove lib from foo/lib
          SmallString<128> P = llvm37::sys::path::parent_path(Lib);

          // Get foo/include/c++/v1
          llvm37::sys::path::append(P, "include", "c++", "v1");
          AddUnmappedPath(P, CXXSystem, false);
        }
      }
      // On Solaris, include the support directory for things like xlocale and
      // fudged system headers.
      if (triple.getOS() == llvm37::Triple::Solaris) 
        AddPath("/usr/include/c++/v1/support/solaris", CXXSystem, false);
      
      AddPath("/usr/include/c++/v1", CXXSystem, false);
    } else {
      AddDefaultCPlusPlusIncludePaths(triple, HSOpts);
    }
  }

  AddDefaultCIncludePaths(triple, HSOpts);

  // Add the default framework include paths on Darwin.
  if (HSOpts.UseStandardSystemIncludes) {
    if (triple.isOSDarwin()) {
      AddPath("/System/Library/Frameworks", System, true);
      AddPath("/Library/Frameworks", System, true);
    }
  }
#endif // HLSL Change Ends
}

/// RemoveDuplicates - If there are duplicate directory entries in the specified
/// search list, remove the later (dead) ones.  Returns the number of non-system
/// headers removed, which is used to update NumAngled.
static unsigned RemoveDuplicates(std::vector<DirectoryLookup> &SearchList,
                                 unsigned First, bool Verbose) {
  llvm37::SmallPtrSet<const DirectoryEntry *, 8> SeenDirs;
  llvm37::SmallPtrSet<const DirectoryEntry *, 8> SeenFrameworkDirs;
  llvm37::SmallPtrSet<const HeaderMap *, 8> SeenHeaderMaps;
  unsigned NonSystemRemoved = 0;
  for (unsigned i = First; i != SearchList.size(); ++i) {
    unsigned DirToRemove = i;

    const DirectoryLookup &CurEntry = SearchList[i];

    if (CurEntry.isNormalDir()) {
      // If this isn't the first time we've seen this dir, remove it.
      if (SeenDirs.insert(CurEntry.getDir()).second)
        continue;
    } else if (CurEntry.isFramework()) {
      // If this isn't the first time we've seen this framework dir, remove it.
      if (SeenFrameworkDirs.insert(CurEntry.getFrameworkDir()).second)
        continue;
    } else {
      assert(CurEntry.isHeaderMap() && "Not a headermap or normal dir?");
      // If this isn't the first time we've seen this headermap, remove it.
      if (SeenHeaderMaps.insert(CurEntry.getHeaderMap()).second)
        continue;
    }

    // If we have a normal #include dir/framework/headermap that is shadowed
    // later in the chain by a system include location, we actually want to
    // ignore the user's request and drop the user dir... keeping the system
    // dir.  This is weird, but required to emulate GCC's search path correctly.
    //
    // Since dupes of system dirs are rare, just rescan to find the original
    // that we're nuking instead of using a DenseMap.
    if (CurEntry.getDirCharacteristic() != SrcMgr::C_User) {
      // Find the dir that this is the same of.
      unsigned FirstDir;
      for (FirstDir = 0; ; ++FirstDir) {
        assert(FirstDir != i && "Didn't find dupe?");

        const DirectoryLookup &SearchEntry = SearchList[FirstDir];

        // If these are different lookup types, then they can't be the dupe.
        if (SearchEntry.getLookupType() != CurEntry.getLookupType())
          continue;

        bool isSame;
        if (CurEntry.isNormalDir())
          isSame = SearchEntry.getDir() == CurEntry.getDir();
        else if (CurEntry.isFramework())
          isSame = SearchEntry.getFrameworkDir() == CurEntry.getFrameworkDir();
        else {
          assert(CurEntry.isHeaderMap() && "Not a headermap or normal dir?");
          isSame = SearchEntry.getHeaderMap() == CurEntry.getHeaderMap();
        }

        if (isSame)
          break;
      }

      // If the first dir in the search path is a non-system dir, zap it
      // instead of the system one.
      if (SearchList[FirstDir].getDirCharacteristic() == SrcMgr::C_User)
        DirToRemove = FirstDir;
    }

    if (Verbose) {
      llvm37::errs() << "ignoring duplicate directory \""
                   << CurEntry.getName() << "\"\n";
      if (DirToRemove != i)
        llvm37::errs() << "  as it is a non-system directory that duplicates "
                     << "a system directory\n";
    }
    if (DirToRemove != i)
      ++NonSystemRemoved;

    // This is reached if the current entry is a duplicate.  Remove the
    // DirToRemove (usually the current dir).
    SearchList.erase(SearchList.begin()+DirToRemove);
    --i;
  }
  return NonSystemRemoved;
}


void InitHeaderSearch::Realize(const LangOptions &Lang) {
  // Concatenate ANGLE+SYSTEM+AFTER chains together into SearchList.
  std::vector<DirectoryLookup> SearchList;
  SearchList.reserve(IncludePath.size());

  // Quoted arguments go first.
  for (path_iterator it = IncludePath.begin(), ie = IncludePath.end();
       it != ie; ++it) {
    if (it->first == Quoted)
      SearchList.push_back(it->second);
  }
  // Deduplicate and remember index.
  RemoveDuplicates(SearchList, 0, Verbose);
  unsigned NumQuoted = SearchList.size();

  for (path_iterator it = IncludePath.begin(), ie = IncludePath.end();
       it != ie; ++it) {
    if (it->first == Angled || it->first == IndexHeaderMap)
      SearchList.push_back(it->second);
  }

  RemoveDuplicates(SearchList, NumQuoted, Verbose);
  unsigned NumAngled = SearchList.size();

  for (path_iterator it = IncludePath.begin(), ie = IncludePath.end();
       it != ie; ++it) {
    if (it->first == System || it->first == ExternCSystem ||
        (!Lang.ObjC1 && !Lang.CPlusPlus && it->first == CSystem)    ||
        (/*FIXME !Lang.ObjC1 && */Lang.CPlusPlus  && it->first == CXXSystem)  ||
        (Lang.ObjC1  && !Lang.CPlusPlus && it->first == ObjCSystem) ||
        (Lang.ObjC1  && Lang.CPlusPlus  && it->first == ObjCXXSystem))
      SearchList.push_back(it->second);
  }

  for (path_iterator it = IncludePath.begin(), ie = IncludePath.end();
       it != ie; ++it) {
    if (it->first == After)
      SearchList.push_back(it->second);
  }

  // Remove duplicates across both the Angled and System directories.  GCC does
  // this and failing to remove duplicates across these two groups breaks
  // #include_next.
  unsigned NonSystemRemoved = RemoveDuplicates(SearchList, NumQuoted, Verbose);
  NumAngled -= NonSystemRemoved;

  bool DontSearchCurDir = false;  // TODO: set to true if -I- is set?
  Headers.SetSearchPaths(SearchList, NumQuoted, NumAngled, DontSearchCurDir);

  Headers.SetSystemHeaderPrefixes(SystemHeaderPrefixes);

  // If verbose, print the list of directories that will be searched.
  if (Verbose) {
    llvm37::errs() << "#include \"...\" search starts here:\n";
    for (unsigned i = 0, e = SearchList.size(); i != e; ++i) {
      if (i == NumQuoted)
        llvm37::errs() << "#include <...> search starts here:\n";
      const char *Name = SearchList[i].getName();
      const char *Suffix;
      if (SearchList[i].isNormalDir())
        Suffix = "";
      else if (SearchList[i].isFramework())
        Suffix = " (framework directory)";
      else {
        assert(SearchList[i].isHeaderMap() && "Unknown DirectoryLookup");
        Suffix = " (headermap)";
      }
      llvm37::errs() << " " << Name << Suffix << "\n";
    }
    llvm37::errs() << "End of search list.\n";
  }
}

void clang::ApplyHeaderSearchOptions(HeaderSearch &HS,
                                     const HeaderSearchOptions &HSOpts,
                                     const LangOptions &Lang,
                                     const llvm37::Triple &Triple) {
  InitHeaderSearch Init(HS, HSOpts.Verbose, HSOpts.Sysroot);

  // Add the user defined entries.
  for (unsigned i = 0, e = HSOpts.UserEntries.size(); i != e; ++i) {
    const HeaderSearchOptions::Entry &E = HSOpts.UserEntries[i];
    if (E.IgnoreSysRoot) {
      Init.AddUnmappedPath(E.Path, E.Group, E.IsFramework);
    } else {
      Init.AddPath(E.Path, E.Group, E.IsFramework);
    }
  }

  Init.AddDefaultIncludePaths(Lang, Triple, HSOpts);

  for (unsigned i = 0, e = HSOpts.SystemHeaderPrefixes.size(); i != e; ++i)
    Init.AddSystemHeaderPrefix(HSOpts.SystemHeaderPrefixes[i].Prefix,
                               HSOpts.SystemHeaderPrefixes[i].IsSystemHeader);

  if (HSOpts.UseBuiltinIncludes) {
    // Set up the builtin include directory in the module map.
    SmallString<128> P = StringRef(HSOpts.ResourceDir);
    llvm37::sys::path::append(P, "include");
    if (const DirectoryEntry *Dir = HS.getFileMgr().getDirectory(P))
      HS.getModuleMap().setBuiltinIncludeDir(Dir);
  }

  Init.Realize(Lang);
}
