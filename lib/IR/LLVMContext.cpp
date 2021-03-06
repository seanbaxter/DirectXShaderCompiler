//===-- LLVM37Context.cpp - Implement LLVM37Context ---------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file implements LLVM37Context, as a wrapper around the opaque
//  class LLVM37ContextImpl.
//
//===----------------------------------------------------------------------===//

#include "llvm37/IR/LLVMContext.h"
#include "LLVMContextImpl.h"
#include "llvm37/IR/Constants.h"
#include "llvm37/IR/DebugLoc.h"
#include "llvm37/IR/DiagnosticInfo.h"
#include "llvm37/IR/DiagnosticPrinter.h"
#include "llvm37/IR/Instruction.h"
#include "llvm37/IR/Metadata.h"
#include "llvm37/Support/ManagedStatic.h"
#include "llvm37/Support/SourceMgr.h"
#include <cctype>
using namespace llvm37;

static ManagedStatic<LLVM37Context> GlobalContext;

LLVM37Context& llvm37::getGlobalContext() {
  return *GlobalContext;
}

LLVM37Context::LLVM37Context() : pImpl(new LLVM37ContextImpl(*this)) {
  std::unique_ptr<LLVM37ContextImpl> implPtrGuard(pImpl); // HLSL Change: Don't leak if constructor throws.
  // Create the fixed metadata kinds. This is done in the same order as the
  // MD_* enum values so that they correspond.

  // Create the 'dbg' metadata kind.
  unsigned DbgID = getMDKindID("dbg");
  assert(DbgID == MD_dbg && "dbg kind id drifted"); (void)DbgID;

  // Create the 'tbaa' metadata kind.
  unsigned TBAAID = getMDKindID("tbaa");
  assert(TBAAID == MD_tbaa && "tbaa kind id drifted"); (void)TBAAID;

  // Create the 'prof' metadata kind.
  unsigned ProfID = getMDKindID("prof");
  assert(ProfID == MD_prof && "prof kind id drifted"); (void)ProfID;

  // Create the 'fpmath' metadata kind.
  unsigned FPAccuracyID = getMDKindID("fpmath");
  assert(FPAccuracyID == MD_fpmath && "fpmath kind id drifted");
  (void)FPAccuracyID;

  // Create the 'range' metadata kind.
  unsigned RangeID = getMDKindID("range");
  assert(RangeID == MD_range && "range kind id drifted");
  (void)RangeID;

  // Create the 'tbaa.struct' metadata kind.
  unsigned TBAAStructID = getMDKindID("tbaa.struct");
  assert(TBAAStructID == MD_tbaa_struct && "tbaa.struct kind id drifted");
  (void)TBAAStructID;

  // Create the 'invariant.load' metadata kind.
  unsigned InvariantLdId = getMDKindID("invariant.load");
  assert(InvariantLdId == MD_invariant_load && "invariant.load kind id drifted");
  (void)InvariantLdId;

  // Create the 'alias.scope' metadata kind.
  unsigned AliasScopeID = getMDKindID("alias.scope");
  assert(AliasScopeID == MD_alias_scope && "alias.scope kind id drifted");
  (void)AliasScopeID;

  // Create the 'noalias' metadata kind.
  unsigned NoAliasID = getMDKindID("noalias");
  assert(NoAliasID == MD_noalias && "noalias kind id drifted");
  (void)NoAliasID;

  // Create the 'nontemporal' metadata kind.
  unsigned NonTemporalID = getMDKindID("nontemporal");
  assert(NonTemporalID == MD_nontemporal && "nontemporal kind id drifted");
  (void)NonTemporalID;

  // Create the 'llvm37.mem.parallel_loop_access' metadata kind.
  unsigned MemParallelLoopAccessID = getMDKindID("llvm.mem.parallel_loop_access");
  assert(MemParallelLoopAccessID == MD_mem_parallel_loop_access &&
         "mem_parallel_loop_access kind id drifted");
  (void)MemParallelLoopAccessID;

  // Create the 'nonnull' metadata kind.
  unsigned NonNullID = getMDKindID("nonnull");
  assert(NonNullID == MD_nonnull && "nonnull kind id drifted");
  (void)NonNullID;
  
  // Create the 'dereferenceable' metadata kind.
  unsigned DereferenceableID = getMDKindID("dereferenceable");
  assert(DereferenceableID == MD_dereferenceable && 
         "dereferenceable kind id drifted");
  (void)DereferenceableID;
  
  // Create the 'dereferenceable_or_null' metadata kind.
  unsigned DereferenceableOrNullID = getMDKindID("dereferenceable_or_null");
  assert(DereferenceableOrNullID == MD_dereferenceable_or_null && 
         "dereferenceable_or_null kind id drifted");
  (void)DereferenceableOrNullID;

  implPtrGuard.release(); // HLSL Change: Destructor now on the hook for destruction
}
LLVM37Context::~LLVM37Context() { delete pImpl; }

void LLVM37Context::addModule(Module *M) {
  pImpl->OwnedModules.insert(M);
}

void LLVM37Context::removeModule(Module *M) {
  pImpl->OwnedModules.erase(M);
}

//===----------------------------------------------------------------------===//
// Recoverable Backend Errors
//===----------------------------------------------------------------------===//

void LLVM37Context::
setInlineAsmDiagnosticHandler(InlineAsmDiagHandlerTy DiagHandler,
                              void *DiagContext) {
  pImpl->InlineAsmDiagHandler = DiagHandler;
  pImpl->InlineAsmDiagContext = DiagContext;
}

/// getInlineAsmDiagnosticHandler - Return the diagnostic handler set by
/// setInlineAsmDiagnosticHandler.
LLVM37Context::InlineAsmDiagHandlerTy
LLVM37Context::getInlineAsmDiagnosticHandler() const {
  return pImpl->InlineAsmDiagHandler;
}

/// getInlineAsmDiagnosticContext - Return the diagnostic context set by
/// setInlineAsmDiagnosticHandler.
void *LLVM37Context::getInlineAsmDiagnosticContext() const {
  return pImpl->InlineAsmDiagContext;
}

void LLVM37Context::setDiagnosticHandler(DiagnosticHandlerTy DiagnosticHandler,
                                       void *DiagnosticContext,
                                       bool RespectFilters) {
  pImpl->DiagnosticHandler = DiagnosticHandler;
  pImpl->DiagnosticContext = DiagnosticContext;
  pImpl->RespectDiagnosticFilters = RespectFilters;
}

LLVM37Context::DiagnosticHandlerTy LLVM37Context::getDiagnosticHandler() const {
  return pImpl->DiagnosticHandler;
}

void *LLVM37Context::getDiagnosticContext() const {
  return pImpl->DiagnosticContext;
}

void LLVM37Context::setYieldCallback(YieldCallbackTy Callback, void *OpaqueHandle)
{
  pImpl->YieldCallback = Callback;
  pImpl->YieldOpaqueHandle = OpaqueHandle;
}

void LLVM37Context::yield() {
  if (pImpl->YieldCallback)
    pImpl->YieldCallback(this, pImpl->YieldOpaqueHandle);
}

void LLVM37Context::emitError(const Twine &ErrorStr) {
  diagnose(DiagnosticInfoInlineAsm(ErrorStr));
}

// HLSL Change Start
void LLVM37Context::emitWarning(const Twine &WarningStr) {
  diagnose(DiagnosticInfoInlineAsm(WarningStr, DiagnosticSeverity::DS_Warning));
}
// HLSL Change End


void LLVM37Context::emitError(const Instruction *I, const Twine &ErrorStr) {
  assert (I && "Invalid instruction");
  diagnose(DiagnosticInfoInlineAsm(*I, ErrorStr));
}

static bool isDiagnosticEnabled(const DiagnosticInfo &DI) {
  // Optimization remarks are selective. They need to check whether the regexp
  // pattern, passed via one of the -pass-remarks* flags, matches the name of
  // the pass that is emitting the diagnostic. If there is no match, ignore the
  // diagnostic and return.
  switch (DI.getKind()) {
  case llvm37::DK_OptimizationRemark:
    if (!cast<DiagnosticInfoOptimizationRemark>(DI).isEnabled())
      return false;
    break;
  case llvm37::DK_OptimizationRemarkMissed:
    if (!cast<DiagnosticInfoOptimizationRemarkMissed>(DI).isEnabled())
      return false;
    break;
  case llvm37::DK_OptimizationRemarkAnalysis:
    if (!cast<DiagnosticInfoOptimizationRemarkAnalysis>(DI).isEnabled())
      return false;
    break;
  default:
    break;
  }
  return true;
}

static const char *getDiagnosticMessagePrefix(DiagnosticSeverity Severity) {
  switch (Severity) {
  case DS_Error:
    return "error";
  case DS_Warning:
    return "warning";
  case DS_Remark:
    return "remark";
  case DS_Note:
    return "note";
  }
  llvm37_unreachable("Unknown DiagnosticSeverity");
}

void LLVM37Context::diagnose(const DiagnosticInfo &DI) {
  // If there is a report handler, use it.
  /*
  if (pImpl->DiagnosticHandler) {
    if (!pImpl->RespectDiagnosticFilters || isDiagnosticEnabled(DI))
      pImpl->DiagnosticHandler(DI, pImpl->DiagnosticContext);
    return;
  }

  if (!isDiagnosticEnabled(DI))
    return;

  // Otherwise, print the message with a prefix based on the severity.
  DiagnosticPrinterRawOStream DP(errs());
  errs() << getDiagnosticMessagePrefix(DI.getSeverity()) << ": ";
  DI.print(DP);
  errs() << "\n";
*/  


    std::string string;
    raw_string_ostream os(string);
    DiagnosticPrinterRawOStream DP(os);
    DI.print(DP);
    os.flush();

    throw std::runtime_error(string);
/*
  if (DI.getSeverity() == DS_Error)
    // exit(1); // HLSL Change - unwind if necessary, but don't terminate the process
    throw std::exception();
  */
}

void LLVM37Context::emitError(unsigned LocCookie, const Twine &ErrorStr) {
  diagnose(DiagnosticInfoInlineAsm(LocCookie, ErrorStr));
}

//===----------------------------------------------------------------------===//
// Metadata Kind Uniquing
//===----------------------------------------------------------------------===//

// HLSL Change - Begin
/// Return a unique non-zero ID for the specified metadata kind if it exists.
bool LLVM37Context::findMDKindID(StringRef Name, unsigned *ID) const {
  auto it = pImpl->CustomMDKindNames.find(Name);
  if (it != pImpl->CustomMDKindNames.end()) {
    *ID = it->second;
    return true;
  }
  return false;
}
// HLSL Change - End

/// Return a unique non-zero ID for the specified metadata kind.
unsigned LLVM37Context::getMDKindID(StringRef Name) const {
  // If this is new, assign it its ID.
  return pImpl->CustomMDKindNames.insert(
                                     std::make_pair(
                                         Name, pImpl->CustomMDKindNames.size()))
      .first->second;
}

/// getHandlerNames - Populate client supplied smallvector using custome
/// metadata name and ID.
void LLVM37Context::getMDKindNames(SmallVectorImpl<StringRef> &Names) const {
  Names.resize(pImpl->CustomMDKindNames.size());
  for (StringMap<unsigned>::const_iterator I = pImpl->CustomMDKindNames.begin(),
       E = pImpl->CustomMDKindNames.end(); I != E; ++I)
    Names[I->second] = I->first();
}
