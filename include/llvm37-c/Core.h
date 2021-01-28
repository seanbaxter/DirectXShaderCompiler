/*===-- llvm37-c/Core.h - Core Library C Interface ------------------*- C -*-===*\
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This header declares the C interface to libLLVM37Core.a, which implements    *|
|* the LLVM37 intermediate representation.                                      *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifndef LLVM37_C_CORE_H
#define LLVM37_C_CORE_H

#include "dxc/Support/WinAdapter.h" // HLSL Change
#include "llvm37-c/Support.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LLVM37C LLVM37-C: C interface to LLVM37
 *
 * This module exposes parts of the LLVM37 library as a C API.
 *
 * @{
 */

/**
 * @defgroup LLVM37CTransforms Transforms
 */

/**
 * @defgroup LLVM37CCore Core
 *
 * This modules provide an interface to libLLVM37Core, which implements
 * the LLVM37 intermediate representation as well as other related types
 * and utilities.
 *
 * LLVM37 uses a polymorphic type hierarchy which C cannot represent, therefore
 * parameters must be passed as base types. Despite the declared types, most
 * of the functions provided operate only on branches of the type hierarchy.
 * The declared parameter names are descriptive and specify which type is
 * required. Additionally, each type hierarchy is documented along with the
 * functions that operate upon it. For more detail, refer to LLVM37's C++ code.
 * If in doubt, refer to Core.cpp, which performs parameter downcasts in the
 * form unwrap<RequiredType>(Param).
 *
 * Many exotic languages can interoperate with C code but have a harder time
 * with C++ due to name mangling. So in addition to C, this interface enables
 * tools written in such languages.
 *
 * @{
 */

/**
 * @defgroup LLVM37CCoreTypes Types and Enumerations
 *
 * @{
 */

/* Opaque types. */

/**
 * The top-level container for all LLVM37 global data. See the LLVM37Context class.
 */
typedef struct LLVM37OpaqueContext *LLVM37ContextRef;

/**
 * The top-level container for all other LLVM37 Intermediate Representation (IR)
 * objects.
 *
 * @see llvm37::Module
 */
typedef struct LLVM37OpaqueModule *LLVM37ModuleRef;

/**
 * Each value in the LLVM37 IR has a type, an LLVM37TypeRef.
 *
 * @see llvm37::Type
 */
typedef struct LLVM37OpaqueType *LLVM37TypeRef;

/**
 * Represents an individual value in LLVM37 IR.
 *
 * This models llvm37::Value.
 */
typedef struct LLVM37OpaqueValue *LLVM37ValueRef;

/**
 * Represents a basic block of instructions in LLVM37 IR.
 *
 * This models llvm37::BasicBlock.
 */
typedef struct LLVM37OpaqueBasicBlock *LLVM37BasicBlockRef;

/**
 * Represents an LLVM37 basic block builder.
 *
 * This models llvm37::IRBuilder.
 */
typedef struct LLVM37OpaqueBuilder *LLVMBuilderRef;

/**
 * Interface used to provide a module to JIT or interpreter.
 * This is now just a synonym for llvm37::Module, but we have to keep using the
 * different type to keep binary compatibility.
 */
typedef struct LLVM37OpaqueModuleProvider *LLVM37ModuleProviderRef;

/** @see llvm37::PassManagerBase */
typedef struct LLVM37OpaquePassManager *LLVM37PassManagerRef;

/** @see llvm37::PassRegistry */
typedef struct LLVM37OpaquePassRegistry *LLVM37PassRegistryRef;

/**
 * Used to get the users and usees of a Value.
 *
 * @see llvm37::Use */
typedef struct LLVM37OpaqueUse *LLVM37UseRef;


/**
 * @see llvm37::DiagnosticInfo
 */
typedef struct LLVM37OpaqueDiagnosticInfo *LLVM37DiagnosticInfoRef;

typedef enum {
    LLVM37ZExtAttribute       = 1<<0,
    LLVM37SExtAttribute       = 1<<1,
    LLVM37NoReturnAttribute   = 1<<2,
    LLVM37InRegAttribute      = 1<<3,
    LLVM37StructRetAttribute  = 1<<4,
    LLVM37NoUnwindAttribute   = 1<<5,
    LLVM37NoAliasAttribute    = 1<<6,
    LLVM37ByValAttribute      = 1<<7,
    LLVM37NestAttribute       = 1<<8,
    LLVM37ReadNoneAttribute   = 1<<9,
    LLVM37ReadOnlyAttribute   = 1<<10,
    LLVM37NoInlineAttribute   = 1<<11,
    LLVM37AlwaysInlineAttribute    = 1<<12,
    LLVM37OptimizeForSizeAttribute = 1<<13,
    LLVM37StackProtectAttribute    = 1<<14,
    LLVM37StackProtectReqAttribute = 1<<15,
    LLVM37Alignment = 31<<16,
    LLVM37NoCaptureAttribute  = 1<<21,
    LLVM37NoRedZoneAttribute  = 1<<22,
    LLVM37NoImplicitFloatAttribute = 1<<23,
    LLVM37NakedAttribute      = 1<<24,
    LLVM37InlineHintAttribute = 1<<25,
    LLVM37StackAlignment = 7<<26,
    LLVM37ReturnsTwice = 1 << 29,
    LLVM37UWTable = 1 << 30,
    LLVM37NonLazyBind = 1 << 31

    /* FIXME: These attributes are currently not included in the C API as
       a temporary measure until the API/ABI impact to the C API is understood
       and the path forward agreed upon.
    LLVM37SanitizeAddressAttribute = 1ULL << 32,
    LLVM37StackProtectStrongAttribute = 1ULL<<35,
    LLVM37ColdAttribute = 1ULL << 40,
    LLVM37OptimizeNoneAttribute = 1ULL << 42,
    LLVM37InAllocaAttribute = 1ULL << 43,
    LLVM37NonNullAttribute = 1ULL << 44,
    LLVM37JumpTableAttribute = 1ULL << 45,
    LLVM37ConvergentAttribute = 1ULL << 46,
    LLVM37SafeStackAttribute = 1ULL << 47,
    */
} LLVM37Attribute;

typedef enum {
  /* Terminator Instructions */
  LLVM37Ret            = 1,
  LLVM37Br             = 2,
  LLVM37Switch         = 3,
  LLVM37IndirectBr     = 4,
  LLVM37Invoke         = 5,
  /* removed 6 due to API changes */
  LLVM37Unreachable    = 7,

  /* Standard Binary Operators */
  LLVM37Add            = 8,
  LLVM37FAdd           = 9,
  LLVM37Sub            = 10,
  LLVM37FSub           = 11,
  LLVM37Mul            = 12,
  LLVM37FMul           = 13,
  LLVM37UDiv           = 14,
  LLVM37SDiv           = 15,
  LLVM37FDiv           = 16,
  LLVM37URem           = 17,
  LLVM37SRem           = 18,
  LLVM37FRem           = 19,

  /* Logical Operators */
  LLVM37Shl            = 20,
  LLVM37LShr           = 21,
  LLVM37AShr           = 22,
  LLVM37And            = 23,
  LLVM37Or             = 24,
  LLVM37Xor            = 25,

  /* Memory Operators */
  LLVM37Alloca         = 26,
  LLVM37Load           = 27,
  LLVM37Store          = 28,
  LLVM37GetElementPtr  = 29,

  /* Cast Operators */
  LLVM37Trunc          = 30,
  LLVM37ZExt           = 31,
  LLVM37SExt           = 32,
  LLVM37FPToUI         = 33,
  LLVM37FPToSI         = 34,
  LLVM37UIToFP         = 35,
  LLVM37SIToFP         = 36,
  LLVM37FPTrunc        = 37,
  LLVM37FPExt          = 38,
  LLVM37PtrToInt       = 39,
  LLVM37IntToPtr       = 40,
  LLVM37BitCast        = 41,
  LLVM37AddrSpaceCast  = 60,

  /* Other Operators */
  LLVM37ICmp           = 42,
  LLVM37FCmp           = 43,
  LLVM37PHI            = 44,
  LLVM37Call           = 45,
  LLVM37Select         = 46,
  LLVM37UserOp1        = 47,
  LLVM37UserOp2        = 48,
  LLVM37VAArg          = 49,
  LLVM37ExtractElement = 50,
  LLVM37InsertElement  = 51,
  LLVM37ShuffleVector  = 52,
  LLVM37ExtractValue   = 53,
  LLVM37InsertValue    = 54,

  /* Atomic operators */
  LLVM37Fence          = 55,
  LLVM37AtomicCmpXchg  = 56,
  LLVM37AtomicRMW      = 57,

  /* Exception Handling Operators */
  LLVM37Resume         = 58,
  LLVM37LandingPad     = 59

} LLVM37Opcode;

typedef enum {
  LLVM37VoidTypeKind,        /**< type with no size */
  LLVM37HalfTypeKind,        /**< 16 bit floating point type */
  LLVM37FloatTypeKind,       /**< 32 bit floating point type */
  LLVM37DoubleTypeKind,      /**< 64 bit floating point type */
  LLVM37X86_FP80TypeKind,    /**< 80 bit floating point type (X87) */
  LLVM37FP128TypeKind,       /**< 128 bit floating point type (112-bit mantissa)*/
  LLVM37PPC_FP128TypeKind,   /**< 128 bit floating point type (two 64-bits) */
  LLVM37LabelTypeKind,       /**< Labels */
  LLVM37IntegerTypeKind,     /**< Arbitrary bit width integers */
  LLVM37FunctionTypeKind,    /**< Functions */
  LLVM37StructTypeKind,      /**< Structures */
  LLVM37ArrayTypeKind,       /**< Arrays */
  LLVM37PointerTypeKind,     /**< Pointers */
  LLVM37VectorTypeKind,      /**< SIMD 'packed' format, or other vector type */
  LLVM37MetadataTypeKind,    /**< Metadata */
  LLVM37X86_MMXTypeKind      /**< X86 MMX */
} LLVM37TypeKind;

typedef enum {
  LLVM37ExternalLinkage,    /**< Externally visible function */
  LLVM37AvailableExternallyLinkage,
  LLVM37LinkOnceAnyLinkage, /**< Keep one copy of function when linking (inline)*/
  LLVM37LinkOnceODRLinkage, /**< Same, but only replaced by something
                            equivalent. */
  LLVM37LinkOnceODRAutoHideLinkage, /**< Obsolete */
  LLVM37WeakAnyLinkage,     /**< Keep one copy of function when linking (weak) */
  LLVM37WeakODRLinkage,     /**< Same, but only replaced by something
                            equivalent. */
  LLVM37AppendingLinkage,   /**< Special purpose, only applies to global arrays */
  LLVM37InternalLinkage,    /**< Rename collisions when linking (static
                               functions) */
  LLVM37PrivateLinkage,     /**< Like Internal, but omit from symbol table */
  LLVM37DLLImportLinkage,   /**< Obsolete */
  LLVM37DLLExportLinkage,   /**< Obsolete */
  LLVM37ExternalWeakLinkage,/**< ExternalWeak linkage description */
  LLVM37GhostLinkage,       /**< Obsolete */
  LLVM37CommonLinkage,      /**< Tentative definitions */
  LLVM37LinkerPrivateLinkage, /**< Like Private, but linker removes. */
  LLVM37LinkerPrivateWeakLinkage /**< Like LinkerPrivate, but is weak. */
} LLVM37Linkage;

typedef enum {
  LLVM37DefaultVisibility,  /**< The GV is visible */
  LLVM37HiddenVisibility,   /**< The GV is hidden */
  LLVM37ProtectedVisibility /**< The GV is protected */
} LLVM37Visibility;

typedef enum {
  LLVM37DefaultStorageClass   = 0,
  LLVM37DLLImportStorageClass = 1, /**< Function to be imported from DLL. */
  LLVM37DLLExportStorageClass = 2  /**< Function to be accessible from DLL. */
} LLVM37DLLStorageClass;

typedef enum {
  LLVM37CCallConv           = 0,
  LLVM37FastCallConv        = 8,
  LLVM37ColdCallConv        = 9,
  LLVM37WebKitJSCallConv    = 12,
  LLVM37AnyRegCallConv      = 13,
  LLVM37X86StdcallCallConv  = 64,
  LLVM37X86FastcallCallConv = 65
} LLVM37CallConv;

typedef enum {
  LLVM37IntEQ = 32, /**< equal */
  LLVM37IntNE,      /**< not equal */
  LLVM37IntUGT,     /**< unsigned greater than */
  LLVM37IntUGE,     /**< unsigned greater or equal */
  LLVM37IntULT,     /**< unsigned less than */
  LLVM37IntULE,     /**< unsigned less or equal */
  LLVM37IntSGT,     /**< signed greater than */
  LLVM37IntSGE,     /**< signed greater or equal */
  LLVM37IntSLT,     /**< signed less than */
  LLVM37IntSLE      /**< signed less or equal */
} LLVM37IntPredicate;

typedef enum {
  LLVM37RealPredicateFalse, /**< Always false (always folded) */
  LLVM37RealOEQ,            /**< True if ordered and equal */
  LLVM37RealOGT,            /**< True if ordered and greater than */
  LLVM37RealOGE,            /**< True if ordered and greater than or equal */
  LLVM37RealOLT,            /**< True if ordered and less than */
  LLVM37RealOLE,            /**< True if ordered and less than or equal */
  LLVM37RealONE,            /**< True if ordered and operands are unequal */
  LLVM37RealORD,            /**< True if ordered (no nans) */
  LLVM37RealUNO,            /**< True if unordered: isnan(X) | isnan(Y) */
  LLVM37RealUEQ,            /**< True if unordered or equal */
  LLVM37RealUGT,            /**< True if unordered or greater than */
  LLVM37RealUGE,            /**< True if unordered, greater than, or equal */
  LLVM37RealULT,            /**< True if unordered or less than */
  LLVM37RealULE,            /**< True if unordered, less than, or equal */
  LLVM37RealUNE,            /**< True if unordered or not equal */
  LLVM37RealPredicateTrue   /**< Always true (always folded) */
} LLVM37RealPredicate;

typedef enum {
  LLVM37LandingPadCatch,    /**< A catch clause   */
  LLVM37LandingPadFilter    /**< A filter clause  */
} LLVM37LandingPadClauseTy;

typedef enum {
  LLVM37NotThreadLocal = 0,
  LLVM37GeneralDynamicTLSModel,
  LLVM37LocalDynamicTLSModel,
  LLVM37InitialExecTLSModel,
  LLVM37LocalExecTLSModel
} LLVM37ThreadLocalMode;

typedef enum {
  LLVM37AtomicOrderingNotAtomic = 0, /**< A load or store which is not atomic */
  LLVM37AtomicOrderingUnordered = 1, /**< Lowest level of atomicity, guarantees
                                     somewhat sane results, lock free. */
  LLVM37AtomicOrderingMonotonic = 2, /**< guarantees that if you take all the
                                     operations affecting a specific address,
                                     a consistent ordering exists */
  LLVM37AtomicOrderingAcquire = 4, /**< Acquire provides a barrier of the sort
                                   necessary to acquire a lock to access other
                                   memory with normal loads and stores. */
  LLVM37AtomicOrderingRelease = 5, /**< Release is similar to Acquire, but with
                                   a barrier of the sort necessary to release
                                   a lock. */
  LLVM37AtomicOrderingAcquireRelease = 6, /**< provides both an Acquire and a
                                          Release barrier (for fences and
                                          operations which both read and write
                                           memory). */
  LLVM37AtomicOrderingSequentiallyConsistent = 7 /**< provides Acquire semantics
                                                 for loads and Release
                                                 semantics for stores.
                                                 Additionally, it guarantees
                                                 that a total ordering exists
                                                 between all
                                                 SequentiallyConsistent
                                                 operations. */
} LLVM37AtomicOrdering;

typedef enum {
    LLVM37AtomicRMWBinOpXchg, /**< Set the new value and return the one old */
    LLVM37AtomicRMWBinOpAdd, /**< Add a value and return the old one */
    LLVM37AtomicRMWBinOpSub, /**< Subtract a value and return the old one */
    LLVM37AtomicRMWBinOpAnd, /**< And a value and return the old one */
    LLVM37AtomicRMWBinOpNand, /**< Not-And a value and return the old one */
    LLVM37AtomicRMWBinOpOr, /**< OR a value and return the old one */
    LLVM37AtomicRMWBinOpXor, /**< Xor a value and return the old one */
    LLVM37AtomicRMWBinOpMax, /**< Sets the value if it's greater than the
                             original using a signed comparison and return
                             the old one */
    LLVM37AtomicRMWBinOpMin, /**< Sets the value if it's Smaller than the
                             original using a signed comparison and return
                             the old one */
    LLVM37AtomicRMWBinOpUMax, /**< Sets the value if it's greater than the
                             original using an unsigned comparison and return
                             the old one */
    LLVM37AtomicRMWBinOpUMin /**< Sets the value if it's greater than the
                             original using an unsigned comparison  and return
                             the old one */
} LLVM37AtomicRMWBinOp;

typedef enum {
    LLVM37DSError,
    LLVM37DSWarning,
    LLVM37DSRemark,
    LLVM37DSNote
} LLVM37DiagnosticSeverity;

/**
 * @}
 */

void LLVM37InitializeCore(LLVM37PassRegistryRef R);

/** Deallocate and destroy all ManagedStatic variables.
    @see llvm37::llvm37_shutdown
    @see ManagedStatic */
void LLVM37Shutdown(void);


/*===-- Error handling ----------------------------------------------------===*/

char *LLVM37CreateMessage(const char *Message);
void LLVM37DisposeMessage(_Out_opt_ char *Message);

typedef void (*LLVM37FatalErrorHandler)(const char *Reason);

/**
 * Install a fatal error handler. By default, if LLVM37 detects a fatal error, it
 * will call exit(1). This may not be appropriate in many contexts. For example,
 * doing exit(1) will bypass many crash reporting/tracing system tools. This
 * function allows you to install a callback that will be invoked prior to the
 * call to exit(1).
 */
void LLVM37InstallFatalErrorHandler(LLVM37FatalErrorHandler Handler);

/**
 * Reset the fatal error handler. This resets LLVM37's fatal error handling
 * behavior to the default.
 */
void LLVM37ResetFatalErrorHandler(void);

/**
 * Enable LLVM37's built-in stack trace code. This intercepts the OS's crash
 * signals and prints which component of LLVM37 you were in at the time if the
 * crash.
 */
void LLVM37EnablePrettyStackTrace(void);

/**
 * @defgroup LLVM37CCoreContext Contexts
 *
 * Contexts are execution states for the core LLVM37 IR system.
 *
 * Most types are tied to a context instance. Multiple contexts can
 * exist simultaneously. A single context is not thread safe. However,
 * different contexts can execute on different threads simultaneously.
 *
 * @{
 */

typedef void (*LLVM37DiagnosticHandler)(LLVM37DiagnosticInfoRef, void *);
typedef void (*LLVM37YieldCallback)(LLVM37ContextRef, void *);

/**
 * Create a new context.
 *
 * Every call to this function should be paired with a call to
 * LLVM37ContextDispose() or the context will leak memory.
 */
LLVM37ContextRef LLVM37ContextCreate(void);

/**
 * Obtain the global context instance.
 */
LLVM37ContextRef LLVM37GetGlobalContext(void);

/**
 * Set the diagnostic handler for this context.
 */
void LLVM37ContextSetDiagnosticHandler(LLVM37ContextRef C,
                                     LLVM37DiagnosticHandler Handler,
                                     void *DiagnosticContext);

/**
 * Set the yield callback function for this context.
 *
 * @see LLVM37Context::setYieldCallback()
 */
void LLVM37ContextSetYieldCallback(LLVM37ContextRef C, LLVM37YieldCallback Callback,
                                 void *OpaqueHandle);

/**
 * Destroy a context instance.
 *
 * This should be called for every call to LLVM37ContextCreate() or memory
 * will be leaked.
 */
void LLVM37ContextDispose(LLVM37ContextRef C);

/**
 * Return a string representation of the DiagnosticInfo. Use
 * LLVM37DisposeMessage to free the string.
 *
 * @see DiagnosticInfo::print()
 */
char *LLVM37GetDiagInfoDescription(LLVM37DiagnosticInfoRef DI);

/**
 * Return an enum LLVM37DiagnosticSeverity.
 *
 * @see DiagnosticInfo::getSeverity()
 */
LLVM37DiagnosticSeverity LLVM37GetDiagInfoSeverity(LLVM37DiagnosticInfoRef DI);

unsigned LLVM37GetMDKindIDInContext(LLVM37ContextRef C, const char* Name,
                                  unsigned SLen);
unsigned LLVM37GetMDKindID(const char* Name, unsigned SLen);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreModule Modules
 *
 * Modules represent the top-level structure in an LLVM37 program. An LLVM37
 * module is effectively a translation unit or a collection of
 * translation units merged together.
 *
 * @{
 */

/**
 * Create a new, empty module in the global context.
 *
 * This is equivalent to calling LLVM37ModuleCreateWithNameInContext with
 * LLVM37GetGlobalContext() as the context parameter.
 *
 * Every invocation should be paired with LLVM37DisposeModule() or memory
 * will be leaked.
 */
LLVM37ModuleRef LLVM37ModuleCreateWithName(const char *ModuleID);

/**
 * Create a new, empty module in a specific context.
 *
 * Every invocation should be paired with LLVM37DisposeModule() or memory
 * will be leaked.
 */
LLVM37ModuleRef LLVM37ModuleCreateWithNameInContext(const char *ModuleID,
                                                LLVM37ContextRef C);
/**
 * Return an exact copy of the specified module.
 */
LLVM37ModuleRef LLVM37CloneModule(LLVM37ModuleRef M);

/**
 * Destroy a module instance.
 *
 * This must be called for every created module or memory will be
 * leaked.
 */
void LLVM37DisposeModule(LLVM37ModuleRef M);

/**
 * Obtain the data layout for a module.
 *
 * @see Module::getDataLayout()
 */
const char *LLVM37GetDataLayout(LLVM37ModuleRef M);

/**
 * Set the data layout for a module.
 *
 * @see Module::setDataLayout()
 */
void LLVM37SetDataLayout(LLVM37ModuleRef M, const char *Triple);

/**
 * Obtain the target triple for a module.
 *
 * @see Module::getTargetTriple()
 */
const char *LLVM37GetTarget(LLVM37ModuleRef M);

/**
 * Set the target triple for a module.
 *
 * @see Module::setTargetTriple()
 */
void LLVM37SetTarget(LLVM37ModuleRef M, const char *Triple);

/**
 * Dump a representation of a module to stderr.
 *
 * @see Module::dump()
 */
void LLVM37DumpModule(LLVM37ModuleRef M);

/**
 * Print a representation of a module to a file. The ErrorMessage needs to be
 * disposed with LLVM37DisposeMessage. Returns 0 on success, 1 otherwise.
 *
 * @see Module::print()
 */
LLVM37Bool LLVM37PrintModuleToFile(LLVM37ModuleRef M, const char *Filename,
    _Out_opt_ char **ErrorMessage);

/**
 * Return a string representation of the module. Use
 * LLVM37DisposeMessage to free the string.
 *
 * @see Module::print()
 */
char *LLVM37PrintModuleToString(LLVM37ModuleRef M);

/**
 * Set inline assembly for a module.
 *
 * @see Module::setModuleInlineAsm()
 */
void LLVM37SetModuleInlineAsm(LLVM37ModuleRef M, const char *Asm);

/**
 * Obtain the context to which this module is associated.
 *
 * @see Module::getContext()
 */
LLVM37ContextRef LLVM37GetModuleContext(LLVM37ModuleRef M);

/**
 * Obtain a Type from a module by its registered name.
 */
LLVM37TypeRef LLVM37GetTypeByName(LLVM37ModuleRef M, const char *Name);

/**
 * Obtain the number of operands for named metadata in a module.
 *
 * @see llvm37::Module::getNamedMetadata()
 */
unsigned LLVM37GetNamedMetadataNumOperands(LLVM37ModuleRef M, const char* name);

/**
 * Obtain the named metadata operands for a module.
 *
 * The passed LLVM37ValueRef pointer should refer to an array of
 * LLVM37ValueRef at least LLVM37GetNamedMetadataNumOperands long. This
 * array will be populated with the LLVM37ValueRef instances. Each
 * instance corresponds to a llvm37::MDNode.
 *
 * @see llvm37::Module::getNamedMetadata()
 * @see llvm37::MDNode::getOperand()
 */
void LLVM37GetNamedMetadataOperands(LLVM37ModuleRef M, const char* name, LLVM37ValueRef *Dest);

/**
 * Add an operand to named metadata.
 *
 * @see llvm37::Module::getNamedMetadata()
 * @see llvm37::MDNode::addOperand()
 */
void LLVM37AddNamedMetadataOperand(LLVM37ModuleRef M, const char* name,
                                 LLVM37ValueRef Val);

/**
 * Add a function to a module under a specified name.
 *
 * @see llvm37::Function::Create()
 */
LLVM37ValueRef LLVM37AddFunction(LLVM37ModuleRef M, const char *Name,
                             LLVM37TypeRef FunctionTy);

/**
 * Obtain a Function value from a Module by its name.
 *
 * The returned value corresponds to a llvm37::Function value.
 *
 * @see llvm37::Module::getFunction()
 */
LLVM37ValueRef LLVM37GetNamedFunction(LLVM37ModuleRef M, const char *Name);

/**
 * Obtain an iterator to the first Function in a Module.
 *
 * @see llvm37::Module::begin()
 */
LLVM37ValueRef LLVM37GetFirstFunction(LLVM37ModuleRef M);

/**
 * Obtain an iterator to the last Function in a Module.
 *
 * @see llvm37::Module::end()
 */
LLVM37ValueRef LLVM37GetLastFunction(LLVM37ModuleRef M);

/**
 * Advance a Function iterator to the next Function.
 *
 * Returns NULL if the iterator was already at the end and there are no more
 * functions.
 */
LLVM37ValueRef LLVM37GetNextFunction(LLVM37ValueRef Fn);

/**
 * Decrement a Function iterator to the previous Function.
 *
 * Returns NULL if the iterator was already at the beginning and there are
 * no previous functions.
 */
LLVM37ValueRef LLVM37GetPreviousFunction(LLVM37ValueRef Fn);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreType Types
 *
 * Types represent the type of a value.
 *
 * Types are associated with a context instance. The context internally
 * deduplicates types so there is only 1 instance of a specific type
 * alive at a time. In other words, a unique type is shared among all
 * consumers within a context.
 *
 * A Type in the C API corresponds to llvm37::Type.
 *
 * Types have the following hierarchy:
 *
 *   types:
 *     integer type
 *     real type
 *     function type
 *     sequence types:
 *       array type
 *       pointer type
 *       vector type
 *     void type
 *     label type
 *     opaque type
 *
 * @{
 */

/**
 * Obtain the enumerated type of a Type instance.
 *
 * @see llvm37::Type:getTypeID()
 */
LLVM37TypeKind LLVM37GetTypeKind(LLVM37TypeRef Ty);

/**
 * Whether the type has a known size.
 *
 * Things that don't have a size are abstract types, labels, and void.a
 *
 * @see llvm37::Type::isSized()
 */
LLVM37Bool LLVM37TypeIsSized(LLVM37TypeRef Ty);

/**
 * Obtain the context to which this type instance is associated.
 *
 * @see llvm37::Type::getContext()
 */
LLVM37ContextRef LLVM37GetTypeContext(LLVM37TypeRef Ty);

/**
 * Dump a representation of a type to stderr.
 *
 * @see llvm37::Type::dump()
 */
void LLVM37DumpType(LLVM37TypeRef Val);

/**
 * Return a string representation of the type. Use
 * LLVM37DisposeMessage to free the string.
 *
 * @see llvm37::Type::print()
 */
char *LLVM37PrintTypeToString(LLVM37TypeRef Val);

/**
 * @defgroup LLVM37CCoreTypeInt Integer Types
 *
 * Functions in this section operate on integer types.
 *
 * @{
 */

/**
 * Obtain an integer type from a context with specified bit width.
 */
LLVM37TypeRef LLVM37Int1TypeInContext(LLVM37ContextRef C);
LLVM37TypeRef LLVM37Int8TypeInContext(LLVM37ContextRef C);
LLVM37TypeRef LLVM37Int16TypeInContext(LLVM37ContextRef C);
LLVM37TypeRef LLVM37Int32TypeInContext(LLVM37ContextRef C);
LLVM37TypeRef LLVM37Int64TypeInContext(LLVM37ContextRef C);
LLVM37TypeRef LLVM37IntTypeInContext(LLVM37ContextRef C, unsigned NumBits);

/**
 * Obtain an integer type from the global context with a specified bit
 * width.
 */
LLVM37TypeRef LLVM37Int1Type(void);
LLVM37TypeRef LLVM37Int8Type(void);
LLVM37TypeRef LLVM37Int16Type(void);
LLVM37TypeRef LLVM37Int32Type(void);
LLVM37TypeRef LLVM37Int64Type(void);
LLVM37TypeRef LLVM37IntType(unsigned NumBits);
unsigned LLVM37GetIntTypeWidth(LLVM37TypeRef IntegerTy);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreTypeFloat Floating Point Types
 *
 * @{
 */

/**
 * Obtain a 16-bit floating point type from a context.
 */
LLVM37TypeRef LLVM37HalfTypeInContext(LLVM37ContextRef C);

/**
 * Obtain a 32-bit floating point type from a context.
 */
LLVM37TypeRef LLVM37FloatTypeInContext(LLVM37ContextRef C);

/**
 * Obtain a 64-bit floating point type from a context.
 */
LLVM37TypeRef LLVM37DoubleTypeInContext(LLVM37ContextRef C);

/**
 * Obtain a 80-bit floating point type (X87) from a context.
 */
LLVM37TypeRef LLVM37X86FP80TypeInContext(LLVM37ContextRef C);

/**
 * Obtain a 128-bit floating point type (112-bit mantissa) from a
 * context.
 */
LLVM37TypeRef LLVM37FP128TypeInContext(LLVM37ContextRef C);

/**
 * Obtain a 128-bit floating point type (two 64-bits) from a context.
 */
LLVM37TypeRef LLVM37PPCFP128TypeInContext(LLVM37ContextRef C);

/**
 * Obtain a floating point type from the global context.
 *
 * These map to the functions in this group of the same name.
 */
LLVM37TypeRef LLVM37HalfType(void);
LLVM37TypeRef LLVM37FloatType(void);
LLVM37TypeRef LLVM37DoubleType(void);
LLVM37TypeRef LLVM37X86FP80Type(void);
LLVM37TypeRef LLVM37FP128Type(void);
LLVM37TypeRef LLVM37PPCFP128Type(void);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreTypeFunction Function Types
 *
 * @{
 */

/**
 * Obtain a function type consisting of a specified signature.
 *
 * The function is defined as a tuple of a return Type, a list of
 * parameter types, and whether the function is variadic.
 */
LLVM37TypeRef LLVM37FunctionType(LLVM37TypeRef ReturnType,
                             LLVM37TypeRef *ParamTypes, unsigned ParamCount,
                             LLVM37Bool IsVarArg);

/**
 * Returns whether a function type is variadic.
 */
LLVM37Bool LLVM37IsFunctionVarArg(LLVM37TypeRef FunctionTy);

/**
 * Obtain the Type this function Type returns.
 */
LLVM37TypeRef LLVM37GetReturnType(LLVM37TypeRef FunctionTy);

/**
 * Obtain the number of parameters this function accepts.
 */
unsigned LLVM37CountParamTypes(LLVM37TypeRef FunctionTy);

/**
 * Obtain the types of a function's parameters.
 *
 * The Dest parameter should point to a pre-allocated array of
 * LLVM37TypeRef at least LLVM37CountParamTypes() large. On return, the
 * first LLVM37CountParamTypes() entries in the array will be populated
 * with LLVM37TypeRef instances.
 *
 * @param FunctionTy The function type to operate on.
 * @param Dest Memory address of an array to be filled with result.
 */
void LLVM37GetParamTypes(LLVM37TypeRef FunctionTy, LLVM37TypeRef *Dest);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreTypeStruct Structure Types
 *
 * These functions relate to LLVM37TypeRef instances.
 *
 * @see llvm37::StructType
 *
 * @{
 */

/**
 * Create a new structure type in a context.
 *
 * A structure is specified by a list of inner elements/types and
 * whether these can be packed together.
 *
 * @see llvm37::StructType::create()
 */
LLVM37TypeRef LLVM37StructTypeInContext(LLVM37ContextRef C, LLVM37TypeRef *ElementTypes,
                                    unsigned ElementCount, LLVM37Bool Packed);

/**
 * Create a new structure type in the global context.
 *
 * @see llvm37::StructType::create()
 */
LLVM37TypeRef LLVM37StructType(LLVM37TypeRef *ElementTypes, unsigned ElementCount,
                           LLVM37Bool Packed);

/**
 * Create an empty structure in a context having a specified name.
 *
 * @see llvm37::StructType::create()
 */
LLVM37TypeRef LLVM37StructCreateNamed(LLVM37ContextRef C, const char *Name);

/**
 * Obtain the name of a structure.
 *
 * @see llvm37::StructType::getName()
 */
const char *LLVM37GetStructName(LLVM37TypeRef Ty);

/**
 * Set the contents of a structure type.
 *
 * @see llvm37::StructType::setBody()
 */
void LLVM37StructSetBody(LLVM37TypeRef StructTy, LLVM37TypeRef *ElementTypes,
                       unsigned ElementCount, LLVM37Bool Packed);

/**
 * Get the number of elements defined inside the structure.
 *
 * @see llvm37::StructType::getNumElements()
 */
unsigned LLVM37CountStructElementTypes(LLVM37TypeRef StructTy);

/**
 * Get the elements within a structure.
 *
 * The function is passed the address of a pre-allocated array of
 * LLVM37TypeRef at least LLVM37CountStructElementTypes() long. After
 * invocation, this array will be populated with the structure's
 * elements. The objects in the destination array will have a lifetime
 * of the structure type itself, which is the lifetime of the context it
 * is contained in.
 */
void LLVM37GetStructElementTypes(LLVM37TypeRef StructTy, LLVM37TypeRef *Dest);

/**
 * Get the type of the element at a given index in the structure.
 *
 * @see llvm37::StructType::getTypeAtIndex()
 */
LLVM37TypeRef LLVM37StructGetTypeAtIndex(LLVM37TypeRef StructTy, unsigned i);

/**
 * Determine whether a structure is packed.
 *
 * @see llvm37::StructType::isPacked()
 */
LLVM37Bool LLVM37IsPackedStruct(LLVM37TypeRef StructTy);

/**
 * Determine whether a structure is opaque.
 *
 * @see llvm37::StructType::isOpaque()
 */
LLVM37Bool LLVM37IsOpaqueStruct(LLVM37TypeRef StructTy);

/**
 * @}
 */


/**
 * @defgroup LLVM37CCoreTypeSequential Sequential Types
 *
 * Sequential types represents "arrays" of types. This is a super class
 * for array, vector, and pointer types.
 *
 * @{
 */

/**
 * Obtain the type of elements within a sequential type.
 *
 * This works on array, vector, and pointer types.
 *
 * @see llvm37::SequentialType::getElementType()
 */
LLVM37TypeRef LLVM37GetElementType(LLVM37TypeRef Ty);

/**
 * Create a fixed size array type that refers to a specific type.
 *
 * The created type will exist in the context that its element type
 * exists in.
 *
 * @see llvm37::ArrayType::get()
 */
LLVM37TypeRef LLVM37ArrayType(LLVM37TypeRef ElementType, unsigned ElementCount);

/**
 * Obtain the length of an array type.
 *
 * This only works on types that represent arrays.
 *
 * @see llvm37::ArrayType::getNumElements()
 */
unsigned LLVM37GetArrayLength(LLVM37TypeRef ArrayTy);

/**
 * Create a pointer type that points to a defined type.
 *
 * The created type will exist in the context that its pointee type
 * exists in.
 *
 * @see llvm37::PointerType::get()
 */
LLVM37TypeRef LLVM37PointerType(LLVM37TypeRef ElementType, unsigned AddressSpace);

/**
 * Obtain the address space of a pointer type.
 *
 * This only works on types that represent pointers.
 *
 * @see llvm37::PointerType::getAddressSpace()
 */
unsigned LLVM37GetPointerAddressSpace(LLVM37TypeRef PointerTy);

/**
 * Create a vector type that contains a defined type and has a specific
 * number of elements.
 *
 * The created type will exist in the context thats its element type
 * exists in.
 *
 * @see llvm37::VectorType::get()
 */
LLVM37TypeRef LLVM37VectorType(LLVM37TypeRef ElementType, unsigned ElementCount);

/**
 * Obtain the number of elements in a vector type.
 *
 * This only works on types that represent vectors.
 *
 * @see llvm37::VectorType::getNumElements()
 */
unsigned LLVM37GetVectorSize(LLVM37TypeRef VectorTy);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreTypeOther Other Types
 *
 * @{
 */

/**
 * Create a void type in a context.
 */
LLVM37TypeRef LLVM37VoidTypeInContext(LLVM37ContextRef C);

/**
 * Create a label type in a context.
 */
LLVM37TypeRef LLVM37LabelTypeInContext(LLVM37ContextRef C);

/**
 * Create a X86 MMX type in a context.
 */
LLVM37TypeRef LLVM37X86MMXTypeInContext(LLVM37ContextRef C);

/**
 * These are similar to the above functions except they operate on the
 * global context.
 */
LLVM37TypeRef LLVM37VoidType(void);
LLVM37TypeRef LLVM37LabelType(void);
LLVM37TypeRef LLVM37X86MMXType(void);

/**
 * @}
 */

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreValues Values
 *
 * The bulk of LLVM37's object model consists of values, which comprise a very
 * rich type hierarchy.
 *
 * LLVM37ValueRef essentially represents llvm37::Value. There is a rich
 * hierarchy of classes within this type. Depending on the instance
 * obtained, not all APIs are available.
 *
 * Callers can determine the type of an LLVM37ValueRef by calling the
 * LLVM37IsA* family of functions (e.g. LLVM37IsAArgument()). These
 * functions are defined by a macro, so it isn't obvious which are
 * available by looking at the Doxygen source code. Instead, look at the
 * source definition of LLVM37_FOR_EACH_VALUE_SUBCLASS and note the list
 * of value names given. These value names also correspond to classes in
 * the llvm37::Value hierarchy.
 *
 * @{
 */

#define LLVM37_FOR_EACH_VALUE_SUBCLASS(macro) \
  macro(Argument)                           \
  macro(BasicBlock)                         \
  macro(InlineAsm)                          \
  macro(User)                               \
    macro(Constant)                         \
      macro(BlockAddress)                   \
      macro(ConstantAggregateZero)          \
      macro(ConstantArray)                  \
      macro(ConstantDataSequential)         \
        macro(ConstantDataArray)            \
        macro(ConstantDataVector)           \
      macro(ConstantExpr)                   \
      macro(ConstantFP)                     \
      macro(ConstantInt)                    \
      macro(ConstantPointerNull)            \
      macro(ConstantStruct)                 \
      macro(ConstantVector)                 \
      macro(GlobalValue)                    \
        macro(GlobalAlias)                  \
        macro(GlobalObject)                 \
          macro(Function)                   \
          macro(GlobalVariable)             \
      macro(UndefValue)                     \
    macro(Instruction)                      \
      macro(BinaryOperator)                 \
      macro(CallInst)                       \
        macro(IntrinsicInst)                \
          macro(DbgInfoIntrinsic)           \
            macro(DbgDeclareInst)           \
          macro(MemIntrinsic)               \
            macro(MemCpyInst)               \
            macro(MemMoveInst)              \
            macro(MemSetInst)               \
      macro(CmpInst)                        \
        macro(FCmpInst)                     \
        macro(ICmpInst)                     \
      macro(ExtractElementInst)             \
      macro(GetElementPtrInst)              \
      macro(InsertElementInst)              \
      macro(InsertValueInst)                \
      macro(LandingPadInst)                 \
      macro(PHINode)                        \
      macro(SelectInst)                     \
      macro(ShuffleVectorInst)              \
      macro(StoreInst)                      \
      macro(TerminatorInst)                 \
        macro(BranchInst)                   \
        macro(IndirectBrInst)               \
        macro(InvokeInst)                   \
        macro(ReturnInst)                   \
        macro(SwitchInst)                   \
        macro(UnreachableInst)              \
        macro(ResumeInst)                   \
      macro(UnaryInstruction)               \
        macro(AllocaInst)                   \
        macro(CastInst)                     \
          macro(AddrSpaceCastInst)          \
          macro(BitCastInst)                \
          macro(FPExtInst)                  \
          macro(FPToSIInst)                 \
          macro(FPToUIInst)                 \
          macro(FPTruncInst)                \
          macro(IntToPtrInst)               \
          macro(PtrToIntInst)               \
          macro(SExtInst)                   \
          macro(SIToFPInst)                 \
          macro(TruncInst)                  \
          macro(UIToFPInst)                 \
          macro(ZExtInst)                   \
        macro(ExtractValueInst)             \
        macro(LoadInst)                     \
        macro(VAArgInst)

/**
 * @defgroup LLVM37CCoreValueGeneral General APIs
 *
 * Functions in this section work on all LLVM37ValueRef instances,
 * regardless of their sub-type. They correspond to functions available
 * on llvm37::Value.
 *
 * @{
 */

/**
 * Obtain the type of a value.
 *
 * @see llvm37::Value::getType()
 */
LLVM37TypeRef LLVM37TypeOf(LLVM37ValueRef Val);

/**
 * Obtain the string name of a value.
 *
 * @see llvm37::Value::getName()
 */
const char *LLVM37GetValueName(LLVM37ValueRef Val);

/**
 * Set the string name of a value.
 *
 * @see llvm37::Value::setName()
 */
void LLVM37SetValueName(LLVM37ValueRef Val, const char *Name);

/**
 * Dump a representation of a value to stderr.
 *
 * @see llvm37::Value::dump()
 */
void LLVM37DumpValue(LLVM37ValueRef Val);

/**
 * Return a string representation of the value. Use
 * LLVM37DisposeMessage to free the string.
 *
 * @see llvm37::Value::print()
 */
char *LLVM37PrintValueToString(LLVM37ValueRef Val);

/**
 * Replace all uses of a value with another one.
 *
 * @see llvm37::Value::replaceAllUsesWith()
 */
void LLVM37ReplaceAllUsesWith(LLVM37ValueRef OldVal, LLVM37ValueRef NewVal);

/**
 * Determine whether the specified constant instance is constant.
 */
LLVM37Bool LLVM37IsConstant(LLVM37ValueRef Val);

/**
 * Determine whether a value instance is undefined.
 */
LLVM37Bool LLVM37IsUndef(LLVM37ValueRef Val);

/**
 * Convert value instances between types.
 *
 * Internally, an LLVM37ValueRef is "pinned" to a specific type. This
 * series of functions allows you to cast an instance to a specific
 * type.
 *
 * If the cast is not valid for the specified type, NULL is returned.
 *
 * @see llvm37::dyn_cast_or_null<>
 */
#define LLVM37_DECLARE_VALUE_CAST(name) \
  LLVM37ValueRef LLVM37IsA##name(LLVM37ValueRef Val);
LLVM37_FOR_EACH_VALUE_SUBCLASS(LLVM37_DECLARE_VALUE_CAST)

LLVM37ValueRef LLVM37IsAMDNode(LLVM37ValueRef Val);
LLVM37ValueRef LLVM37IsAMDString(LLVM37ValueRef Val);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreValueUses Usage
 *
 * This module defines functions that allow you to inspect the uses of a
 * LLVM37ValueRef.
 *
 * It is possible to obtain an LLVM37UseRef for any LLVM37ValueRef instance.
 * Each LLVM37UseRef (which corresponds to a llvm37::Use instance) holds a
 * llvm37::User and llvm37::Value.
 *
 * @{
 */

/**
 * Obtain the first use of a value.
 *
 * Uses are obtained in an iterator fashion. First, call this function
 * to obtain a reference to the first use. Then, call LLVM37GetNextUse()
 * on that instance and all subsequently obtained instances until
 * LLVM37GetNextUse() returns NULL.
 *
 * @see llvm37::Value::use_begin()
 */
LLVM37UseRef LLVM37GetFirstUse(LLVM37ValueRef Val);

/**
 * Obtain the next use of a value.
 *
 * This effectively advances the iterator. It returns NULL if you are on
 * the final use and no more are available.
 */
LLVM37UseRef LLVM37GetNextUse(LLVM37UseRef U);

/**
 * Obtain the user value for a user.
 *
 * The returned value corresponds to a llvm37::User type.
 *
 * @see llvm37::Use::getUser()
 */
LLVM37ValueRef LLVM37GetUser(LLVM37UseRef U);

/**
 * Obtain the value this use corresponds to.
 *
 * @see llvm37::Use::get().
 */
LLVM37ValueRef LLVM37GetUsedValue(LLVM37UseRef U);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreValueUser User value
 *
 * Function in this group pertain to LLVM37ValueRef instances that descent
 * from llvm37::User. This includes constants, instructions, and
 * operators.
 *
 * @{
 */

/**
 * Obtain an operand at a specific index in a llvm37::User value.
 *
 * @see llvm37::User::getOperand()
 */
LLVM37ValueRef LLVM37GetOperand(LLVM37ValueRef Val, unsigned Index);

/**
 * Obtain the use of an operand at a specific index in a llvm37::User value.
 *
 * @see llvm37::User::getOperandUse()
 */
LLVM37UseRef LLVM37GetOperandUse(LLVM37ValueRef Val, unsigned Index);

/**
 * Set an operand at a specific index in a llvm37::User value.
 *
 * @see llvm37::User::setOperand()
 */
void LLVM37SetOperand(LLVM37ValueRef User, unsigned Index, LLVM37ValueRef Val);

/**
 * Obtain the number of operands in a llvm37::User value.
 *
 * @see llvm37::User::getNumOperands()
 */
int LLVM37GetNumOperands(LLVM37ValueRef Val);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreValueConstant Constants
 *
 * This section contains APIs for interacting with LLVM37ValueRef that
 * correspond to llvm37::Constant instances.
 *
 * These functions will work for any LLVM37ValueRef in the llvm37::Constant
 * class hierarchy.
 *
 * @{
 */

/**
 * Obtain a constant value referring to the null instance of a type.
 *
 * @see llvm37::Constant::getNullValue()
 */
LLVM37ValueRef LLVM37ConstNull(LLVM37TypeRef Ty); /* all zeroes */

/**
 * Obtain a constant value referring to the instance of a type
 * consisting of all ones.
 *
 * This is only valid for integer types.
 *
 * @see llvm37::Constant::getAllOnesValue()
 */
LLVM37ValueRef LLVM37ConstAllOnes(LLVM37TypeRef Ty);

/**
 * Obtain a constant value referring to an undefined value of a type.
 *
 * @see llvm37::UndefValue::get()
 */
LLVM37ValueRef LLVM37GetUndef(LLVM37TypeRef Ty);

/**
 * Determine whether a value instance is null.
 *
 * @see llvm37::Constant::isNullValue()
 */
LLVM37Bool LLVM37IsNull(LLVM37ValueRef Val);

/**
 * Obtain a constant that is a constant pointer pointing to NULL for a
 * specified type.
 */
LLVM37ValueRef LLVM37ConstPointerNull(LLVM37TypeRef Ty);

/**
 * @defgroup LLVM37CCoreValueConstantScalar Scalar constants
 *
 * Functions in this group model LLVM37ValueRef instances that correspond
 * to constants referring to scalar types.
 *
 * For integer types, the LLVM37TypeRef parameter should correspond to a
 * llvm37::IntegerType instance and the returned LLVM37ValueRef will
 * correspond to a llvm37::ConstantInt.
 *
 * For floating point types, the LLVM37TypeRef returned corresponds to a
 * llvm37::ConstantFP.
 *
 * @{
 */

/**
 * Obtain a constant value for an integer type.
 *
 * The returned value corresponds to a llvm37::ConstantInt.
 *
 * @see llvm37::ConstantInt::get()
 *
 * @param IntTy Integer type to obtain value of.
 * @param N The value the returned instance should refer to.
 * @param SignExtend Whether to sign extend the produced value.
 */
LLVM37ValueRef LLVM37ConstInt(LLVM37TypeRef IntTy, unsigned long long N,
                          LLVM37Bool SignExtend);

/**
 * Obtain a constant value for an integer of arbitrary precision.
 *
 * @see llvm37::ConstantInt::get()
 */
LLVM37ValueRef LLVM37ConstIntOfArbitraryPrecision(LLVM37TypeRef IntTy,
                                              unsigned NumWords,
                                              const uint64_t Words[]);

/**
 * Obtain a constant value for an integer parsed from a string.
 *
 * A similar API, LLVM37ConstIntOfStringAndSize is also available. If the
 * string's length is available, it is preferred to call that function
 * instead.
 *
 * @see llvm37::ConstantInt::get()
 */
LLVM37ValueRef LLVM37ConstIntOfString(LLVM37TypeRef IntTy, const char *Text,
                                  uint8_t Radix);

/**
 * Obtain a constant value for an integer parsed from a string with
 * specified length.
 *
 * @see llvm37::ConstantInt::get()
 */
LLVM37ValueRef LLVM37ConstIntOfStringAndSize(LLVM37TypeRef IntTy, const char *Text,
                                         unsigned SLen, uint8_t Radix);

/**
 * Obtain a constant value referring to a double floating point value.
 */
LLVM37ValueRef LLVM37ConstReal(LLVM37TypeRef RealTy, double N);

/**
 * Obtain a constant for a floating point value parsed from a string.
 *
 * A similar API, LLVM37ConstRealOfStringAndSize is also available. It
 * should be used if the input string's length is known.
 */
LLVM37ValueRef LLVM37ConstRealOfString(LLVM37TypeRef RealTy, const char *Text);

/**
 * Obtain a constant for a floating point value parsed from a string.
 */
LLVM37ValueRef LLVM37ConstRealOfStringAndSize(LLVM37TypeRef RealTy, const char *Text,
                                          unsigned SLen);

/**
 * Obtain the zero extended value for an integer constant value.
 *
 * @see llvm37::ConstantInt::getZExtValue()
 */
unsigned long long LLVM37ConstIntGetZExtValue(LLVM37ValueRef ConstantVal);

/**
 * Obtain the sign extended value for an integer constant value.
 *
 * @see llvm37::ConstantInt::getSExtValue()
 */
long long LLVM37ConstIntGetSExtValue(LLVM37ValueRef ConstantVal);

/**
 * Obtain the double value for an floating point constant value.
 * losesInfo indicates if some precision was lost in the conversion.
 *
 * @see llvm37::ConstantFP::getDoubleValue
 */
double LLVM37ConstRealGetDouble(LLVM37ValueRef ConstantVal, LLVM37Bool *losesInfo);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreValueConstantComposite Composite Constants
 *
 * Functions in this group operate on composite constants.
 *
 * @{
 */

/**
 * Create a ConstantDataSequential and initialize it with a string.
 *
 * @see llvm37::ConstantDataArray::getString()
 */
LLVM37ValueRef LLVM37ConstStringInContext(LLVM37ContextRef C, const char *Str,
                                      unsigned Length, LLVM37Bool DontNullTerminate);

/**
 * Create a ConstantDataSequential with string content in the global context.
 *
 * This is the same as LLVM37ConstStringInContext except it operates on the
 * global context.
 *
 * @see LLVM37ConstStringInContext()
 * @see llvm37::ConstantDataArray::getString()
 */
LLVM37ValueRef LLVM37ConstString(const char *Str, unsigned Length,
                             LLVM37Bool DontNullTerminate);

/**
 * Returns true if the specified constant is an array of i8.
 *
 * @see ConstantDataSequential::getAsString()
 */
LLVM37Bool LLVM37IsConstantString(LLVM37ValueRef c);

/**
 * Get the given constant data sequential as a string.
 *
 * @see ConstantDataSequential::getAsString()
 */
const char *LLVM37GetAsString(LLVM37ValueRef c, size_t* out);

/**
 * Create an anonymous ConstantStruct with the specified values.
 *
 * @see llvm37::ConstantStruct::getAnon()
 */
LLVM37ValueRef LLVM37ConstStructInContext(LLVM37ContextRef C,
                                      LLVM37ValueRef *ConstantVals,
                                      unsigned Count, LLVM37Bool Packed);

/**
 * Create a ConstantStruct in the global Context.
 *
 * This is the same as LLVM37ConstStructInContext except it operates on the
 * global Context.
 *
 * @see LLVM37ConstStructInContext()
 */
LLVM37ValueRef LLVM37ConstStruct(LLVM37ValueRef *ConstantVals, unsigned Count,
                             LLVM37Bool Packed);

/**
 * Create a ConstantArray from values.
 *
 * @see llvm37::ConstantArray::get()
 */
LLVM37ValueRef LLVM37ConstArray(LLVM37TypeRef ElementTy,
                            LLVM37ValueRef *ConstantVals, unsigned Length);

/**
 * Create a non-anonymous ConstantStruct from values.
 *
 * @see llvm37::ConstantStruct::get()
 */
LLVM37ValueRef LLVM37ConstNamedStruct(LLVM37TypeRef StructTy,
                                  LLVM37ValueRef *ConstantVals,
                                  unsigned Count);

/**
 * Get an element at specified index as a constant.
 *
 * @see ConstantDataSequential::getElementAsConstant()
 */
LLVM37ValueRef LLVM37GetElementAsConstant(LLVM37ValueRef c, unsigned idx);

/**
 * Create a ConstantVector from values.
 *
 * @see llvm37::ConstantVector::get()
 */
LLVM37ValueRef LLVM37ConstVector(LLVM37ValueRef *ScalarConstantVals, unsigned Size);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreValueConstantExpressions Constant Expressions
 *
 * Functions in this group correspond to APIs on llvm37::ConstantExpr.
 *
 * @see llvm37::ConstantExpr.
 *
 * @{
 */
LLVM37Opcode LLVM37GetConstOpcode(LLVM37ValueRef ConstantVal);
LLVM37ValueRef LLVM37AlignOf(LLVM37TypeRef Ty);
LLVM37ValueRef LLVM37SizeOf(LLVM37TypeRef Ty);
LLVM37ValueRef LLVM37ConstNeg(LLVM37ValueRef ConstantVal);
LLVM37ValueRef LLVM37ConstNSWNeg(LLVM37ValueRef ConstantVal);
LLVM37ValueRef LLVM37ConstNUWNeg(LLVM37ValueRef ConstantVal);
LLVM37ValueRef LLVM37ConstFNeg(LLVM37ValueRef ConstantVal);
LLVM37ValueRef LLVM37ConstNot(LLVM37ValueRef ConstantVal);
LLVM37ValueRef LLVM37ConstAdd(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstNSWAdd(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstNUWAdd(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstFAdd(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstSub(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstNSWSub(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstNUWSub(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstFSub(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstMul(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstNSWMul(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstNUWMul(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstFMul(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstUDiv(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstSDiv(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstExactSDiv(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstFDiv(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstURem(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstSRem(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstFRem(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstAnd(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstOr(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstXor(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstICmp(LLVM37IntPredicate Predicate,
                           LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstFCmp(LLVM37RealPredicate Predicate,
                           LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstShl(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstLShr(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstAShr(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant);
LLVM37ValueRef LLVM37ConstGEP(LLVM37ValueRef ConstantVal,
                          LLVM37ValueRef *ConstantIndices, unsigned NumIndices);
LLVM37ValueRef LLVM37ConstInBoundsGEP(LLVM37ValueRef ConstantVal,
                                  LLVM37ValueRef *ConstantIndices,
                                  unsigned NumIndices);
LLVM37ValueRef LLVM37ConstTrunc(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType);
LLVM37ValueRef LLVM37ConstSExt(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType);
LLVM37ValueRef LLVM37ConstZExt(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType);
LLVM37ValueRef LLVM37ConstFPTrunc(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType);
LLVM37ValueRef LLVM37ConstFPExt(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType);
LLVM37ValueRef LLVM37ConstUIToFP(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType);
LLVM37ValueRef LLVM37ConstSIToFP(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType);
LLVM37ValueRef LLVM37ConstFPToUI(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType);
LLVM37ValueRef LLVM37ConstFPToSI(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType);
LLVM37ValueRef LLVM37ConstPtrToInt(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType);
LLVM37ValueRef LLVM37ConstIntToPtr(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType);
LLVM37ValueRef LLVM37ConstBitCast(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType);
LLVM37ValueRef LLVM37ConstAddrSpaceCast(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType);
LLVM37ValueRef LLVM37ConstZExtOrBitCast(LLVM37ValueRef ConstantVal,
                                    LLVM37TypeRef ToType);
LLVM37ValueRef LLVM37ConstSExtOrBitCast(LLVM37ValueRef ConstantVal,
                                    LLVM37TypeRef ToType);
LLVM37ValueRef LLVM37ConstTruncOrBitCast(LLVM37ValueRef ConstantVal,
                                     LLVM37TypeRef ToType);
LLVM37ValueRef LLVM37ConstPointerCast(LLVM37ValueRef ConstantVal,
                                  LLVM37TypeRef ToType);
LLVM37ValueRef LLVM37ConstIntCast(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType,
                              LLVM37Bool isSigned);
LLVM37ValueRef LLVM37ConstFPCast(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType);
LLVM37ValueRef LLVM37ConstSelect(LLVM37ValueRef ConstantCondition,
                             LLVM37ValueRef ConstantIfTrue,
                             LLVM37ValueRef ConstantIfFalse);
LLVM37ValueRef LLVM37ConstExtractElement(LLVM37ValueRef VectorConstant,
                                     LLVM37ValueRef IndexConstant);
LLVM37ValueRef LLVM37ConstInsertElement(LLVM37ValueRef VectorConstant,
                                    LLVM37ValueRef ElementValueConstant,
                                    LLVM37ValueRef IndexConstant);
LLVM37ValueRef LLVM37ConstShuffleVector(LLVM37ValueRef VectorAConstant,
                                    LLVM37ValueRef VectorBConstant,
                                    LLVM37ValueRef MaskConstant);
LLVM37ValueRef LLVM37ConstExtractValue(LLVM37ValueRef AggConstant, unsigned *IdxList,
                                   unsigned NumIdx);
LLVM37ValueRef LLVM37ConstInsertValue(LLVM37ValueRef AggConstant,
                                  LLVM37ValueRef ElementValueConstant,
                                  unsigned *IdxList, unsigned NumIdx);
LLVM37ValueRef LLVM37ConstInlineAsm(LLVM37TypeRef Ty,
                                const char *AsmString, const char *Constraints,
                                LLVM37Bool HasSideEffects, LLVM37Bool IsAlignStack);
LLVM37ValueRef LLVM37BlockAddress(LLVM37ValueRef F, LLVM37BasicBlockRef BB);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreValueConstantGlobals Global Values
 *
 * This group contains functions that operate on global values. Functions in
 * this group relate to functions in the llvm37::GlobalValue class tree.
 *
 * @see llvm37::GlobalValue
 *
 * @{
 */

LLVM37ModuleRef LLVM37GetGlobalParent(LLVM37ValueRef Global);
LLVM37Bool LLVM37IsDeclaration(LLVM37ValueRef Global);
LLVM37Linkage LLVM37GetLinkage(LLVM37ValueRef Global);
void LLVM37SetLinkage(LLVM37ValueRef Global, LLVM37Linkage Linkage);
const char *LLVM37GetSection(LLVM37ValueRef Global);
void LLVM37SetSection(LLVM37ValueRef Global, const char *Section);
LLVM37Visibility LLVM37GetVisibility(LLVM37ValueRef Global);
void LLVM37SetVisibility(LLVM37ValueRef Global, LLVM37Visibility Viz);
LLVM37DLLStorageClass LLVM37GetDLLStorageClass(LLVM37ValueRef Global);
void LLVM37SetDLLStorageClass(LLVM37ValueRef Global, LLVM37DLLStorageClass Class);
LLVM37Bool LLVM37HasUnnamedAddr(LLVM37ValueRef Global);
void LLVM37SetUnnamedAddr(LLVM37ValueRef Global, LLVM37Bool HasUnnamedAddr);

/**
 * @defgroup LLVM37CCoreValueWithAlignment Values with alignment
 *
 * Functions in this group only apply to values with alignment, i.e.
 * global variables, load and store instructions.
 */

/**
 * Obtain the preferred alignment of the value.
 * @see llvm37::AllocaInst::getAlignment()
 * @see llvm37::LoadInst::getAlignment()
 * @see llvm37::StoreInst::getAlignment()
 * @see llvm37::GlobalValue::getAlignment()
 */
unsigned LLVM37GetAlignment(LLVM37ValueRef V);

/**
 * Set the preferred alignment of the value.
 * @see llvm37::AllocaInst::setAlignment()
 * @see llvm37::LoadInst::setAlignment()
 * @see llvm37::StoreInst::setAlignment()
 * @see llvm37::GlobalValue::setAlignment()
 */
void LLVM37SetAlignment(LLVM37ValueRef V, unsigned Bytes);

/**
  * @}
  */

/**
 * @defgroup LLVM37CoreValueConstantGlobalVariable Global Variables
 *
 * This group contains functions that operate on global variable values.
 *
 * @see llvm37::GlobalVariable
 *
 * @{
 */
LLVM37ValueRef LLVM37AddGlobal(LLVM37ModuleRef M, LLVM37TypeRef Ty, const char *Name);
LLVM37ValueRef LLVM37AddGlobalInAddressSpace(LLVM37ModuleRef M, LLVM37TypeRef Ty,
                                         const char *Name,
                                         unsigned AddressSpace);
LLVM37ValueRef LLVM37GetNamedGlobal(LLVM37ModuleRef M, const char *Name);
LLVM37ValueRef LLVM37GetFirstGlobal(LLVM37ModuleRef M);
LLVM37ValueRef LLVM37GetLastGlobal(LLVM37ModuleRef M);
LLVM37ValueRef LLVM37GetNextGlobal(LLVM37ValueRef GlobalVar);
LLVM37ValueRef LLVM37GetPreviousGlobal(LLVM37ValueRef GlobalVar);
void LLVM37DeleteGlobal(LLVM37ValueRef GlobalVar);
LLVM37ValueRef LLVM37GetInitializer(LLVM37ValueRef GlobalVar);
void LLVM37SetInitializer(LLVM37ValueRef GlobalVar, LLVM37ValueRef ConstantVal);
LLVM37Bool LLVM37IsThreadLocal(LLVM37ValueRef GlobalVar);
void LLVM37SetThreadLocal(LLVM37ValueRef GlobalVar, LLVM37Bool IsThreadLocal);
LLVM37Bool LLVM37IsGlobalConstant(LLVM37ValueRef GlobalVar);
void LLVM37SetGlobalConstant(LLVM37ValueRef GlobalVar, LLVM37Bool IsConstant);
LLVM37ThreadLocalMode LLVM37GetThreadLocalMode(LLVM37ValueRef GlobalVar);
void LLVM37SetThreadLocalMode(LLVM37ValueRef GlobalVar, LLVM37ThreadLocalMode Mode);
LLVM37Bool LLVM37IsExternallyInitialized(LLVM37ValueRef GlobalVar);
void LLVM37SetExternallyInitialized(LLVM37ValueRef GlobalVar, LLVM37Bool IsExtInit);

/**
 * @}
 */

/**
 * @defgroup LLVM37CoreValueConstantGlobalAlias Global Aliases
 *
 * This group contains function that operate on global alias values.
 *
 * @see llvm37::GlobalAlias
 *
 * @{
 */
LLVM37ValueRef LLVM37AddAlias(LLVM37ModuleRef M, LLVM37TypeRef Ty, LLVM37ValueRef Aliasee,
                          const char *Name);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreValueFunction Function values
 *
 * Functions in this group operate on LLVM37ValueRef instances that
 * correspond to llvm37::Function instances.
 *
 * @see llvm37::Function
 *
 * @{
 */

/**
 * Remove a function from its containing module and deletes it.
 *
 * @see llvm37::Function::eraseFromParent()
 */
void LLVM37DeleteFunction(LLVM37ValueRef Fn);

/**
 * Obtain the personality function attached to the function.
 *
 * @see llvm37::Function::getPersonalityFn()
 */
LLVM37ValueRef LLVM37GetPersonalityFn(LLVM37ValueRef Fn);

/**
 * Set the personality function attached to the function.
 *
 * @see llvm37::Function::setPersonalityFn()
 */
void LLVM37SetPersonalityFn(LLVM37ValueRef Fn, LLVM37ValueRef PersonalityFn);

/**
 * Obtain the ID number from a function instance.
 *
 * @see llvm37::Function::getIntrinsicID()
 */
unsigned LLVM37GetIntrinsicID(LLVM37ValueRef Fn);

/**
 * Obtain the calling function of a function.
 *
 * The returned value corresponds to the LLVM37CallConv enumeration.
 *
 * @see llvm37::Function::getCallingConv()
 */
unsigned LLVM37GetFunctionCallConv(LLVM37ValueRef Fn);

/**
 * Set the calling convention of a function.
 *
 * @see llvm37::Function::setCallingConv()
 *
 * @param Fn Function to operate on
 * @param CC LLVM37CallConv to set calling convention to
 */
void LLVM37SetFunctionCallConv(LLVM37ValueRef Fn, unsigned CC);

/**
 * Obtain the name of the garbage collector to use during code
 * generation.
 *
 * @see llvm37::Function::getGC()
 */
const char *LLVM37GetGC(LLVM37ValueRef Fn);

/**
 * Define the garbage collector to use during code generation.
 *
 * @see llvm37::Function::setGC()
 */
void LLVM37SetGC(LLVM37ValueRef Fn, const char *Name);

/**
 * Add an attribute to a function.
 *
 * @see llvm37::Function::addAttribute()
 */
void LLVM37AddFunctionAttr(LLVM37ValueRef Fn, LLVM37Attribute PA);

/**
 * Add a target-dependent attribute to a fuction
 * @see llvm37::AttrBuilder::addAttribute()
 */
void LLVM37AddTargetDependentFunctionAttr(LLVM37ValueRef Fn, const char *A,
                                        const char *V);

/**
 * Obtain an attribute from a function.
 *
 * @see llvm37::Function::getAttributes()
 */
LLVM37Attribute LLVM37GetFunctionAttr(LLVM37ValueRef Fn);

/**
 * Remove an attribute from a function.
 */
void LLVM37RemoveFunctionAttr(LLVM37ValueRef Fn, LLVM37Attribute PA);

/**
 * @defgroup LLVM37CCoreValueFunctionParameters Function Parameters
 *
 * Functions in this group relate to arguments/parameters on functions.
 *
 * Functions in this group expect LLVM37ValueRef instances that correspond
 * to llvm37::Function instances.
 *
 * @{
 */

/**
 * Obtain the number of parameters in a function.
 *
 * @see llvm37::Function::arg_size()
 */
unsigned LLVM37CountParams(LLVM37ValueRef Fn);

/**
 * Obtain the parameters in a function.
 *
 * The takes a pointer to a pre-allocated array of LLVM37ValueRef that is
 * at least LLVM37CountParams() long. This array will be filled with
 * LLVM37ValueRef instances which correspond to the parameters the
 * function receives. Each LLVM37ValueRef corresponds to a llvm37::Argument
 * instance.
 *
 * @see llvm37::Function::arg_begin()
 */
void LLVM37GetParams(LLVM37ValueRef Fn, LLVM37ValueRef *Params);

/**
 * Obtain the parameter at the specified index.
 *
 * Parameters are indexed from 0.
 *
 * @see llvm37::Function::arg_begin()
 */
LLVM37ValueRef LLVM37GetParam(LLVM37ValueRef Fn, unsigned Index);

/**
 * Obtain the function to which this argument belongs.
 *
 * Unlike other functions in this group, this one takes an LLVM37ValueRef
 * that corresponds to a llvm37::Attribute.
 *
 * The returned LLVM37ValueRef is the llvm37::Function to which this
 * argument belongs.
 */
LLVM37ValueRef LLVM37GetParamParent(LLVM37ValueRef Inst);

/**
 * Obtain the first parameter to a function.
 *
 * @see llvm37::Function::arg_begin()
 */
LLVM37ValueRef LLVM37GetFirstParam(LLVM37ValueRef Fn);

/**
 * Obtain the last parameter to a function.
 *
 * @see llvm37::Function::arg_end()
 */
LLVM37ValueRef LLVM37GetLastParam(LLVM37ValueRef Fn);

/**
 * Obtain the next parameter to a function.
 *
 * This takes an LLVM37ValueRef obtained from LLVM37GetFirstParam() (which is
 * actually a wrapped iterator) and obtains the next parameter from the
 * underlying iterator.
 */
LLVM37ValueRef LLVM37GetNextParam(LLVM37ValueRef Arg);

/**
 * Obtain the previous parameter to a function.
 *
 * This is the opposite of LLVM37GetNextParam().
 */
LLVM37ValueRef LLVM37GetPreviousParam(LLVM37ValueRef Arg);

/**
 * Add an attribute to a function argument.
 *
 * @see llvm37::Argument::addAttr()
 */
void LLVM37AddAttribute(LLVM37ValueRef Arg, LLVM37Attribute PA);

/**
 * Remove an attribute from a function argument.
 *
 * @see llvm37::Argument::removeAttr()
 */
void LLVM37RemoveAttribute(LLVM37ValueRef Arg, LLVM37Attribute PA);

/**
 * Get an attribute from a function argument.
 */
LLVM37Attribute LLVM37GetAttribute(LLVM37ValueRef Arg);

/**
 * Set the alignment for a function parameter.
 *
 * @see llvm37::Argument::addAttr()
 * @see llvm37::AttrBuilder::addAlignmentAttr()
 */
void LLVM37SetParamAlignment(LLVM37ValueRef Arg, unsigned align);

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreValueMetadata Metadata
 *
 * @{
 */

/**
 * Obtain a MDString value from a context.
 *
 * The returned instance corresponds to the llvm37::MDString class.
 *
 * The instance is specified by string data of a specified length. The
 * string content is copied, so the backing memory can be freed after
 * this function returns.
 */
LLVM37ValueRef LLVM37MDStringInContext(LLVM37ContextRef C, const char *Str,
                                   unsigned SLen);

/**
 * Obtain a MDString value from the global context.
 */
LLVM37ValueRef LLVM37MDString(const char *Str, unsigned SLen);

/**
 * Obtain a MDNode value from a context.
 *
 * The returned value corresponds to the llvm37::MDNode class.
 */
LLVM37ValueRef LLVM37MDNodeInContext(LLVM37ContextRef C, LLVM37ValueRef *Vals,
                                 unsigned Count);

/**
 * Obtain a MDNode value from the global context.
 */
LLVM37ValueRef LLVM37MDNode(LLVM37ValueRef *Vals, unsigned Count);

/**
 * Obtain the underlying string from a MDString value.
 *
 * @param V Instance to obtain string from.
 * @param Len Memory address which will hold length of returned string.
 * @return String data in MDString.
 */
const char  *LLVM37GetMDString(LLVM37ValueRef V, unsigned* Len);

/**
 * Obtain the number of operands from an MDNode value.
 *
 * @param V MDNode to get number of operands from.
 * @return Number of operands of the MDNode.
 */
unsigned LLVM37GetMDNodeNumOperands(LLVM37ValueRef V);

/**
 * Obtain the given MDNode's operands.
 *
 * The passed LLVM37ValueRef pointer should point to enough memory to hold all of
 * the operands of the given MDNode (see LLVM37GetMDNodeNumOperands) as
 * LLVM37ValueRefs. This memory will be populated with the LLVM37ValueRefs of the
 * MDNode's operands.
 *
 * @param V MDNode to get the operands from.
 * @param Dest Destination array for operands.
 */
void LLVM37GetMDNodeOperands(LLVM37ValueRef V, LLVM37ValueRef *Dest);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreValueBasicBlock Basic Block
 *
 * A basic block represents a single entry single exit section of code.
 * Basic blocks contain a list of instructions which form the body of
 * the block.
 *
 * Basic blocks belong to functions. They have the type of label.
 *
 * Basic blocks are themselves values. However, the C API models them as
 * LLVM37BasicBlockRef.
 *
 * @see llvm37::BasicBlock
 *
 * @{
 */

/**
 * Convert a basic block instance to a value type.
 */
LLVM37ValueRef LLVM37BasicBlockAsValue(LLVM37BasicBlockRef BB);

/**
 * Determine whether an LLVM37ValueRef is itself a basic block.
 */
LLVM37Bool LLVM37ValueIsBasicBlock(LLVM37ValueRef Val);

/**
 * Convert an LLVM37ValueRef to an LLVM37BasicBlockRef instance.
 */
LLVM37BasicBlockRef LLVM37ValueAsBasicBlock(LLVM37ValueRef Val);

/**
 * Obtain the function to which a basic block belongs.
 *
 * @see llvm37::BasicBlock::getParent()
 */
LLVM37ValueRef LLVM37GetBasicBlockParent(LLVM37BasicBlockRef BB);

/**
 * Obtain the terminator instruction for a basic block.
 *
 * If the basic block does not have a terminator (it is not well-formed
 * if it doesn't), then NULL is returned.
 *
 * The returned LLVM37ValueRef corresponds to a llvm37::TerminatorInst.
 *
 * @see llvm37::BasicBlock::getTerminator()
 */
LLVM37ValueRef LLVM37GetBasicBlockTerminator(LLVM37BasicBlockRef BB);

/**
 * Obtain the number of basic blocks in a function.
 *
 * @param Fn Function value to operate on.
 */
unsigned LLVM37CountBasicBlocks(LLVM37ValueRef Fn);

/**
 * Obtain all of the basic blocks in a function.
 *
 * This operates on a function value. The BasicBlocks parameter is a
 * pointer to a pre-allocated array of LLVM37BasicBlockRef of at least
 * LLVM37CountBasicBlocks() in length. This array is populated with
 * LLVM37BasicBlockRef instances.
 */
void LLVM37GetBasicBlocks(LLVM37ValueRef Fn, LLVM37BasicBlockRef *BasicBlocks);

/**
 * Obtain the first basic block in a function.
 *
 * The returned basic block can be used as an iterator. You will likely
 * eventually call into LLVM37GetNextBasicBlock() with it.
 *
 * @see llvm37::Function::begin()
 */
LLVM37BasicBlockRef LLVM37GetFirstBasicBlock(LLVM37ValueRef Fn);

/**
 * Obtain the last basic block in a function.
 *
 * @see llvm37::Function::end()
 */
LLVM37BasicBlockRef LLVM37GetLastBasicBlock(LLVM37ValueRef Fn);

/**
 * Advance a basic block iterator.
 */
LLVM37BasicBlockRef LLVM37GetNextBasicBlock(LLVM37BasicBlockRef BB);

/**
 * Go backwards in a basic block iterator.
 */
LLVM37BasicBlockRef LLVM37GetPreviousBasicBlock(LLVM37BasicBlockRef BB);

/**
 * Obtain the basic block that corresponds to the entry point of a
 * function.
 *
 * @see llvm37::Function::getEntryBlock()
 */
LLVM37BasicBlockRef LLVM37GetEntryBasicBlock(LLVM37ValueRef Fn);

/**
 * Append a basic block to the end of a function.
 *
 * @see llvm37::BasicBlock::Create()
 */
LLVM37BasicBlockRef LLVM37AppendBasicBlockInContext(LLVM37ContextRef C,
                                                LLVM37ValueRef Fn,
                                                const char *Name);

/**
 * Append a basic block to the end of a function using the global
 * context.
 *
 * @see llvm37::BasicBlock::Create()
 */
LLVM37BasicBlockRef LLVM37AppendBasicBlock(LLVM37ValueRef Fn, const char *Name);

/**
 * Insert a basic block in a function before another basic block.
 *
 * The function to add to is determined by the function of the
 * passed basic block.
 *
 * @see llvm37::BasicBlock::Create()
 */
LLVM37BasicBlockRef LLVM37InsertBasicBlockInContext(LLVM37ContextRef C,
                                                LLVM37BasicBlockRef BB,
                                                const char *Name);

/**
 * Insert a basic block in a function using the global context.
 *
 * @see llvm37::BasicBlock::Create()
 */
LLVM37BasicBlockRef LLVM37InsertBasicBlock(LLVM37BasicBlockRef InsertBeforeBB,
                                       const char *Name);

/**
 * Remove a basic block from a function and delete it.
 *
 * This deletes the basic block from its containing function and deletes
 * the basic block itself.
 *
 * @see llvm37::BasicBlock::eraseFromParent()
 */
void LLVM37DeleteBasicBlock(LLVM37BasicBlockRef BB);

/**
 * Remove a basic block from a function.
 *
 * This deletes the basic block from its containing function but keep
 * the basic block alive.
 *
 * @see llvm37::BasicBlock::removeFromParent()
 */
void LLVM37RemoveBasicBlockFromParent(LLVM37BasicBlockRef BB);

/**
 * Move a basic block to before another one.
 *
 * @see llvm37::BasicBlock::moveBefore()
 */
void LLVM37MoveBasicBlockBefore(LLVM37BasicBlockRef BB, LLVM37BasicBlockRef MovePos);

/**
 * Move a basic block to after another one.
 *
 * @see llvm37::BasicBlock::moveAfter()
 */
void LLVM37MoveBasicBlockAfter(LLVM37BasicBlockRef BB, LLVM37BasicBlockRef MovePos);

/**
 * Obtain the first instruction in a basic block.
 *
 * The returned LLVM37ValueRef corresponds to a llvm37::Instruction
 * instance.
 */
LLVM37ValueRef LLVM37GetFirstInstruction(LLVM37BasicBlockRef BB);

/**
 * Obtain the last instruction in a basic block.
 *
 * The returned LLVM37ValueRef corresponds to an LLVM37:Instruction.
 */
LLVM37ValueRef LLVM37GetLastInstruction(LLVM37BasicBlockRef BB);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreValueInstruction Instructions
 *
 * Functions in this group relate to the inspection and manipulation of
 * individual instructions.
 *
 * In the C++ API, an instruction is modeled by llvm37::Instruction. This
 * class has a large number of descendents. llvm37::Instruction is a
 * llvm37::Value and in the C API, instructions are modeled by
 * LLVM37ValueRef.
 *
 * This group also contains sub-groups which operate on specific
 * llvm37::Instruction types, e.g. llvm37::CallInst.
 *
 * @{
 */

/**
 * Determine whether an instruction has any metadata attached.
 */
int LLVM37HasMetadata(LLVM37ValueRef Val);

/**
 * Return metadata associated with an instruction value.
 */
LLVM37ValueRef LLVM37GetMetadata(LLVM37ValueRef Val, unsigned KindID);

/**
 * Set metadata associated with an instruction value.
 */
void LLVM37SetMetadata(LLVM37ValueRef Val, unsigned KindID, LLVM37ValueRef Node);

/**
 * Obtain the basic block to which an instruction belongs.
 *
 * @see llvm37::Instruction::getParent()
 */
LLVM37BasicBlockRef LLVM37GetInstructionParent(LLVM37ValueRef Inst);

/**
 * Obtain the instruction that occurs after the one specified.
 *
 * The next instruction will be from the same basic block.
 *
 * If this is the last instruction in a basic block, NULL will be
 * returned.
 */
LLVM37ValueRef LLVM37GetNextInstruction(LLVM37ValueRef Inst);

/**
 * Obtain the instruction that occurred before this one.
 *
 * If the instruction is the first instruction in a basic block, NULL
 * will be returned.
 */
LLVM37ValueRef LLVM37GetPreviousInstruction(LLVM37ValueRef Inst);

/**
 * Remove and delete an instruction.
 *
 * The instruction specified is removed from its containing building
 * block and then deleted.
 *
 * @see llvm37::Instruction::eraseFromParent()
 */
void LLVM37InstructionEraseFromParent(LLVM37ValueRef Inst);

/**
 * Obtain the code opcode for an individual instruction.
 *
 * @see llvm37::Instruction::getOpCode()
 */
LLVM37Opcode   LLVM37GetInstructionOpcode(LLVM37ValueRef Inst);

/**
 * Obtain the predicate of an instruction.
 *
 * This is only valid for instructions that correspond to llvm37::ICmpInst
 * or llvm37::ConstantExpr whose opcode is llvm37::Instruction::ICmp.
 *
 * @see llvm37::ICmpInst::getPredicate()
 */
LLVM37IntPredicate LLVM37GetICmpPredicate(LLVM37ValueRef Inst);

/**
 * Obtain the float predicate of an instruction.
 *
 * This is only valid for instructions that correspond to llvm37::FCmpInst
 * or llvm37::ConstantExpr whose opcode is llvm37::Instruction::FCmp.
 *
 * @see llvm37::FCmpInst::getPredicate()
 */
LLVM37RealPredicate LLVM37GetFCmpPredicate(LLVM37ValueRef Inst);

/**
 * Create a copy of 'this' instruction that is identical in all ways
 * except the following:
 *   * The instruction has no parent
 *   * The instruction has no name
 *
 * @see llvm37::Instruction::clone()
 */
LLVM37ValueRef LLVM37InstructionClone(LLVM37ValueRef Inst);

/**
 * @defgroup LLVM37CCoreValueInstructionCall Call Sites and Invocations
 *
 * Functions in this group apply to instructions that refer to call
 * sites and invocations. These correspond to C++ types in the
 * llvm37::CallInst class tree.
 *
 * @{
 */

/**
 * Set the calling convention for a call instruction.
 *
 * This expects an LLVM37ValueRef that corresponds to a llvm37::CallInst or
 * llvm37::InvokeInst.
 *
 * @see llvm37::CallInst::setCallingConv()
 * @see llvm37::InvokeInst::setCallingConv()
 */
void LLVM37SetInstructionCallConv(LLVM37ValueRef Instr, unsigned CC);

/**
 * Obtain the calling convention for a call instruction.
 *
 * This is the opposite of LLVM37SetInstructionCallConv(). Reads its
 * usage.
 *
 * @see LLVM37SetInstructionCallConv()
 */
unsigned LLVM37GetInstructionCallConv(LLVM37ValueRef Instr);


void LLVM37AddInstrAttribute(LLVM37ValueRef Instr, unsigned index, LLVM37Attribute);
void LLVM37RemoveInstrAttribute(LLVM37ValueRef Instr, unsigned index,
                              LLVM37Attribute);
void LLVM37SetInstrParamAlignment(LLVM37ValueRef Instr, unsigned index,
                                unsigned align);

/**
 * Obtain whether a call instruction is a tail call.
 *
 * This only works on llvm37::CallInst instructions.
 *
 * @see llvm37::CallInst::isTailCall()
 */
LLVM37Bool LLVM37IsTailCall(LLVM37ValueRef CallInst);

/**
 * Set whether a call instruction is a tail call.
 *
 * This only works on llvm37::CallInst instructions.
 *
 * @see llvm37::CallInst::setTailCall()
 */
void LLVM37SetTailCall(LLVM37ValueRef CallInst, LLVM37Bool IsTailCall);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreValueInstructionTerminator Terminators
 *
 * Functions in this group only apply to instructions that map to
 * llvm37::TerminatorInst instances.
 *
 * @{
 */

/**
 * Return the number of successors that this terminator has.
 *
 * @see llvm37::TerminatorInst::getNumSuccessors
 */
unsigned LLVM37GetNumSuccessors(LLVM37ValueRef Term);

/**
 * Return the specified successor.
 *
 * @see llvm37::TerminatorInst::getSuccessor
 */
LLVM37BasicBlockRef LLVM37GetSuccessor(LLVM37ValueRef Term, unsigned i);

/**
 * Update the specified successor to point at the provided block.
 *
 * @see llvm37::TerminatorInst::setSuccessor
 */
void LLVM37SetSuccessor(LLVM37ValueRef Term, unsigned i, LLVM37BasicBlockRef block);

/**
 * Return if a branch is conditional.
 *
 * This only works on llvm37::BranchInst instructions.
 *
 * @see llvm37::BranchInst::isConditional
 */
LLVM37Bool LLVM37IsConditional(LLVM37ValueRef Branch);

/**
 * Return the condition of a branch instruction.
 *
 * This only works on llvm37::BranchInst instructions.
 *
 * @see llvm37::BranchInst::getCondition
 */
LLVM37ValueRef LLVM37GetCondition(LLVM37ValueRef Branch);

/**
 * Set the condition of a branch instruction.
 *
 * This only works on llvm37::BranchInst instructions.
 *
 * @see llvm37::BranchInst::setCondition
 */
void LLVM37SetCondition(LLVM37ValueRef Branch, LLVM37ValueRef Cond);

/**
 * Obtain the default destination basic block of a switch instruction.
 *
 * This only works on llvm37::SwitchInst instructions.
 *
 * @see llvm37::SwitchInst::getDefaultDest()
 */
LLVM37BasicBlockRef LLVM37GetSwitchDefaultDest(LLVM37ValueRef SwitchInstr);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreValueInstructionPHINode PHI Nodes
 *
 * Functions in this group only apply to instructions that map to
 * llvm37::PHINode instances.
 *
 * @{
 */

/**
 * Add an incoming value to the end of a PHI list.
 */
void LLVM37AddIncoming(LLVM37ValueRef PhiNode, LLVM37ValueRef *IncomingValues,
                     LLVM37BasicBlockRef *IncomingBlocks, unsigned Count);

/**
 * Obtain the number of incoming basic blocks to a PHI node.
 */
unsigned LLVM37CountIncoming(LLVM37ValueRef PhiNode);

/**
 * Obtain an incoming value to a PHI node as an LLVM37ValueRef.
 */
LLVM37ValueRef LLVM37GetIncomingValue(LLVM37ValueRef PhiNode, unsigned Index);

/**
 * Obtain an incoming value to a PHI node as an LLVM37BasicBlockRef.
 */
LLVM37BasicBlockRef LLVM37GetIncomingBlock(LLVM37ValueRef PhiNode, unsigned Index);

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreInstructionBuilder Instruction Builders
 *
 * An instruction builder represents a point within a basic block and is
 * the exclusive means of building instructions using the C interface.
 *
 * @{
 */

LLVMBuilderRef LLVM37CreateBuilderInContext(LLVM37ContextRef C);
LLVMBuilderRef LLVM37CreateBuilder(void);
void LLVM37PositionBuilder(LLVMBuilderRef Builder, LLVM37BasicBlockRef Block,
                         LLVM37ValueRef Instr);
void LLVM37PositionBuilderBefore(LLVMBuilderRef Builder, LLVM37ValueRef Instr);
void LLVM37PositionBuilderAtEnd(LLVMBuilderRef Builder, LLVM37BasicBlockRef Block);
LLVM37BasicBlockRef LLVM37GetInsertBlock(LLVMBuilderRef Builder);
void LLVM37ClearInsertionPosition(LLVMBuilderRef Builder);
void LLVM37InsertIntoBuilder(LLVMBuilderRef Builder, LLVM37ValueRef Instr);
void LLVM37InsertIntoBuilderWithName(LLVMBuilderRef Builder, LLVM37ValueRef Instr,
                                   const char *Name);
void LLVM37DisposeBuilder(LLVMBuilderRef Builder);

/* Metadata */
void LLVM37SetCurrentDebugLocation(LLVMBuilderRef Builder, LLVM37ValueRef L);
LLVM37ValueRef LLVM37GetCurrentDebugLocation(LLVMBuilderRef Builder);
void LLVM37SetInstDebugLocation(LLVMBuilderRef Builder, LLVM37ValueRef Inst);

/* Terminators */
LLVM37ValueRef LLVMBuildRetVoid(LLVMBuilderRef);
LLVM37ValueRef LLVMBuildRet(LLVMBuilderRef, LLVM37ValueRef V);
LLVM37ValueRef LLVMBuildAggregateRet(LLVMBuilderRef, LLVM37ValueRef *RetVals,
                                   unsigned N);
LLVM37ValueRef LLVMBuildBr(LLVMBuilderRef, LLVM37BasicBlockRef Dest);
LLVM37ValueRef LLVMBuildCondBr(LLVMBuilderRef, LLVM37ValueRef If,
                             LLVM37BasicBlockRef Then, LLVM37BasicBlockRef Else);
LLVM37ValueRef LLVMBuildSwitch(LLVMBuilderRef, LLVM37ValueRef V,
                             LLVM37BasicBlockRef Else, unsigned NumCases);
LLVM37ValueRef LLVMBuildIndirectBr(LLVMBuilderRef B, LLVM37ValueRef Addr,
                                 unsigned NumDests);
LLVM37ValueRef LLVMBuildInvoke(LLVMBuilderRef, LLVM37ValueRef Fn,
                             LLVM37ValueRef *Args, unsigned NumArgs,
                             LLVM37BasicBlockRef Then, LLVM37BasicBlockRef Catch,
                             const char *Name);
LLVM37ValueRef LLVMBuildLandingPad(LLVMBuilderRef B, LLVM37TypeRef Ty,
                                 unsigned NumClauses, const char *Name);
LLVM37ValueRef LLVMBuildResume(LLVMBuilderRef B, LLVM37ValueRef Exn);
LLVM37ValueRef LLVMBuildUnreachable(LLVMBuilderRef);

/* Add a case to the switch instruction */
void LLVM37AddCase(LLVM37ValueRef Switch, LLVM37ValueRef OnVal,
                 LLVM37BasicBlockRef Dest);

/* Add a destination to the indirectbr instruction */
void LLVM37AddDestination(LLVM37ValueRef IndirectBr, LLVM37BasicBlockRef Dest);

/* Add a catch or filter clause to the landingpad instruction */
void LLVM37AddClause(LLVM37ValueRef LandingPad, LLVM37ValueRef ClauseVal);

/* Set the 'cleanup' flag in the landingpad instruction */
void LLVM37SetCleanup(LLVM37ValueRef LandingPad, LLVM37Bool Val);

/* Arithmetic */
LLVM37ValueRef LLVMBuildAdd(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name);
LLVM37ValueRef LLVMBuildNSWAdd(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                             const char *Name);
LLVM37ValueRef LLVMBuildNUWAdd(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                             const char *Name);
LLVM37ValueRef LLVMBuildFAdd(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name);
LLVM37ValueRef LLVMBuildSub(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name);
LLVM37ValueRef LLVMBuildNSWSub(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                             const char *Name);
LLVM37ValueRef LLVMBuildNUWSub(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                             const char *Name);
LLVM37ValueRef LLVMBuildFSub(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name);
LLVM37ValueRef LLVMBuildMul(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name);
LLVM37ValueRef LLVMBuildNSWMul(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                             const char *Name);
LLVM37ValueRef LLVMBuildNUWMul(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                             const char *Name);
LLVM37ValueRef LLVMBuildFMul(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name);
LLVM37ValueRef LLVMBuildUDiv(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name);
LLVM37ValueRef LLVMBuildSDiv(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name);
LLVM37ValueRef LLVMBuildExactSDiv(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                                const char *Name);
LLVM37ValueRef LLVMBuildFDiv(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name);
LLVM37ValueRef LLVMBuildURem(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name);
LLVM37ValueRef LLVMBuildSRem(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name);
LLVM37ValueRef LLVMBuildFRem(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name);
LLVM37ValueRef LLVMBuildShl(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name);
LLVM37ValueRef LLVMBuildLShr(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name);
LLVM37ValueRef LLVMBuildAShr(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name);
LLVM37ValueRef LLVMBuildAnd(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name);
LLVM37ValueRef LLVMBuildOr(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name);
LLVM37ValueRef LLVMBuildXor(LLVMBuilderRef, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name);
LLVM37ValueRef LLVMBuildBinOp(LLVMBuilderRef B, LLVM37Opcode Op,
                            LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                            const char *Name);
LLVM37ValueRef LLVMBuildNeg(LLVMBuilderRef, LLVM37ValueRef V, const char *Name);
LLVM37ValueRef LLVMBuildNSWNeg(LLVMBuilderRef B, LLVM37ValueRef V,
                             const char *Name);
LLVM37ValueRef LLVMBuildNUWNeg(LLVMBuilderRef B, LLVM37ValueRef V,
                             const char *Name);
LLVM37ValueRef LLVMBuildFNeg(LLVMBuilderRef, LLVM37ValueRef V, const char *Name);
LLVM37ValueRef LLVMBuildNot(LLVMBuilderRef, LLVM37ValueRef V, const char *Name);

/* Memory */
LLVM37ValueRef LLVMBuildMalloc(LLVMBuilderRef, LLVM37TypeRef Ty, const char *Name);
LLVM37ValueRef LLVMBuildArrayMalloc(LLVMBuilderRef, LLVM37TypeRef Ty,
                                  LLVM37ValueRef Val, const char *Name);
LLVM37ValueRef LLVMBuildAlloca(LLVMBuilderRef, LLVM37TypeRef Ty, const char *Name);
LLVM37ValueRef LLVMBuildArrayAlloca(LLVMBuilderRef, LLVM37TypeRef Ty,
                                  LLVM37ValueRef Val, const char *Name);
LLVM37ValueRef LLVMBuildFree(LLVMBuilderRef, LLVM37ValueRef PointerVal);
LLVM37ValueRef LLVMBuildLoad(LLVMBuilderRef, LLVM37ValueRef PointerVal,
                           const char *Name);
LLVM37ValueRef LLVMBuildStore(LLVMBuilderRef, LLVM37ValueRef Val, LLVM37ValueRef Ptr);
LLVM37ValueRef LLVMBuildGEP(LLVMBuilderRef B, LLVM37ValueRef Pointer,
                          LLVM37ValueRef *Indices, unsigned NumIndices,
                          const char *Name);
LLVM37ValueRef LLVMBuildInBoundsGEP(LLVMBuilderRef B, LLVM37ValueRef Pointer,
                                  LLVM37ValueRef *Indices, unsigned NumIndices,
                                  const char *Name);
LLVM37ValueRef LLVMBuildStructGEP(LLVMBuilderRef B, LLVM37ValueRef Pointer,
                                unsigned Idx, const char *Name);
LLVM37ValueRef LLVMBuildGlobalString(LLVMBuilderRef B, const char *Str,
                                   const char *Name);
LLVM37ValueRef LLVMBuildGlobalStringPtr(LLVMBuilderRef B, const char *Str,
                                      const char *Name);
LLVM37Bool LLVM37GetVolatile(LLVM37ValueRef MemoryAccessInst);
void LLVM37SetVolatile(LLVM37ValueRef MemoryAccessInst, LLVM37Bool IsVolatile);

/* Casts */
LLVM37ValueRef LLVMBuildTrunc(LLVMBuilderRef, LLVM37ValueRef Val,
                            LLVM37TypeRef DestTy, const char *Name);
LLVM37ValueRef LLVMBuildZExt(LLVMBuilderRef, LLVM37ValueRef Val,
                           LLVM37TypeRef DestTy, const char *Name);
LLVM37ValueRef LLVMBuildSExt(LLVMBuilderRef, LLVM37ValueRef Val,
                           LLVM37TypeRef DestTy, const char *Name);
LLVM37ValueRef LLVMBuildFPToUI(LLVMBuilderRef, LLVM37ValueRef Val,
                             LLVM37TypeRef DestTy, const char *Name);
LLVM37ValueRef LLVMBuildFPToSI(LLVMBuilderRef, LLVM37ValueRef Val,
                             LLVM37TypeRef DestTy, const char *Name);
LLVM37ValueRef LLVMBuildUIToFP(LLVMBuilderRef, LLVM37ValueRef Val,
                             LLVM37TypeRef DestTy, const char *Name);
LLVM37ValueRef LLVMBuildSIToFP(LLVMBuilderRef, LLVM37ValueRef Val,
                             LLVM37TypeRef DestTy, const char *Name);
LLVM37ValueRef LLVMBuildFPTrunc(LLVMBuilderRef, LLVM37ValueRef Val,
                              LLVM37TypeRef DestTy, const char *Name);
LLVM37ValueRef LLVMBuildFPExt(LLVMBuilderRef, LLVM37ValueRef Val,
                            LLVM37TypeRef DestTy, const char *Name);
LLVM37ValueRef LLVMBuildPtrToInt(LLVMBuilderRef, LLVM37ValueRef Val,
                               LLVM37TypeRef DestTy, const char *Name);
LLVM37ValueRef LLVMBuildIntToPtr(LLVMBuilderRef, LLVM37ValueRef Val,
                               LLVM37TypeRef DestTy, const char *Name);
LLVM37ValueRef LLVMBuildBitCast(LLVMBuilderRef, LLVM37ValueRef Val,
                              LLVM37TypeRef DestTy, const char *Name);
LLVM37ValueRef LLVMBuildAddrSpaceCast(LLVMBuilderRef, LLVM37ValueRef Val,
                                    LLVM37TypeRef DestTy, const char *Name);
LLVM37ValueRef LLVMBuildZExtOrBitCast(LLVMBuilderRef, LLVM37ValueRef Val,
                                    LLVM37TypeRef DestTy, const char *Name);
LLVM37ValueRef LLVMBuildSExtOrBitCast(LLVMBuilderRef, LLVM37ValueRef Val,
                                    LLVM37TypeRef DestTy, const char *Name);
LLVM37ValueRef LLVMBuildTruncOrBitCast(LLVMBuilderRef, LLVM37ValueRef Val,
                                     LLVM37TypeRef DestTy, const char *Name);
LLVM37ValueRef LLVMBuildCast(LLVMBuilderRef B, LLVM37Opcode Op, LLVM37ValueRef Val,
                           LLVM37TypeRef DestTy, const char *Name);
LLVM37ValueRef LLVMBuildPointerCast(LLVMBuilderRef, LLVM37ValueRef Val,
                                  LLVM37TypeRef DestTy, const char *Name);
LLVM37ValueRef LLVMBuildIntCast(LLVMBuilderRef, LLVM37ValueRef Val, /*Signed cast!*/
                              LLVM37TypeRef DestTy, const char *Name);
LLVM37ValueRef LLVMBuildFPCast(LLVMBuilderRef, LLVM37ValueRef Val,
                             LLVM37TypeRef DestTy, const char *Name);

/* Comparisons */
LLVM37ValueRef LLVMBuildICmp(LLVMBuilderRef, LLVM37IntPredicate Op,
                           LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name);
LLVM37ValueRef LLVMBuildFCmp(LLVMBuilderRef, LLVM37RealPredicate Op,
                           LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name);

/* Miscellaneous instructions */
LLVM37ValueRef LLVMBuildPhi(LLVMBuilderRef, LLVM37TypeRef Ty, const char *Name);
LLVM37ValueRef LLVMBuildCall(LLVMBuilderRef, LLVM37ValueRef Fn,
                           LLVM37ValueRef *Args, unsigned NumArgs,
                           const char *Name);
LLVM37ValueRef LLVMBuildSelect(LLVMBuilderRef, LLVM37ValueRef If,
                             LLVM37ValueRef Then, LLVM37ValueRef Else,
                             const char *Name);
LLVM37ValueRef LLVMBuildVAArg(LLVMBuilderRef, LLVM37ValueRef List, LLVM37TypeRef Ty,
                            const char *Name);
LLVM37ValueRef LLVMBuildExtractElement(LLVMBuilderRef, LLVM37ValueRef VecVal,
                                     LLVM37ValueRef Index, const char *Name);
LLVM37ValueRef LLVMBuildInsertElement(LLVMBuilderRef, LLVM37ValueRef VecVal,
                                    LLVM37ValueRef EltVal, LLVM37ValueRef Index,
                                    const char *Name);
LLVM37ValueRef LLVMBuildShuffleVector(LLVMBuilderRef, LLVM37ValueRef V1,
                                    LLVM37ValueRef V2, LLVM37ValueRef Mask,
                                    const char *Name);
LLVM37ValueRef LLVMBuildExtractValue(LLVMBuilderRef, LLVM37ValueRef AggVal,
                                   unsigned Index, const char *Name);
LLVM37ValueRef LLVMBuildInsertValue(LLVMBuilderRef, LLVM37ValueRef AggVal,
                                  LLVM37ValueRef EltVal, unsigned Index,
                                  const char *Name);

LLVM37ValueRef LLVMBuildIsNull(LLVMBuilderRef, LLVM37ValueRef Val,
                             const char *Name);
LLVM37ValueRef LLVMBuildIsNotNull(LLVMBuilderRef, LLVM37ValueRef Val,
                                const char *Name);
LLVM37ValueRef LLVMBuildPtrDiff(LLVMBuilderRef, LLVM37ValueRef LHS,
                              LLVM37ValueRef RHS, const char *Name);
LLVM37ValueRef LLVMBuildFence(LLVMBuilderRef B, LLVM37AtomicOrdering ordering,
                            LLVM37Bool singleThread, const char *Name);
LLVM37ValueRef LLVMBuildAtomicRMW(LLVMBuilderRef B, LLVM37AtomicRMWBinOp op,
                                LLVM37ValueRef PTR, LLVM37ValueRef Val,
                                LLVM37AtomicOrdering ordering,
                                LLVM37Bool singleThread);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreModuleProvider Module Providers
 *
 * @{
 */

/**
 * Changes the type of M so it can be passed to FunctionPassManagers and the
 * JIT.  They take ModuleProviders for historical reasons.
 */
LLVM37ModuleProviderRef
LLVM37CreateModuleProviderForExistingModule(LLVM37ModuleRef M);

/**
 * Destroys the module M.
 */
void LLVM37DisposeModuleProvider(LLVM37ModuleProviderRef M);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreMemoryBuffers Memory Buffers
 *
 * @{
 */

LLVM37Bool LLVM37CreateMemoryBufferWithContentsOfFile(const char *Path,
                                                  LLVM37MemoryBufferRef *OutMemBuf,
                                                  _Out_opt_ char **OutMessage);
LLVM37Bool LLVM37CreateMemoryBufferWithSTDIN(LLVM37MemoryBufferRef *OutMemBuf,
    _Out_opt_ char **OutMessage);
LLVM37MemoryBufferRef LLVM37CreateMemoryBufferWithMemoryRange(const char *InputData,
                                                          size_t InputDataLength,
                                                          const char *BufferName,
                                                          LLVM37Bool RequiresNullTerminator);
LLVM37MemoryBufferRef LLVM37CreateMemoryBufferWithMemoryRangeCopy(const char *InputData,
                                                              size_t InputDataLength,
                                                              const char *BufferName);
const char *LLVM37GetBufferStart(LLVM37MemoryBufferRef MemBuf);
size_t LLVM37GetBufferSize(LLVM37MemoryBufferRef MemBuf);
void LLVM37DisposeMemoryBuffer(LLVM37MemoryBufferRef MemBuf);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCorePassRegistry Pass Registry
 *
 * @{
 */

/** Return the global pass registry, for use with initialization functions.
    @see llvm37::PassRegistry::getPassRegistry */
LLVM37PassRegistryRef LLVM37GetGlobalPassRegistry(void);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCorePassManagers Pass Managers
 *
 * @{
 */

/** Constructs a new whole-module pass pipeline. This type of pipeline is
    suitable for link-time optimization and whole-module transformations.
    @see llvm37::PassManager::PassManager */
LLVM37PassManagerRef LLVM37CreatePassManager(void);

/** Constructs a new function-by-function pass pipeline over the module
    provider. It does not take ownership of the module provider. This type of
    pipeline is suitable for code generation and JIT compilation tasks.
    @see llvm37::FunctionPassManager::FunctionPassManager */
LLVM37PassManagerRef LLVM37CreateFunctionPassManagerForModule(LLVM37ModuleRef M);

/** Deprecated: Use LLVM37CreateFunctionPassManagerForModule instead. */
LLVM37PassManagerRef LLVM37CreateFunctionPassManager(LLVM37ModuleProviderRef MP);

/** Initializes, executes on the provided module, and finalizes all of the
    passes scheduled in the pass manager. Returns 1 if any of the passes
    modified the module, 0 otherwise.
    @see llvm37::PassManager::run(Module&) */
LLVM37Bool LLVM37RunPassManager(LLVM37PassManagerRef PM, LLVM37ModuleRef M);

/** Initializes all of the function passes scheduled in the function pass
    manager. Returns 1 if any of the passes modified the module, 0 otherwise.
    @see llvm37::FunctionPassManager::doInitialization */
LLVM37Bool LLVM37InitializeFunctionPassManager(LLVM37PassManagerRef FPM);

/** Executes all of the function passes scheduled in the function pass manager
    on the provided function. Returns 1 if any of the passes modified the
    function, false otherwise.
    @see llvm37::FunctionPassManager::run(Function&) */
LLVM37Bool LLVM37RunFunctionPassManager(LLVM37PassManagerRef FPM, LLVM37ValueRef F);

/** Finalizes all of the function passes scheduled in in the function pass
    manager. Returns 1 if any of the passes modified the module, 0 otherwise.
    @see llvm37::FunctionPassManager::doFinalization */
LLVM37Bool LLVM37FinalizeFunctionPassManager(LLVM37PassManagerRef FPM);

/** Frees the memory of a pass pipeline. For function pipelines, does not free
    the module provider.
    @see llvm37::PassManagerBase::~PassManagerBase. */
void LLVM37DisposePassManager(LLVM37PassManagerRef PM);

/**
 * @}
 */

/**
 * @defgroup LLVM37CCoreThreading Threading
 *
 * Handle the structures needed to make LLVM37 safe for multithreading.
 *
 * @{
 */

/** Deprecated: Multi-threading can only be enabled/disabled with the compile
    time define LLVM37_ENABLE_THREADS.  This function always returns
    LLVM37IsMultithreaded(). */
LLVM37Bool LLVM37StartMultithreaded(void);

/** Deprecated: Multi-threading can only be enabled/disabled with the compile
    time define LLVM37_ENABLE_THREADS. */
void LLVM37StopMultithreaded(void);

/** Check whether LLVM37 is executing in thread-safe mode or not.
    @see llvm37::llvm37_is_multithreaded */
LLVM37Bool LLVM37IsMultithreaded(void);

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* !defined(__cplusplus) */

#endif /* !defined(LLVM37_C_CORE_H) */
