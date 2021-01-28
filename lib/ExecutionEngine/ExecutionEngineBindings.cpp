//===-- ExecutionEngineBindings.cpp - C bindings for EEs ------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the C bindings for the ExecutionEngine library.
//
//===----------------------------------------------------------------------===//

#include "llvm37-c/ExecutionEngine.h"
#include "llvm37/ExecutionEngine/ExecutionEngine.h"
#include "llvm37/ExecutionEngine/GenericValue.h"
#include "llvm37/ExecutionEngine/RTDyldMemoryManager.h"
#include "llvm37/IR/DerivedTypes.h"
#include "llvm37/IR/Module.h"
#include "llvm37/Support/ErrorHandling.h"
#include "llvm37/Target/TargetOptions.h"
#include <cstring>

using namespace llvm37;

#define DEBUG_TYPE "jit"

// Wrapping the C bindings types.
DEFINE_SIMPLE_CONVERSION_FUNCTIONS(GenericValue, LLVM37GenericValueRef)


inline LLVM37TargetMachineRef wrap(const TargetMachine *P) {
  return
  reinterpret_cast<LLVM37TargetMachineRef>(const_cast<TargetMachine*>(P));
}

/*===-- Operations on generic values --------------------------------------===*/

LLVM37GenericValueRef LLVM37CreateGenericValueOfInt(LLVM37TypeRef Ty,
                                                unsigned long long N,
                                                LLVM37Bool IsSigned) {
  GenericValue *GenVal = new GenericValue();
  GenVal->IntVal = APInt(unwrap<IntegerType>(Ty)->getBitWidth(), N, IsSigned);
  return wrap(GenVal);
}

LLVM37GenericValueRef LLVM37CreateGenericValueOfPointer(void *P) {
  GenericValue *GenVal = new GenericValue();
  GenVal->PointerVal = P;
  return wrap(GenVal);
}

LLVM37GenericValueRef LLVM37CreateGenericValueOfFloat(LLVM37TypeRef TyRef, double N) {
  GenericValue *GenVal = new GenericValue();
  switch (unwrap(TyRef)->getTypeID()) {
  case Type::FloatTyID:
    GenVal->FloatVal = N;
    break;
  case Type::DoubleTyID:
    GenVal->DoubleVal = N;
    break;
  default:
    llvm37_unreachable("LLVM37GenericValueToFloat supports only float and double.");
  }
  return wrap(GenVal);
}

unsigned LLVM37GenericValueIntWidth(LLVM37GenericValueRef GenValRef) {
  return unwrap(GenValRef)->IntVal.getBitWidth();
}

unsigned long long LLVM37GenericValueToInt(LLVM37GenericValueRef GenValRef,
                                         LLVM37Bool IsSigned) {
  GenericValue *GenVal = unwrap(GenValRef);
  if (IsSigned)
    return GenVal->IntVal.getSExtValue();
  else
    return GenVal->IntVal.getZExtValue();
}

void *LLVM37GenericValueToPointer(LLVM37GenericValueRef GenVal) {
  return unwrap(GenVal)->PointerVal;
}

double LLVM37GenericValueToFloat(LLVM37TypeRef TyRef, LLVM37GenericValueRef GenVal) {
  switch (unwrap(TyRef)->getTypeID()) {
  case Type::FloatTyID:
    return unwrap(GenVal)->FloatVal;
  case Type::DoubleTyID:
    return unwrap(GenVal)->DoubleVal;
  default:
    llvm37_unreachable("LLVM37GenericValueToFloat supports only float and double.");
  }
}

void LLVM37DisposeGenericValue(LLVM37GenericValueRef GenVal) {
  delete unwrap(GenVal);
}

/*===-- Operations on execution engines -----------------------------------===*/

LLVM37Bool LLVM37CreateExecutionEngineForModule(LLVM37ExecutionEngineRef *OutEE,
                                            LLVM37ModuleRef M,
                                            char **OutError) {
  std::string Error;
  EngineBuilder builder(std::unique_ptr<Module>(unwrap(M)));
  builder.setEngineKind(EngineKind::Either)
         .setErrorStr(&Error);
  if (ExecutionEngine *EE = builder.create()){
    *OutEE = wrap(EE);
    return 0;
  }
  *OutError = strdup(Error.c_str());
  return 1;
}

LLVM37Bool LLVM37CreateInterpreterForModule(LLVM37ExecutionEngineRef *OutInterp,
                                        LLVM37ModuleRef M,
                                        char **OutError) {
  std::string Error;
  EngineBuilder builder(std::unique_ptr<Module>(unwrap(M)));
  builder.setEngineKind(EngineKind::Interpreter)
         .setErrorStr(&Error);
  if (ExecutionEngine *Interp = builder.create()) {
    *OutInterp = wrap(Interp);
    return 0;
  }
  *OutError = strdup(Error.c_str());
  return 1;
}

LLVM37Bool LLVM37CreateJITCompilerForModule(LLVM37ExecutionEngineRef *OutJIT,
                                        LLVM37ModuleRef M,
                                        unsigned OptLevel,
                                        char **OutError) {
  std::string Error;
  EngineBuilder builder(std::unique_ptr<Module>(unwrap(M)));
  builder.setEngineKind(EngineKind::JIT)
         .setErrorStr(&Error)
         .setOptLevel((CodeGenOpt::Level)OptLevel);
  if (ExecutionEngine *JIT = builder.create()) {
    *OutJIT = wrap(JIT);
    return 0;
  }
  *OutError = strdup(Error.c_str());
  return 1;
}

void LLVM37InitializeMCJITCompilerOptions(LLVM37MCJITCompilerOptions *PassedOptions,
                                        size_t SizeOfPassedOptions) {
  LLVM37MCJITCompilerOptions options;
  memset(&options, 0, sizeof(options)); // Most fields are zero by default.
  options.CodeModel = LLVM37CodeModelJITDefault;
  
  memcpy(PassedOptions, &options,
         std::min(sizeof(options), SizeOfPassedOptions));
}

LLVM37Bool LLVM37CreateMCJITCompilerForModule(
    LLVM37ExecutionEngineRef *OutJIT, LLVM37ModuleRef M,
    LLVM37MCJITCompilerOptions *PassedOptions, size_t SizeOfPassedOptions,
    char **OutError) {
  LLVM37MCJITCompilerOptions options;
  // If the user passed a larger sized options struct, then they were compiled
  // against a newer LLVM37. Tell them that something is wrong.
  if (SizeOfPassedOptions > sizeof(options)) {
    *OutError = strdup(
      "Refusing to use options struct that is larger than my own; assuming "
      "LLVM37 library mismatch.");
    return 1;
  }
  
  // Defend against the user having an old version of the API by ensuring that
  // any fields they didn't see are cleared. We must defend against fields being
  // set to the bitwise equivalent of zero, and assume that this means "do the
  // default" as if that option hadn't been available.
  LLVM37InitializeMCJITCompilerOptions(&options, sizeof(options));
  memcpy(&options, PassedOptions, SizeOfPassedOptions);
  
  TargetOptions targetOptions;
  targetOptions.EnableFastISel = options.EnableFastISel;
  std::unique_ptr<Module> Mod(unwrap(M));

  if (Mod)
    // Set function attribute "no-frame-pointer-elim" based on
    // NoFramePointerElim.
    for (auto &F : *Mod) {
      auto Attrs = F.getAttributes();
      auto Value = options.NoFramePointerElim ? "true" : "false";
      Attrs = Attrs.addAttribute(F.getContext(), AttributeSet::FunctionIndex,
                                 "no-frame-pointer-elim", Value);
      F.setAttributes(Attrs);
    }

  std::string Error;
  EngineBuilder builder(std::move(Mod));
  builder.setEngineKind(EngineKind::JIT)
         .setErrorStr(&Error)
         .setOptLevel((CodeGenOpt::Level)options.OptLevel)
         .setCodeModel(unwrap(options.CodeModel))
         .setTargetOptions(targetOptions);
  if (options.MCJMM)
    builder.setMCJITMemoryManager(
      std::unique_ptr<RTDyldMemoryManager>(unwrap(options.MCJMM)));
  if (ExecutionEngine *JIT = builder.create()) {
    *OutJIT = wrap(JIT);
    return 0;
  }
  *OutError = strdup(Error.c_str());
  return 1;
}

LLVM37Bool LLVM37CreateExecutionEngine(LLVM37ExecutionEngineRef *OutEE,
                                   LLVM37ModuleProviderRef MP,
                                   char **OutError) {
  /* The module provider is now actually a module. */
  return LLVM37CreateExecutionEngineForModule(OutEE,
                                            reinterpret_cast<LLVM37ModuleRef>(MP),
                                            OutError);
}

LLVM37Bool LLVM37CreateInterpreter(LLVM37ExecutionEngineRef *OutInterp,
                               LLVM37ModuleProviderRef MP,
                               char **OutError) {
  /* The module provider is now actually a module. */
  return LLVM37CreateInterpreterForModule(OutInterp,
                                        reinterpret_cast<LLVM37ModuleRef>(MP),
                                        OutError);
}

LLVM37Bool LLVM37CreateJITCompiler(LLVM37ExecutionEngineRef *OutJIT,
                               LLVM37ModuleProviderRef MP,
                               unsigned OptLevel,
                               char **OutError) {
  /* The module provider is now actually a module. */
  return LLVM37CreateJITCompilerForModule(OutJIT,
                                        reinterpret_cast<LLVM37ModuleRef>(MP),
                                        OptLevel, OutError);
}


void LLVM37DisposeExecutionEngine(LLVM37ExecutionEngineRef EE) {
  delete unwrap(EE);
}

void LLVM37RunStaticConstructors(LLVM37ExecutionEngineRef EE) {
  unwrap(EE)->runStaticConstructorsDestructors(false);
}

void LLVM37RunStaticDestructors(LLVM37ExecutionEngineRef EE) {
  unwrap(EE)->runStaticConstructorsDestructors(true);
}

int LLVM37RunFunctionAsMain(LLVM37ExecutionEngineRef EE, LLVM37ValueRef F,
                          unsigned ArgC, const char * const *ArgV,
                          const char * const *EnvP) {
  unwrap(EE)->finalizeObject();

  std::vector<std::string> ArgVec(ArgV, ArgV + ArgC);
  return unwrap(EE)->runFunctionAsMain(unwrap<Function>(F), ArgVec, EnvP);
}

LLVM37GenericValueRef LLVM37RunFunction(LLVM37ExecutionEngineRef EE, LLVM37ValueRef F,
                                    unsigned NumArgs,
                                    LLVM37GenericValueRef *Args) {
  unwrap(EE)->finalizeObject();
  
  std::vector<GenericValue> ArgVec;
  ArgVec.reserve(NumArgs);
  for (unsigned I = 0; I != NumArgs; ++I)
    ArgVec.push_back(*unwrap(Args[I]));
  
  GenericValue *Result = new GenericValue();
  *Result = unwrap(EE)->runFunction(unwrap<Function>(F), ArgVec);
  return wrap(Result);
}

void LLVM37FreeMachineCodeForFunction(LLVM37ExecutionEngineRef EE, LLVM37ValueRef F) {
}

void LLVM37AddModule(LLVM37ExecutionEngineRef EE, LLVM37ModuleRef M){
  unwrap(EE)->addModule(std::unique_ptr<Module>(unwrap(M)));
}

void LLVM37AddModuleProvider(LLVM37ExecutionEngineRef EE, LLVM37ModuleProviderRef MP){
  /* The module provider is now actually a module. */
  LLVM37AddModule(EE, reinterpret_cast<LLVM37ModuleRef>(MP));
}

LLVM37Bool LLVM37RemoveModule(LLVM37ExecutionEngineRef EE, LLVM37ModuleRef M,
                          LLVM37ModuleRef *OutMod, char **OutError) {
  Module *Mod = unwrap(M);
  unwrap(EE)->removeModule(Mod);
  *OutMod = wrap(Mod);
  return 0;
}

LLVM37Bool LLVM37RemoveModuleProvider(LLVM37ExecutionEngineRef EE,
                                  LLVM37ModuleProviderRef MP,
                                  LLVM37ModuleRef *OutMod, char **OutError) {
  /* The module provider is now actually a module. */
  return LLVM37RemoveModule(EE, reinterpret_cast<LLVM37ModuleRef>(MP), OutMod,
                          OutError);
}

LLVM37Bool LLVM37FindFunction(LLVM37ExecutionEngineRef EE, const char *Name,
                          LLVM37ValueRef *OutFn) {
  if (Function *F = unwrap(EE)->FindFunctionNamed(Name)) {
    *OutFn = wrap(F);
    return 0;
  }
  return 1;
}

void *LLVM37RecompileAndRelinkFunction(LLVM37ExecutionEngineRef EE,
                                     LLVM37ValueRef Fn) {
  return nullptr;
}

LLVM37TargetDataRef LLVM37GetExecutionEngineTargetData(LLVM37ExecutionEngineRef EE) {
  return wrap(unwrap(EE)->getDataLayout());
}

LLVM37TargetMachineRef
LLVM37GetExecutionEngineTargetMachine(LLVM37ExecutionEngineRef EE) {
  return wrap(unwrap(EE)->getTargetMachine());
}

void LLVM37AddGlobalMapping(LLVM37ExecutionEngineRef EE, LLVM37ValueRef Global,
                          void* Addr) {
  unwrap(EE)->addGlobalMapping(unwrap<GlobalValue>(Global), Addr);
}

void *LLVM37GetPointerToGlobal(LLVM37ExecutionEngineRef EE, LLVM37ValueRef Global) {
  unwrap(EE)->finalizeObject();
  
  return unwrap(EE)->getPointerToGlobal(unwrap<GlobalValue>(Global));
}

uint64_t LLVM37GetGlobalValueAddress(LLVM37ExecutionEngineRef EE, const char *Name) {
  return unwrap(EE)->getGlobalValueAddress(Name);
}

uint64_t LLVM37GetFunctionAddress(LLVM37ExecutionEngineRef EE, const char *Name) {
  return unwrap(EE)->getFunctionAddress(Name);
}

/*===-- Operations on memory managers -------------------------------------===*/

namespace {

struct SimpleBindingMMFunctions {
  LLVM37MemoryManagerAllocateCodeSectionCallback AllocateCodeSection;
  LLVM37MemoryManagerAllocateDataSectionCallback AllocateDataSection;
  LLVM37MemoryManagerFinalizeMemoryCallback FinalizeMemory;
  LLVM37MemoryManagerDestroyCallback Destroy;
};

class SimpleBindingMemoryManager : public RTDyldMemoryManager {
public:
  SimpleBindingMemoryManager(const SimpleBindingMMFunctions& Functions,
                             void *Opaque);
  ~SimpleBindingMemoryManager() override;

  uint8_t *allocateCodeSection(uintptr_t Size, unsigned Alignment,
                               unsigned SectionID,
                               StringRef SectionName) override;

  uint8_t *allocateDataSection(uintptr_t Size, unsigned Alignment,
                               unsigned SectionID, StringRef SectionName,
                               bool isReadOnly) override;

  bool finalizeMemory(std::string *ErrMsg) override;

private:
  SimpleBindingMMFunctions Functions;
  void *Opaque;
};

SimpleBindingMemoryManager::SimpleBindingMemoryManager(
  const SimpleBindingMMFunctions& Functions,
  void *Opaque)
  : Functions(Functions), Opaque(Opaque) {
  assert(Functions.AllocateCodeSection &&
         "No AllocateCodeSection function provided!");
  assert(Functions.AllocateDataSection &&
         "No AllocateDataSection function provided!");
  assert(Functions.FinalizeMemory &&
         "No FinalizeMemory function provided!");
  assert(Functions.Destroy &&
         "No Destroy function provided!");
}

SimpleBindingMemoryManager::~SimpleBindingMemoryManager() {
  Functions.Destroy(Opaque);
}

uint8_t *SimpleBindingMemoryManager::allocateCodeSection(
  uintptr_t Size, unsigned Alignment, unsigned SectionID,
  StringRef SectionName) {
  return Functions.AllocateCodeSection(Opaque, Size, Alignment, SectionID,
                                       SectionName.str().c_str());
}

uint8_t *SimpleBindingMemoryManager::allocateDataSection(
  uintptr_t Size, unsigned Alignment, unsigned SectionID,
  StringRef SectionName, bool isReadOnly) {
  return Functions.AllocateDataSection(Opaque, Size, Alignment, SectionID,
                                       SectionName.str().c_str(),
                                       isReadOnly);
}

bool SimpleBindingMemoryManager::finalizeMemory(std::string *ErrMsg) {
  char *errMsgCString = nullptr;
  bool result = Functions.FinalizeMemory(Opaque, &errMsgCString);
  assert((result || !errMsgCString) &&
         "Did not expect an error message if FinalizeMemory succeeded");
  if (errMsgCString) {
    if (ErrMsg)
      *ErrMsg = errMsgCString;
    free(errMsgCString);
  }
  return result;
}

} // anonymous namespace

LLVM37MCJITMemoryManagerRef LLVM37CreateSimpleMCJITMemoryManager(
  void *Opaque,
  LLVM37MemoryManagerAllocateCodeSectionCallback AllocateCodeSection,
  LLVM37MemoryManagerAllocateDataSectionCallback AllocateDataSection,
  LLVM37MemoryManagerFinalizeMemoryCallback FinalizeMemory,
  LLVM37MemoryManagerDestroyCallback Destroy) {
  
  if (!AllocateCodeSection || !AllocateDataSection || !FinalizeMemory ||
      !Destroy)
    return nullptr;
  
  SimpleBindingMMFunctions functions;
  functions.AllocateCodeSection = AllocateCodeSection;
  functions.AllocateDataSection = AllocateDataSection;
  functions.FinalizeMemory = FinalizeMemory;
  functions.Destroy = Destroy;
  return wrap(new SimpleBindingMemoryManager(functions, Opaque));
}

void LLVM37DisposeMCJITMemoryManager(LLVM37MCJITMemoryManagerRef MM) {
  delete unwrap(MM);
}

