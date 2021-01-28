//===-- llvm37-dwarfdump.cpp - Debug info dumping utility for llvm37 ----------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This program is a utility that works like "dwarfdump".
//
//===----------------------------------------------------------------------===//

#include "llvm37/ADT/STLExtras.h"
#include "llvm37/ADT/Triple.h"
#include "llvm37/DebugInfo/DIContext.h"
#include "llvm37/DebugInfo/DWARF/DWARFContext.h"
#include "llvm37/Object/ObjectFile.h"
#include "llvm37/Object/RelocVisitor.h"
#include "llvm37/Support/CommandLine.h"
#include "llvm37/Support/Debug.h"
#include "llvm37/Support/Format.h"
#include "llvm37/Support/ManagedStatic.h"
#include "llvm37/Support/MemoryBuffer.h"
#include "llvm37/Support/PrettyStackTrace.h"
#include "llvm37/Support/Signals.h"
#include "llvm37/Support/raw_ostream.h"
#include <algorithm>
#include <cstring>
#include <list>
#include <string>
#include <system_error>

using namespace llvm37;
using namespace object;

static cl::list<std::string>
InputFilenames(cl::Positional, cl::desc("<input object files>"),
               cl::ZeroOrMore);

static cl::opt<DIDumpType>
DumpType("debug-dump", cl::init(DIDT_All),
  cl::desc("Dump of debug sections:"),
  cl::values(
        clEnumValN(DIDT_All, "all", "Dump all debug sections"),
        clEnumValN(DIDT_Abbrev, "abbrev", ".debug_abbrev"),
        clEnumValN(DIDT_AbbrevDwo, "abbrev.dwo", ".debug_abbrev.dwo"),
        clEnumValN(DIDT_AppleNames, "apple_names", ".apple_names"),
        clEnumValN(DIDT_AppleTypes, "apple_types", ".apple_types"),
        clEnumValN(DIDT_AppleNamespaces, "apple_namespaces", ".apple_namespaces"),
        clEnumValN(DIDT_AppleObjC, "apple_objc", ".apple_objc"),
        clEnumValN(DIDT_Aranges, "aranges", ".debug_aranges"),
        clEnumValN(DIDT_Info, "info", ".debug_info"),
        clEnumValN(DIDT_InfoDwo, "info.dwo", ".debug_info.dwo"),
        clEnumValN(DIDT_Types, "types", ".debug_types"),
        clEnumValN(DIDT_TypesDwo, "types.dwo", ".debug_types.dwo"),
        clEnumValN(DIDT_Line, "line", ".debug_line"),
        clEnumValN(DIDT_LineDwo, "line.dwo", ".debug_line.dwo"),
        clEnumValN(DIDT_Loc, "loc", ".debug_loc"),
        clEnumValN(DIDT_LocDwo, "loc.dwo", ".debug_loc.dwo"),
        clEnumValN(DIDT_Frames, "frames", ".debug_frame"),
        clEnumValN(DIDT_Ranges, "ranges", ".debug_ranges"),
        clEnumValN(DIDT_Pubnames, "pubnames", ".debug_pubnames"),
        clEnumValN(DIDT_Pubtypes, "pubtypes", ".debug_pubtypes"),
        clEnumValN(DIDT_GnuPubnames, "gnu_pubnames", ".debug_gnu_pubnames"),
        clEnumValN(DIDT_GnuPubtypes, "gnu_pubtypes", ".debug_gnu_pubtypes"),
        clEnumValN(DIDT_Str, "str", ".debug_str"),
        clEnumValN(DIDT_StrDwo, "str.dwo", ".debug_str.dwo"),
        clEnumValN(DIDT_StrOffsetsDwo, "str_offsets.dwo", ".debug_str_offsets.dwo"),
        clEnumValEnd));

static int ReturnValue = EXIT_SUCCESS;

static bool error(StringRef Filename, std::error_code EC) {
  if (!EC)
    return false;
  errs() << Filename << ": " << EC.message() << "\n";
  ReturnValue = EXIT_FAILURE;
  return true;
}

static void DumpInput(StringRef Filename) {
  ErrorOr<std::unique_ptr<MemoryBuffer>> BuffOrErr =
      MemoryBuffer::getFileOrSTDIN(Filename);
  if (error(Filename, BuffOrErr.getError()))
    return;
  std::unique_ptr<MemoryBuffer> Buff = std::move(BuffOrErr.get());

  ErrorOr<std::unique_ptr<ObjectFile>> ObjOrErr =
      ObjectFile::createObjectFile(Buff->getMemBufferRef());
  if (error(Filename, ObjOrErr.getError()))
    return;
  ObjectFile &Obj = *ObjOrErr.get();

  std::unique_ptr<DIContext> DICtx(new DWARFContextInMemory(Obj));

  outs() << Filename
         << ":\tfile format " << Obj.getFileFormatName() << "\n\n";
  // Dump the complete DWARF structure.
  DICtx->dump(outs(), DumpType);
}

// HLSL Change: changed calling convention to __cdecl
int __cdecl main(int argc, char **argv) {
  // Print a stack trace if we signal out.
  sys::PrintStackTraceOnErrorSignal();
  PrettyStackTraceProgram X(argc, argv);
  llvm37_shutdown_obj Y;  // Call llvm37_shutdown() on exit.

  cl::ParseCommandLineOptions(argc, argv, "llvm37 dwarf dumper\n");

  // Defaults to a.out if no filenames specified.
  if (InputFilenames.size() == 0)
    InputFilenames.push_back("a.out");

  std::for_each(InputFilenames.begin(), InputFilenames.end(), DumpInput);

  return ReturnValue;
}
