//===-- llvm37/LLVMContext.h - Class for managing "global" state --*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares LLVM37Context, a container of "global" state in LLVM37, such
// as the global type and constant uniquing tables.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_IR_LLVM37CONTEXT_H
#define LLVM37_IR_LLVM37CONTEXT_H

#include "llvm37-c/Core.h"
#include "llvm37/Support/CBindingWrapping.h"
#include "llvm37/Support/Compiler.h"
#include "llvm37/Support/Options.h"

namespace llvm37 {

class LLVM37ContextImpl;
class StringRef;
class Twine;
class Instruction;
class Module;
class SMDiagnostic;
class DiagnosticInfo;
template <typename T> class SmallVectorImpl;
class Function;
class DebugLoc;

/// This is an important class for using LLVM37 in a threaded context.  It
/// (opaquely) owns and manages the core "global" data of LLVM37's core
/// infrastructure, including the type and constant uniquing tables.
/// LLVM37Context itself provides no locking guarantees, so you should be careful
/// to have one context per thread.
class LLVM37Context {
public:
  LLVM37ContextImpl *const pImpl;
  LLVM37Context();
  ~LLVM37Context();

  // Pinned metadata names, which always have the same value.  This is a
  // compile-time performance optimization, not a correctness optimization.
  enum {
    MD_dbg = 0,  // "dbg"
    MD_tbaa = 1, // "tbaa"
    MD_prof = 2,  // "prof"
    MD_fpmath = 3,  // "fpmath"
    MD_range = 4, // "range"
    MD_tbaa_struct = 5, // "tbaa.struct"
    MD_invariant_load = 6, // "invariant.load"
    MD_alias_scope = 7, // "alias.scope"
    MD_noalias = 8, // "noalias",
    MD_nontemporal = 9, // "nontemporal"
    MD_mem_parallel_loop_access = 10, // "llvm.mem.parallel_loop_access"
    MD_nonnull = 11, // "nonnull"
    MD_dereferenceable = 12, // "dereferenceable"
    MD_dereferenceable_or_null = 13 // "dereferenceable_or_null"
  };

  /// getMDKindID - Return a unique non-zero ID for the specified metadata kind.
  /// This ID is uniqued across modules in the current LLVM37Context.
  unsigned getMDKindID(StringRef Name) const;

  // HLSL Change - Begin
  /// Return a unique non-zero ID for the specified metadata kind if it exists.
  bool findMDKindID(StringRef Name, unsigned *ID) const;
  // HLSL Change - End

  /// getMDKindNames - Populate client supplied SmallVector with the name for
  /// custom metadata IDs registered in this LLVM37Context.
  void getMDKindNames(SmallVectorImpl<StringRef> &Result) const;


  typedef void (*InlineAsmDiagHandlerTy)(const SMDiagnostic&, void *Context,
                                         unsigned LocCookie);

  /// Defines the type of a diagnostic handler.
  /// \see LLVM37Context::setDiagnosticHandler.
  /// \see LLVM37Context::diagnose.
  typedef void (*DiagnosticHandlerTy)(const DiagnosticInfo &DI, void *Context);

  /// Defines the type of a yield callback.
  /// \see LLVM37Context::setYieldCallback.
  typedef void (*YieldCallbackTy)(LLVM37Context *Context, void *OpaqueHandle);

  /// setInlineAsmDiagnosticHandler - This method sets a handler that is invoked
  /// when problems with inline asm are detected by the backend.  The first
  /// argument is a function pointer and the second is a context pointer that
  /// gets passed into the DiagHandler.
  ///
  /// LLVM37Context doesn't take ownership or interpret either of these
  /// pointers.
  void setInlineAsmDiagnosticHandler(InlineAsmDiagHandlerTy DiagHandler,
                                     void *DiagContext = nullptr);

  /// getInlineAsmDiagnosticHandler - Return the diagnostic handler set by
  /// setInlineAsmDiagnosticHandler.
  InlineAsmDiagHandlerTy getInlineAsmDiagnosticHandler() const;

  /// getInlineAsmDiagnosticContext - Return the diagnostic context set by
  /// setInlineAsmDiagnosticHandler.
  void *getInlineAsmDiagnosticContext() const;

  /// setDiagnosticHandler - This method sets a handler that is invoked
  /// when the backend needs to report anything to the user.  The first
  /// argument is a function pointer and the second is a context pointer that
  /// gets passed into the DiagHandler.  The third argument should be set to
  /// true if the handler only expects enabled diagnostics.
  ///
  /// LLVM37Context doesn't take ownership or interpret either of these
  /// pointers.
  void setDiagnosticHandler(DiagnosticHandlerTy DiagHandler,
                            void *DiagContext = nullptr,
                            bool RespectFilters = false);

  /// getDiagnosticHandler - Return the diagnostic handler set by
  /// setDiagnosticHandler.
  DiagnosticHandlerTy getDiagnosticHandler() const;

  /// getDiagnosticContext - Return the diagnostic context set by
  /// setDiagnosticContext.
  void *getDiagnosticContext() const;

  /// \brief Report a message to the currently installed diagnostic handler.
  ///
  /// This function returns, in particular in the case of error reporting
  /// (DI.Severity == \a DS_Error), so the caller should leave the compilation
  /// process in a self-consistent state, even though the generated code
  /// need not be correct.
  ///
  /// The diagnostic message will be implicitly prefixed with a severity keyword
  /// according to \p DI.getSeverity(), i.e., "error: " for \a DS_Error,
  /// "warning: " for \a DS_Warning, and "note: " for \a DS_Note.
  void diagnose(const DiagnosticInfo &DI);

  /// \brief Registers a yield callback with the given context.
  ///
  /// The yield callback function may be called by LLVM37 to transfer control back
  /// to the client that invoked the LLVM37 compilation. This can be used to yield
  /// control of the thread, or perform periodic work needed by the client.
  /// There is no guaranteed frequency at which callbacks must occur; in fact,
  /// the client is not guaranteed to ever receive this callback. It is at the
  /// sole discretion of LLVM37 to do so and only if it can guarantee that
  /// suspending the thread won't block any forward progress in other LLVM37
  /// contexts in the same process.
  ///
  /// At a suspend point, the state of the current LLVM37 context is intentionally
  /// undefined. No assumptions about it can or should be made. Only LLVM37
  /// context API calls that explicitly state that they can be used during a
  /// yield callback are allowed to be used. Any other API calls into the
  /// context are not supported until the yield callback function returns
  /// control to LLVM37. Other LLVM37 contexts are unaffected by this restriction.
  void setYieldCallback(YieldCallbackTy Callback, void *OpaqueHandle);

  /// \brief Calls the yield callback (if applicable).
  ///
  /// This transfers control of the current thread back to the client, which may
  /// suspend the current thread. Only call this method when LLVM37 doesn't hold
  /// any global mutex or cannot block the execution in another LLVM37 context.
  void yield();

  /// emitError - Emit an error message to the currently installed error handler
  /// with optional location information.  This function returns, so code should
  /// be prepared to drop the erroneous construct on the floor and "not crash".
  /// The generated code need not be correct.  The error message will be
  /// implicitly prefixed with "error: " and should not end with a ".".
  void emitError(unsigned LocCookie, const Twine &ErrorStr);
  void emitError(const Instruction *I, const Twine &ErrorStr);
  void emitError(const Twine &ErrorStr);
  void emitWarning(const Twine &WarningStr); // HLSL Change

  /// \brief Query for a debug option's value.
  ///
  /// This function returns typed data populated from command line parsing.
  template <typename ValT, typename Base, ValT(Base::*Mem)>
  ValT getOption() const {
    return OptionRegistry::instance().template get<ValT, Base, Mem>();
  }

private:
  LLVM37Context(LLVM37Context&) = delete;
  void operator=(LLVM37Context&) = delete;

  /// addModule - Register a module as being instantiated in this context.  If
  /// the context is deleted, the module will be deleted as well.
  void addModule(Module*);

  /// removeModule - Unregister a module from this context.
  void removeModule(Module*);

  // Module needs access to the add/removeModule methods.
  friend class Module;
};

/// getGlobalContext - Returns a global context.  This is for LLVM37 clients that
/// only care about operating on a single thread.
extern LLVM37Context &getGlobalContext();

// Create wrappers for C Binding types (see CBindingWrapping.h).
DEFINE_SIMPLE_CONVERSION_FUNCTIONS(LLVM37Context, LLVM37ContextRef)

/* Specialized opaque context conversions.
 */
inline LLVM37Context **unwrap(LLVM37ContextRef* Tys) {
  return reinterpret_cast<LLVM37Context**>(Tys);
}

inline LLVM37ContextRef *wrap(const LLVM37Context **Tys) {
  return reinterpret_cast<LLVM37ContextRef*>(const_cast<LLVM37Context**>(Tys));
}

}

#endif
