/*===-- llvm37-c/ExecutionEngine.h - ExecutionEngine Lib C Iface --*- C++ -*-===*\
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This header declares the C interface to libLLVM37ExecutionEngine.o, which    *|
|* implements various analyses of the LLVM37 IR.                                *|
|*                                                                            *|
|* Many exotic languages can interoperate with C code but have a harder time  *|
|* with C++ due to name mangling. So in addition to C, this interface enables *|
|* tools written in such languages.                                           *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifndef LLVM37_C_EXECUTIONENGINE_H
#define LLVM37_C_EXECUTIONENGINE_H

#include "llvm37-c/Core.h"
#include "llvm37-c/Target.h"
#include "llvm37-c/TargetMachine.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LLVM37CExecutionEngine Execution Engine
 * @ingroup LLVM37C
 *
 * @{
 */

void LLVM37LinkInMCJIT(void);
void LLVM37LinkInInterpreter(void);

typedef struct LLVM37OpaqueGenericValue *LLVM37GenericValueRef;
typedef struct LLVM37OpaqueExecutionEngine *LLVM37ExecutionEngineRef;
typedef struct LLVM37OpaqueMCJITMemoryManager *LLVM37MCJITMemoryManagerRef;

struct LLVM37MCJITCompilerOptions {
  unsigned OptLevel;
  LLVM37CodeModel CodeModel;
  LLVM37Bool NoFramePointerElim;
  LLVM37Bool EnableFastISel;
  LLVM37MCJITMemoryManagerRef MCJMM;
};

/*===-- Operations on generic values --------------------------------------===*/

LLVM37GenericValueRef LLVM37CreateGenericValueOfInt(LLVM37TypeRef Ty,
                                                unsigned long long N,
                                                LLVM37Bool IsSigned);

LLVM37GenericValueRef LLVM37CreateGenericValueOfPointer(void *P);

LLVM37GenericValueRef LLVM37CreateGenericValueOfFloat(LLVM37TypeRef Ty, double N);

unsigned LLVM37GenericValueIntWidth(LLVM37GenericValueRef GenValRef);

unsigned long long LLVM37GenericValueToInt(LLVM37GenericValueRef GenVal,
                                         LLVM37Bool IsSigned);

void *LLVM37GenericValueToPointer(LLVM37GenericValueRef GenVal);

double LLVM37GenericValueToFloat(LLVM37TypeRef TyRef, LLVM37GenericValueRef GenVal);

void LLVM37DisposeGenericValue(LLVM37GenericValueRef GenVal);

/*===-- Operations on execution engines -----------------------------------===*/

LLVM37Bool LLVM37CreateExecutionEngineForModule(LLVM37ExecutionEngineRef *OutEE,
                                            LLVM37ModuleRef M,
											_Out_opt_  char **OutError);

LLVM37Bool LLVM37CreateInterpreterForModule(LLVM37ExecutionEngineRef *OutInterp,
                                        LLVM37ModuleRef M,
										_Out_opt_ char **OutError);

LLVM37Bool LLVM37CreateJITCompilerForModule(LLVM37ExecutionEngineRef *OutJIT,
                                        LLVM37ModuleRef M,
                                        unsigned OptLevel,
										_Out_opt_ char **OutError);

void LLVM37InitializeMCJITCompilerOptions(
  struct LLVM37MCJITCompilerOptions *Options, size_t SizeOfOptions);

/**
 * Create an MCJIT execution engine for a module, with the given options. It is
 * the responsibility of the caller to ensure that all fields in Options up to
 * the given SizeOfOptions are initialized. It is correct to pass a smaller
 * value of SizeOfOptions that omits some fields. The canonical way of using
 * this is:
 *
 * LLVM37MCJITCompilerOptions options;
 * LLVM37InitializeMCJITCompilerOptions(&options, sizeof(options));
 * ... fill in those options you care about
 * LLVM37CreateMCJITCompilerForModule(&jit, mod, &options, sizeof(options),
 *                                  &error);
 *
 * Note that this is also correct, though possibly suboptimal:
 *
 * LLVM37CreateMCJITCompilerForModule(&jit, mod, 0, 0, &error);
 */
LLVM37Bool LLVM37CreateMCJITCompilerForModule(
  LLVM37ExecutionEngineRef *OutJIT, LLVM37ModuleRef M,
  struct LLVM37MCJITCompilerOptions *Options, size_t SizeOfOptions,
	  _Out_opt_ char **OutError);

/** Deprecated: Use LLVM37CreateExecutionEngineForModule instead. */
LLVM37Bool LLVM37CreateExecutionEngine(LLVM37ExecutionEngineRef *OutEE,
                                   LLVM37ModuleProviderRef MP,
								   _Out_opt_ char **OutError);

/** Deprecated: Use LLVM37CreateInterpreterForModule instead. */
LLVM37Bool LLVM37CreateInterpreter(LLVM37ExecutionEngineRef *OutInterp,
                               LLVM37ModuleProviderRef MP,
							   _Out_opt_ char **OutError);

/** Deprecated: Use LLVM37CreateJITCompilerForModule instead. */
LLVM37Bool LLVM37CreateJITCompiler(LLVM37ExecutionEngineRef *OutJIT,
                               LLVM37ModuleProviderRef MP,
                               unsigned OptLevel,
							   _Out_opt_ char **OutError);

void LLVM37DisposeExecutionEngine(LLVM37ExecutionEngineRef EE);

void LLVM37RunStaticConstructors(LLVM37ExecutionEngineRef EE);

void LLVM37RunStaticDestructors(LLVM37ExecutionEngineRef EE);

int LLVM37RunFunctionAsMain(LLVM37ExecutionEngineRef EE, LLVM37ValueRef F,
                          unsigned ArgC, const char * const *ArgV,
                          const char * const *EnvP);

LLVM37GenericValueRef LLVM37RunFunction(LLVM37ExecutionEngineRef EE, LLVM37ValueRef F,
                                    unsigned NumArgs,
                                    LLVM37GenericValueRef *Args);

void LLVM37FreeMachineCodeForFunction(LLVM37ExecutionEngineRef EE, LLVM37ValueRef F);

void LLVM37AddModule(LLVM37ExecutionEngineRef EE, LLVM37ModuleRef M);

/** Deprecated: Use LLVM37AddModule instead. */
void LLVM37AddModuleProvider(LLVM37ExecutionEngineRef EE, LLVM37ModuleProviderRef MP);

LLVM37Bool LLVM37RemoveModule(LLVM37ExecutionEngineRef EE, LLVM37ModuleRef M,
	LLVM37ModuleRef *OutMod, _Out_opt_  char **OutError);

/** Deprecated: Use LLVM37RemoveModule instead. */
LLVM37Bool LLVM37RemoveModuleProvider(LLVM37ExecutionEngineRef EE,
                                  LLVM37ModuleProviderRef MP,
								  LLVM37ModuleRef *OutMod, _Out_opt_ char **OutError);

LLVM37Bool LLVM37FindFunction(LLVM37ExecutionEngineRef EE, const char *Name,
                          LLVM37ValueRef *OutFn);

void *LLVM37RecompileAndRelinkFunction(LLVM37ExecutionEngineRef EE,
                                     LLVM37ValueRef Fn);

LLVM37TargetDataRef LLVM37GetExecutionEngineTargetData(LLVM37ExecutionEngineRef EE);
LLVM37TargetMachineRef
LLVM37GetExecutionEngineTargetMachine(LLVM37ExecutionEngineRef EE);

void LLVM37AddGlobalMapping(LLVM37ExecutionEngineRef EE, LLVM37ValueRef Global,
                          void* Addr);

void *LLVM37GetPointerToGlobal(LLVM37ExecutionEngineRef EE, LLVM37ValueRef Global);

uint64_t LLVM37GetGlobalValueAddress(LLVM37ExecutionEngineRef EE, const char *Name);

uint64_t LLVM37GetFunctionAddress(LLVM37ExecutionEngineRef EE, const char *Name);

/*===-- Operations on memory managers -------------------------------------===*/

typedef uint8_t *(*LLVM37MemoryManagerAllocateCodeSectionCallback)(
  void *Opaque, uintptr_t Size, unsigned Alignment, unsigned SectionID,
  const char *SectionName);
typedef uint8_t *(*LLVM37MemoryManagerAllocateDataSectionCallback)(
  void *Opaque, uintptr_t Size, unsigned Alignment, unsigned SectionID,
  const char *SectionName, LLVM37Bool IsReadOnly);
typedef LLVM37Bool (*LLVM37MemoryManagerFinalizeMemoryCallback)(
  void *Opaque, char **ErrMsg);
typedef void (*LLVM37MemoryManagerDestroyCallback)(void *Opaque);

/**
 * Create a simple custom MCJIT memory manager. This memory manager can
 * intercept allocations in a module-oblivious way. This will return NULL
 * if any of the passed functions are NULL.
 *
 * @param Opaque An opaque client object to pass back to the callbacks.
 * @param AllocateCodeSection Allocate a block of memory for executable code.
 * @param AllocateDataSection Allocate a block of memory for data.
 * @param FinalizeMemory Set page permissions and flush cache. Return 0 on
 *   success, 1 on error.
 */
LLVM37MCJITMemoryManagerRef LLVM37CreateSimpleMCJITMemoryManager(
  void *Opaque,
  LLVM37MemoryManagerAllocateCodeSectionCallback AllocateCodeSection,
  LLVM37MemoryManagerAllocateDataSectionCallback AllocateDataSection,
  LLVM37MemoryManagerFinalizeMemoryCallback FinalizeMemory,
  LLVM37MemoryManagerDestroyCallback Destroy);

void LLVM37DisposeMCJITMemoryManager(LLVM37MCJITMemoryManagerRef MM);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* defined(__cplusplus) */

#endif
