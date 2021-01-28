//===-- TargetMachine.cpp -------------------------------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the LLVM37-C part of TargetMachine.h
//
//===----------------------------------------------------------------------===//

#include "llvm37-c/TargetMachine.h"
#include "llvm37-c/Core.h"
#include "llvm37-c/Target.h"
#include "llvm37/Analysis/TargetTransformInfo.h"
#include "llvm37/IR/DataLayout.h"
#include "llvm37/IR/Module.h"
#include "llvm37/IR/LegacyPassManager.h"
#include "llvm37/Support/CodeGen.h"
#include "llvm37/Support/FileSystem.h"
#include "llvm37/Support/FormattedStream.h"
#include "llvm37/Support/Host.h"
#include "llvm37/Support/TargetRegistry.h"
#include "llvm37/Support/raw_ostream.h"
#include "llvm37/Target/TargetMachine.h"
#include "llvm37/Target/TargetSubtargetInfo.h"
#include <cassert>
#include <cstdlib>
#include <cstring>

using namespace llvm37;

inline TargetMachine *unwrap(LLVM37TargetMachineRef P) {
  return reinterpret_cast<TargetMachine*>(P);
}
inline Target *unwrap(LLVM37TargetRef P) {
  return reinterpret_cast<Target*>(P);
}
inline LLVM37TargetMachineRef wrap(const TargetMachine *P) {
  return
    reinterpret_cast<LLVM37TargetMachineRef>(const_cast<TargetMachine*>(P));
}
inline LLVM37TargetRef wrap(const Target * P) {
  return reinterpret_cast<LLVM37TargetRef>(const_cast<Target*>(P));
}

LLVM37TargetRef LLVM37GetFirstTarget() {
  if (TargetRegistry::targets().begin() == TargetRegistry::targets().end()) {
    return nullptr;
  }

  const Target *target = &*TargetRegistry::targets().begin();
  return wrap(target);
}
LLVM37TargetRef LLVM37GetNextTarget(LLVM37TargetRef T) {
  return wrap(unwrap(T)->getNext());
}

LLVM37TargetRef LLVM37GetTargetFromName(const char *Name) {
  StringRef NameRef = Name;
  auto I = std::find_if(
      TargetRegistry::targets().begin(), TargetRegistry::targets().end(),
      [&](const Target &T) { return T.getName() == NameRef; });
  return I != TargetRegistry::targets().end() ? wrap(&*I) : nullptr;
}

LLVM37Bool LLVM37GetTargetFromTriple(const char* TripleStr, LLVM37TargetRef *T,
                                 char **ErrorMessage) {
  std::string Error;
  
  *T = wrap(TargetRegistry::lookupTarget(TripleStr, Error));
  
  if (!*T) {
    if (ErrorMessage)
      *ErrorMessage = strdup(Error.c_str());

    return 1;
  }
  
  return 0;
}

const char * LLVM37GetTargetName(LLVM37TargetRef T) {
  return unwrap(T)->getName();
}

const char * LLVM37GetTargetDescription(LLVM37TargetRef T) {
  return unwrap(T)->getShortDescription();
}

LLVM37Bool LLVM37TargetHasJIT(LLVM37TargetRef T) {
  return unwrap(T)->hasJIT();
}

LLVM37Bool LLVM37TargetHasTargetMachine(LLVM37TargetRef T) {
  return unwrap(T)->hasTargetMachine();
}

LLVM37Bool LLVM37TargetHasAsmBackend(LLVM37TargetRef T) {
  return unwrap(T)->hasMCAsmBackend();
}

LLVM37TargetMachineRef LLVM37CreateTargetMachine(LLVM37TargetRef T,
        const char* Triple, const char* CPU, const char* Features,
        LLVM37CodeGenOptLevel Level, LLVM37RelocMode Reloc,
        LLVM37CodeModel CodeModel) {
  Reloc::Model RM;
  switch (Reloc){
    case LLVM37RelocStatic:
      RM = Reloc::Static;
      break;
    case LLVM37RelocPIC:
      RM = Reloc::PIC_;
      break;
    case LLVM37RelocDynamicNoPic:
      RM = Reloc::DynamicNoPIC;
      break;
    default:
      RM = Reloc::Default;
      break;
  }

  CodeModel::Model CM = unwrap(CodeModel);

  CodeGenOpt::Level OL;
  switch (Level) {
    case LLVM37CodeGenLevelNone:
      OL = CodeGenOpt::None;
      break;
    case LLVM37CodeGenLevelLess:
      OL = CodeGenOpt::Less;
      break;
    case LLVM37CodeGenLevelAggressive:
      OL = CodeGenOpt::Aggressive;
      break;
    default:
      OL = CodeGenOpt::Default;
      break;
  }

  TargetOptions opt;
  return wrap(unwrap(T)->createTargetMachine(Triple, CPU, Features, opt, RM,
    CM, OL));
}


void LLVM37DisposeTargetMachine(LLVM37TargetMachineRef T) {
  delete unwrap(T);
}

LLVM37TargetRef LLVM37GetTargetMachineTarget(LLVM37TargetMachineRef T) {
  const Target* target = &(unwrap(T)->getTarget());
  return wrap(target);
}

char* LLVM37GetTargetMachineTriple(LLVM37TargetMachineRef T) {
  std::string StringRep = unwrap(T)->getTargetTriple().str();
  return strdup(StringRep.c_str());
}

char* LLVM37GetTargetMachineCPU(LLVM37TargetMachineRef T) {
  std::string StringRep = unwrap(T)->getTargetCPU();
  return strdup(StringRep.c_str());
}

char* LLVM37GetTargetMachineFeatureString(LLVM37TargetMachineRef T) {
  std::string StringRep = unwrap(T)->getTargetFeatureString();
  return strdup(StringRep.c_str());
}

LLVM37TargetDataRef LLVM37GetTargetMachineData(LLVM37TargetMachineRef T) {
  return wrap(unwrap(T)->getDataLayout());
}

void LLVM37SetTargetMachineAsmVerbosity(LLVM37TargetMachineRef T,
                                      LLVM37Bool VerboseAsm) {
  unwrap(T)->Options.MCOptions.AsmVerbose = VerboseAsm;
}

static LLVM37Bool LLVM37TargetMachineEmit(LLVM37TargetMachineRef T, LLVM37ModuleRef M,
                                      raw_pwrite_stream &OS,
                                      LLVM37CodeGenFileType codegen,
                                      char **ErrorMessage) {
  TargetMachine* TM = unwrap(T);
  Module* Mod = unwrap(M);

  legacy::PassManager pass;

  std::string error;

  const DataLayout *td = TM->getDataLayout();

  if (!td) {
    error = "No DataLayout in TargetMachine";
    *ErrorMessage = strdup(error.c_str());
    return true;
  }
  Mod->setDataLayout(*td);

  TargetMachine::CodeGenFileType ft;
  switch (codegen) {
    case LLVM37AssemblyFile:
      ft = TargetMachine::CGFT_AssemblyFile;
      break;
    default:
      ft = TargetMachine::CGFT_ObjectFile;
      break;
  }
  if (TM->addPassesToEmitFile(pass, OS, ft)) {
    error = "TargetMachine can't emit a file of this type";
    *ErrorMessage = strdup(error.c_str());
    return true;
  }

  pass.run(*Mod);

  OS.flush();
  return false;
}

LLVM37Bool LLVM37TargetMachineEmitToFile(LLVM37TargetMachineRef T, LLVM37ModuleRef M,
  char* Filename, LLVM37CodeGenFileType codegen, char** ErrorMessage) {
  std::error_code EC;
  raw_fd_ostream dest(Filename, EC, sys::fs::F_None);
  if (EC) {
    *ErrorMessage = strdup(EC.message().c_str());
    return true;
  }
  bool Result = LLVM37TargetMachineEmit(T, M, dest, codegen, ErrorMessage);
  dest.flush();
  return Result;
}

LLVM37Bool LLVM37TargetMachineEmitToMemoryBuffer(LLVM37TargetMachineRef T,
  LLVM37ModuleRef M, LLVM37CodeGenFileType codegen, char** ErrorMessage,
  LLVM37MemoryBufferRef *OutMemBuf) {
  SmallString<0> CodeString;
  raw_svector_ostream OStream(CodeString);
  bool Result = LLVM37TargetMachineEmit(T, M, OStream, codegen, ErrorMessage);
  OStream.flush();

  StringRef Data = OStream.str();
  *OutMemBuf =
      LLVM37CreateMemoryBufferWithMemoryRangeCopy(Data.data(), Data.size(), "");
  return Result;
}

#if 0 // HLSL Change Starts - DXIL has a fixed triple
char *LLVM37GetDefaultTargetTriple(void) {
  return strdup(sys::getDefaultTargetTriple().c_str());
}
#endif // HLSL Change Ends

void LLVM37AddAnalysisPasses(LLVM37TargetMachineRef T, LLVM37PassManagerRef PM) {
  unwrap(PM)->add(
      createTargetTransformInfoWrapperPass(unwrap(T)->getTargetIRAnalysis()));
}
