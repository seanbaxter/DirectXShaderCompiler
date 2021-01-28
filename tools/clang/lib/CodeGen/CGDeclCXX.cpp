//===--- CGDeclCXX.cpp - Emit LLVM37 Code for C++ declarations --------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This contains code dealing with code generation of C++ declarations
//
//===----------------------------------------------------------------------===//

#include "CodeGenFunction.h"
#include "CGCXXABI.h"
#include "CGObjCRuntime.h"
#include "CGOpenMPRuntime.h"
#include "clang/Frontend/CodeGenOptions.h"
#include "llvm37/ADT/StringExtras.h"
#include "llvm37/IR/Intrinsics.h"
#include "llvm37/Support/Path.h"

using namespace clang;
using namespace CodeGen;

static void EmitDeclInit(CodeGenFunction &CGF, const VarDecl &D,
                         llvm37::Constant *DeclPtr) {
  assert(D.hasGlobalStorage() && "VarDecl must have global storage!");
  assert(!D.getType()->isReferenceType() && 
         "Should not call EmitDeclInit on a reference!");
  
  ASTContext &Context = CGF.getContext();

  CharUnits alignment = Context.getDeclAlign(&D);
  QualType type = D.getType();
  LValue lv = CGF.MakeAddrLValue(DeclPtr, type, alignment);

  const Expr *Init = D.getInit();
  switch (CGF.getEvaluationKind(type)) {
  case TEK_Scalar: {
    CodeGenModule &CGM = CGF.CGM;
    if (lv.isObjCStrong())
      CGM.getObjCRuntime().EmitObjCGlobalAssign(CGF, CGF.EmitScalarExpr(Init),
                                                DeclPtr, D.getTLSKind());
    else if (lv.isObjCWeak())
      CGM.getObjCRuntime().EmitObjCWeakAssign(CGF, CGF.EmitScalarExpr(Init),
                                              DeclPtr);
    else
      CGF.EmitScalarInit(Init, &D, lv, false);
    return;
  }
  case TEK_Complex:
    CGF.EmitComplexExprIntoLValue(Init, lv, /*isInit*/ true);
    return;
  case TEK_Aggregate:
    CGF.EmitAggExpr(Init, AggValueSlot::forLValue(lv,AggValueSlot::IsDestructed,
                                          AggValueSlot::DoesNotNeedGCBarriers,
                                                  AggValueSlot::IsNotAliased));
    return;
  }
  llvm37_unreachable("bad evaluation kind");
}

/// Emit code to cause the destruction of the given variable with
/// static storage duration.
static void EmitDeclDestroy(CodeGenFunction &CGF, const VarDecl &D,
                            llvm37::Constant *addr) {
  CodeGenModule &CGM = CGF.CGM;

  // FIXME:  __attribute__((cleanup)) ?
  
  QualType type = D.getType();
  QualType::DestructionKind dtorKind = type.isDestructedType();

  switch (dtorKind) {
  case QualType::DK_none:
    return;

  case QualType::DK_cxx_destructor:
    break;

  case QualType::DK_objc_strong_lifetime:
  case QualType::DK_objc_weak_lifetime:
    // We don't care about releasing objects during process teardown.
    assert(!D.getTLSKind() && "should have rejected this");
    return;
  }

  llvm37::Constant *function;
  llvm37::Constant *argument;

  // Special-case non-array C++ destructors, where there's a function
  // with the right signature that we can just call.
  const CXXRecordDecl *record = nullptr;
  if (dtorKind == QualType::DK_cxx_destructor &&
      (record = type->getAsCXXRecordDecl())) {
    assert(!record->hasTrivialDestructor());
    CXXDestructorDecl *dtor = record->getDestructor();

    function = CGM.getAddrOfCXXStructor(dtor, StructorType::Complete);
    argument = llvm37::ConstantExpr::getBitCast(
        addr, CGF.getTypes().ConvertType(type)->getPointerTo());

  // Otherwise, the standard logic requires a helper function.
  } else {
    function = CodeGenFunction(CGM)
        .generateDestroyHelper(addr, type, CGF.getDestroyer(dtorKind),
                               CGF.needsEHCleanup(dtorKind), &D);
    argument = llvm37::Constant::getNullValue(CGF.Int8PtrTy);
  }

  CGM.getCXXABI().registerGlobalDtor(CGF, D, function, argument);
}

/// Emit code to cause the variable at the given address to be considered as
/// constant from this point onwards.
static void EmitDeclInvariant(CodeGenFunction &CGF, const VarDecl &D,
                              llvm37::Constant *Addr) {
  // Don't emit the intrinsic if we're not optimizing.
  if (!CGF.CGM.getCodeGenOpts().OptimizationLevel)
    return;
  // HLSL Change Begins.
  // Don't emit the intrinsic for hlsl.
  // Enable this will require SROA_HLSL to support the intrinsic.
  // Will do it later when support invariant marker in HLSL.
  if (CGF.CGM.getLangOpts().HLSL)
    return;
  // HLSL Change Ends.

  // Grab the llvm37.invariant.start intrinsic.
  llvm37::Intrinsic::ID InvStartID = llvm37::Intrinsic::invariant_start;
  llvm37::Constant *InvariantStart = CGF.CGM.getIntrinsic(InvStartID);

  // Emit a call with the size in bytes of the object.
  CharUnits WidthChars = CGF.getContext().getTypeSizeInChars(D.getType());
  uint64_t Width = WidthChars.getQuantity();
  llvm37::Value *Args[2] = { llvm37::ConstantInt::getSigned(CGF.Int64Ty, Width),
                           llvm37::ConstantExpr::getBitCast(Addr, CGF.Int8PtrTy)};
  CGF.Builder.CreateCall(InvariantStart, Args);
}

void CodeGenFunction::EmitCXXGlobalVarDeclInit(const VarDecl &D,
                                               llvm37::Constant *DeclPtr,
                                               bool PerformInit) {

  const Expr *Init = D.getInit();
  QualType T = D.getType();

  // The address space of a static local variable (DeclPtr) may be different
  // from the address space of the "this" argument of the constructor. In that
  // case, we need an addrspacecast before calling the constructor.
  //
  // struct StructWithCtor {
  //   __device__ StructWithCtor() {...}
  // };
  // __device__ void foo() {
  //   __shared__ StructWithCtor s;
  //   ...
  // }
  //
  // For example, in the above CUDA code, the static local variable s has a
  // "shared" address space qualifier, but the constructor of StructWithCtor
  // expects "this" in the "generic" address space.
  unsigned ExpectedAddrSpace = getContext().getTargetAddressSpace(T);
  unsigned ActualAddrSpace = DeclPtr->getType()->getPointerAddressSpace();
  if (ActualAddrSpace != ExpectedAddrSpace) {
    llvm37::Type *LTy = CGM.getTypes().ConvertTypeForMem(T);
    llvm37::PointerType *PTy = llvm37::PointerType::get(LTy, ExpectedAddrSpace);
    DeclPtr = llvm37::ConstantExpr::getAddrSpaceCast(DeclPtr, PTy);
  }

  if (!T->isReferenceType()) {
    if (getLangOpts().OpenMP && D.hasAttr<OMPThreadPrivateDeclAttr>())
      (void)CGM.getOpenMPRuntime().emitThreadPrivateVarDefinition(
          &D, DeclPtr, D.getAttr<OMPThreadPrivateDeclAttr>()->getLocation(),
          PerformInit, this);
    if (PerformInit)
      EmitDeclInit(*this, D, DeclPtr);
    if (CGM.isTypeConstant(D.getType(), true))
      EmitDeclInvariant(*this, D, DeclPtr);
    else
      EmitDeclDestroy(*this, D, DeclPtr);
    return;
  }

  assert(PerformInit && "cannot have constant initializer which needs "
         "destruction for reference");
  unsigned Alignment = getContext().getDeclAlign(&D).getQuantity();
  RValue RV = EmitReferenceBindingToExpr(Init);
  EmitStoreOfScalar(RV.getScalarVal(), DeclPtr, false, Alignment, T);
}

/// Create a stub function, suitable for being passed to atexit,
/// which passes the given address to the given destructor function.
llvm37::Constant *CodeGenFunction::createAtExitStub(const VarDecl &VD,
                                                  llvm37::Constant *dtor,
                                                  llvm37::Constant *addr) {
  // Get the destructor function type, void(*)(void).
  llvm37::FunctionType *ty = llvm37::FunctionType::get(CGM.VoidTy, false);
  SmallString<256> FnName;
  {
    llvm37::raw_svector_ostream Out(FnName);
    CGM.getCXXABI().getMangleContext().mangleDynamicAtExitDestructor(&VD, Out);
  }
  llvm37::Function *fn = CGM.CreateGlobalInitOrDestructFunction(ty, FnName.str(),
                                                              VD.getLocation());

  CodeGenFunction CGF(CGM);

  CGF.StartFunction(&VD, CGM.getContext().VoidTy, fn,
                    CGM.getTypes().arrangeNullaryFunction(), FunctionArgList());

  llvm37::CallInst *call = CGF.Builder.CreateCall(dtor, addr);
 
 // Make sure the call and the callee agree on calling convention.
  if (llvm37::Function *dtorFn =
        dyn_cast<llvm37::Function>(dtor->stripPointerCasts()))
    call->setCallingConv(dtorFn->getCallingConv());

  CGF.FinishFunction();

  return fn;
}

/// Register a global destructor using the C atexit runtime function.
void CodeGenFunction::registerGlobalDtorWithAtExit(const VarDecl &VD,
                                                   llvm37::Constant *dtor,
                                                   llvm37::Constant *addr) {
  // Create a function which calls the destructor.
  llvm37::Constant *dtorStub = createAtExitStub(VD, dtor, addr);

  // extern "C" int atexit(void (*f)(void));
  llvm37::FunctionType *atexitTy =
    llvm37::FunctionType::get(IntTy, dtorStub->getType(), false);

  llvm37::Constant *atexit =
    CGM.CreateRuntimeFunction(atexitTy, "atexit");
  if (llvm37::Function *atexitFn = dyn_cast<llvm37::Function>(atexit))
    atexitFn->setDoesNotThrow();

  EmitNounwindRuntimeCall(atexit, dtorStub);
}

void CodeGenFunction::EmitCXXGuardedInit(const VarDecl &D,
                                         llvm37::GlobalVariable *DeclPtr,
                                         bool PerformInit) {
  // If we've been asked to forbid guard variables, emit an error now.
  // This diagnostic is hard-coded for Darwin's use case;  we can find
  // better phrasing if someone else needs it.
  if (CGM.getCodeGenOpts().ForbidGuardVariables)
    CGM.Error(D.getLocation(),
              "this initialization requires a guard variable, which "
              "the kernel does not support");

  CGM.getCXXABI().EmitGuardedInit(*this, D, DeclPtr, PerformInit);
}

llvm37::Function *CodeGenModule::CreateGlobalInitOrDestructFunction(
    llvm37::FunctionType *FTy, const Twine &Name, SourceLocation Loc, bool TLS) {
  llvm37::Function *Fn =
    llvm37::Function::Create(FTy, llvm37::GlobalValue::InternalLinkage,
                           Name, &getModule());
  if (!getLangOpts().AppleKext && !TLS) {
    // Set the section if needed.
    if (const char *Section = getTarget().getStaticInitSectionSpecifier())
      Fn->setSection(Section);
  }

  SetLLVM37FunctionAttributes(nullptr, getTypes().arrangeNullaryFunction(), Fn);

  Fn->setCallingConv(getRuntimeCC());

  if (!getLangOpts().Exceptions)
    Fn->setDoesNotThrow();

  if (!isInSanitizerBlacklist(Fn, Loc)) {
    if (getLangOpts().Sanitize.hasOneOf(SanitizerKind::Address |
                                        SanitizerKind::KernelAddress))
      Fn->addFnAttr(llvm37::Attribute::SanitizeAddress);
    if (getLangOpts().Sanitize.has(SanitizerKind::Thread))
      Fn->addFnAttr(llvm37::Attribute::SanitizeThread);
    if (getLangOpts().Sanitize.has(SanitizerKind::Memory))
      Fn->addFnAttr(llvm37::Attribute::SanitizeMemory);
    if (getLangOpts().Sanitize.has(SanitizerKind::SafeStack))
      Fn->addFnAttr(llvm37::Attribute::SafeStack);
  }

  return Fn;
}

/// Create a global pointer to a function that will initialize a global
/// variable.  The user has requested that this pointer be emitted in a specific
/// section.
void CodeGenModule::EmitPointerToInitFunc(const VarDecl *D,
                                          llvm37::GlobalVariable *GV,
                                          llvm37::Function *InitFunc,
                                          InitSegAttr *ISA) {
  llvm37::GlobalVariable *PtrArray = new llvm37::GlobalVariable(
      TheModule, InitFunc->getType(), /*isConstant=*/true,
      llvm37::GlobalValue::PrivateLinkage, InitFunc, "__cxx_init_fn_ptr");
  PtrArray->setSection(ISA->getSection());
  addUsedGlobal(PtrArray);

  // If the GV is already in a comdat group, then we have to join it.
  if (llvm37::Comdat *C = GV->getComdat())
    PtrArray->setComdat(C);
}

void
CodeGenModule::EmitCXXGlobalVarDeclInitFunc(const VarDecl *D,
                                            llvm37::GlobalVariable *Addr,
                                            bool PerformInit) {
  // Check if we've already initialized this decl.
  auto I = DelayedCXXInitPosition.find(D);
  if (I != DelayedCXXInitPosition.end() && I->second == ~0U)
    return;

  llvm37::FunctionType *FTy = llvm37::FunctionType::get(VoidTy, false);
  SmallString<256> FnName;
  {
    llvm37::raw_svector_ostream Out(FnName);
    getCXXABI().getMangleContext().mangleDynamicInitializer(D, Out);
  }

  // Create a variable initialization function.
  llvm37::Function *Fn =
      CreateGlobalInitOrDestructFunction(FTy, FnName.str(), D->getLocation());

  auto *ISA = D->getAttr<InitSegAttr>();
  CodeGenFunction(*this).GenerateCXXGlobalVarDeclInitFunc(Fn, D, Addr,
                                                          PerformInit);

  llvm37::GlobalVariable *COMDATKey =
      supportsCOMDAT() && D->isExternallyVisible() ? Addr : nullptr;

  if (D->getTLSKind()) {
    // FIXME: Should we support init_priority for thread_local?
    // FIXME: Ideally, initialization of instantiated thread_local static data
    // members of class templates should not trigger initialization of other
    // entities in the TU.
    // FIXME: We only need to register one __cxa_thread_atexit function for the
    // entire TU.
    CXXThreadLocalInits.push_back(Fn);
    CXXThreadLocalInitVars.push_back(Addr);
  } else if (PerformInit && ISA) {
    EmitPointerToInitFunc(D, Addr, Fn, ISA);
  } else if (auto *IPA = D->getAttr<InitPriorityAttr>()) {
    OrderGlobalInits Key(IPA->getPriority(), PrioritizedCXXGlobalInits.size());
    PrioritizedCXXGlobalInits.push_back(std::make_pair(Key, Fn));
  } else if (isTemplateInstantiation(D->getTemplateSpecializationKind())) {
    // C++ [basic.start.init]p2:
    //   Definitions of explicitly specialized class template static data
    //   members have ordered initialization. Other class template static data
    //   members (i.e., implicitly or explicitly instantiated specializations)
    //   have unordered initialization.
    //
    // As a consequence, we can put them into their own llvm37.global_ctors entry.
    //
    // If the global is externally visible, put the initializer into a COMDAT
    // group with the global being initialized.  On most platforms, this is a
    // minor startup time optimization.  In the MS C++ ABI, there are no guard
    // variables, so this COMDAT key is required for correctness.
    AddGlobalCtor(Fn, 65535, COMDATKey);
  } else if (D->hasAttr<SelectAnyAttr>()) {
    // SelectAny globals will be comdat-folded. Put the initializer into a
    // COMDAT group associated with the global, so the initializers get folded
    // too.
    AddGlobalCtor(Fn, 65535, COMDATKey);
  } else {
    I = DelayedCXXInitPosition.find(D); // Re-do lookup in case of re-hash.
    if (I == DelayedCXXInitPosition.end()) {
      CXXGlobalInits.push_back(Fn);
    } else if (I->second != ~0U) {
      assert(I->second < CXXGlobalInits.size() &&
             CXXGlobalInits[I->second] == nullptr);
      CXXGlobalInits[I->second] = Fn;
    }
  }

  // Remember that we already emitted the initializer for this global.
  DelayedCXXInitPosition[D] = ~0U;
}

void CodeGenModule::EmitCXXThreadLocalInitFunc() {
  getCXXABI().EmitThreadLocalInitFuncs(
      *this, CXXThreadLocals, CXXThreadLocalInits, CXXThreadLocalInitVars);

  CXXThreadLocalInits.clear();
  CXXThreadLocalInitVars.clear();
  CXXThreadLocals.clear();
}

void
CodeGenModule::EmitCXXGlobalInitFunc() {
  while (!CXXGlobalInits.empty() && !CXXGlobalInits.back())
    CXXGlobalInits.pop_back();

  if (CXXGlobalInits.empty() && PrioritizedCXXGlobalInits.empty())
    return;

  llvm37::FunctionType *FTy = llvm37::FunctionType::get(VoidTy, false);


  // Create our global initialization function.
  if (!PrioritizedCXXGlobalInits.empty()) {
    SmallVector<llvm37::Function *, 8> LocalCXXGlobalInits;
    llvm37::array_pod_sort(PrioritizedCXXGlobalInits.begin(), 
                         PrioritizedCXXGlobalInits.end());
    // Iterate over "chunks" of ctors with same priority and emit each chunk
    // into separate function. Note - everything is sorted first by priority,
    // second - by lex order, so we emit ctor functions in proper order.
    for (SmallVectorImpl<GlobalInitData >::iterator
           I = PrioritizedCXXGlobalInits.begin(),
           E = PrioritizedCXXGlobalInits.end(); I != E; ) {
      SmallVectorImpl<GlobalInitData >::iterator
        PrioE = std::upper_bound(I + 1, E, *I, GlobalInitPriorityCmp());

      LocalCXXGlobalInits.clear();
      unsigned Priority = I->first.priority;
      // Compute the function suffix from priority. Prepend with zeroes to make
      // sure the function names are also ordered as priorities.
      std::string PrioritySuffix = llvm37::utostr(Priority);
      // Priority is always <= 65535 (enforced by sema).
      PrioritySuffix = std::string(6-PrioritySuffix.size(), '0')+PrioritySuffix;
      llvm37::Function *Fn = CreateGlobalInitOrDestructFunction(
          FTy, "_GLOBAL__I_" + PrioritySuffix);

      for (; I < PrioE; ++I)
        LocalCXXGlobalInits.push_back(I->second);

      CodeGenFunction(*this).GenerateCXXGlobalInitFunc(Fn, LocalCXXGlobalInits);
      AddGlobalCtor(Fn, Priority);
    }
    PrioritizedCXXGlobalInits.clear();
  }

  SmallString<128> FileName;
  SourceManager &SM = Context.getSourceManager();
  if (const FileEntry *MainFile = SM.getFileEntryForID(SM.getMainFileID())) {
    // Include the filename in the symbol name. Including "sub_" matches gcc and
    // makes sure these symbols appear lexicographically behind the symbols with
    // priority emitted above.
    FileName = llvm37::sys::path::filename(MainFile->getName());
  } else {
    FileName = "<null>";
  }

  for (size_t i = 0; i < FileName.size(); ++i) {
    // Replace everything that's not [a-zA-Z0-9._] with a _. This set happens
    // to be the set of C preprocessing numbers.
    if (!isPreprocessingNumberBody(FileName[i]))
      FileName[i] = '_';
  }

  llvm37::Function *Fn = CreateGlobalInitOrDestructFunction(
      FTy, llvm37::Twine("_GLOBAL__sub_I_", FileName));

  CodeGenFunction(*this).GenerateCXXGlobalInitFunc(Fn, CXXGlobalInits);
  AddGlobalCtor(Fn);

  CXXGlobalInits.clear();
}

void CodeGenModule::EmitCXXGlobalDtorFunc() {
  if (CXXGlobalDtors.empty())
    return;

  llvm37::FunctionType *FTy = llvm37::FunctionType::get(VoidTy, false);

  // Create our global destructor function.
  llvm37::Function *Fn = CreateGlobalInitOrDestructFunction(FTy, "_GLOBAL__D_a");

  CodeGenFunction(*this).GenerateCXXGlobalDtorsFunc(Fn, CXXGlobalDtors);
  AddGlobalDtor(Fn);
}

/// Emit the code necessary to initialize the given global variable.
void CodeGenFunction::GenerateCXXGlobalVarDeclInitFunc(llvm37::Function *Fn,
                                                       const VarDecl *D,
                                                 llvm37::GlobalVariable *Addr,
                                                       bool PerformInit) {
  // Check if we need to emit debug info for variable initializer.
  if (D->hasAttr<NoDebugAttr>())
    DebugInfo = nullptr; // disable debug info indefinitely for this function

  CurEHLocation = D->getLocStart();

  StartFunction(GlobalDecl(D), getContext().VoidTy, Fn,
                getTypes().arrangeNullaryFunction(),
                FunctionArgList(), D->getLocation(),
                D->getInit()->getExprLoc());

  // Use guarded initialization if the global variable is weak. This
  // occurs for, e.g., instantiated static data members and
  // definitions explicitly marked weak.
  if (Addr->hasWeakLinkage() || Addr->hasLinkOnceLinkage()) {
    EmitCXXGuardedInit(*D, Addr, PerformInit);
  } else {
    EmitCXXGlobalVarDeclInit(*D, Addr, PerformInit);
  }

  FinishFunction();
}

void
CodeGenFunction::GenerateCXXGlobalInitFunc(llvm37::Function *Fn,
                                           ArrayRef<llvm37::Function *> Decls,
                                           llvm37::GlobalVariable *Guard) {
  {
    auto NL = ApplyDebugLocation::CreateEmpty(*this);
    StartFunction(GlobalDecl(), getContext().VoidTy, Fn,
                  getTypes().arrangeNullaryFunction(), FunctionArgList());
    // Emit an artificial location for this function.
    auto AL = ApplyDebugLocation::CreateArtificial(*this);

    llvm37::BasicBlock *ExitBlock = nullptr;
    if (Guard) {
      // If we have a guard variable, check whether we've already performed
      // these initializations. This happens for TLS initialization functions.
      llvm37::Value *GuardVal = Builder.CreateLoad(Guard);
      llvm37::Value *Uninit = Builder.CreateIsNull(GuardVal,
                                                 "guard.uninitialized");
      // Mark as initialized before initializing anything else. If the
      // initializers use previously-initialized thread_local vars, that's
      // probably supposed to be OK, but the standard doesn't say.
      Builder.CreateStore(llvm37::ConstantInt::get(GuardVal->getType(),1), Guard);
      llvm37::BasicBlock *InitBlock = createBasicBlock("init");
      ExitBlock = createBasicBlock("exit");
      Builder.CreateCondBr(Uninit, InitBlock, ExitBlock);
      EmitBlock(InitBlock);
    }

    RunCleanupsScope Scope(*this);

    // When building in Objective-C++ ARC mode, create an autorelease pool
    // around the global initializers.
#if 0 // HLSL Change - no ObjC support
    if (getLangOpts().ObjCAutoRefCount && getLangOpts().CPlusPlus) {
      llvm37::Value *token = EmitObjCAutoreleasePoolPush();
      EmitObjCAutoreleasePoolCleanup(token);
    }
#endif // HLSL Change - no ObjC support

    for (unsigned i = 0, e = Decls.size(); i != e; ++i)
      if (Decls[i])
        EmitRuntimeCall(Decls[i]);

    Scope.ForceCleanup();

    if (ExitBlock) {
      Builder.CreateBr(ExitBlock);
      EmitBlock(ExitBlock);
    }
  }

  FinishFunction();
}

void CodeGenFunction::GenerateCXXGlobalDtorsFunc(llvm37::Function *Fn,
                  const std::vector<std::pair<llvm37::WeakVH, llvm37::Constant*> >
                                                &DtorsAndObjects) {
  {
    auto NL = ApplyDebugLocation::CreateEmpty(*this);
    StartFunction(GlobalDecl(), getContext().VoidTy, Fn,
                  getTypes().arrangeNullaryFunction(), FunctionArgList());
    // Emit an artificial location for this function.
    auto AL = ApplyDebugLocation::CreateArtificial(*this);

    // Emit the dtors, in reverse order from construction.
    for (unsigned i = 0, e = DtorsAndObjects.size(); i != e; ++i) {
      llvm37::Value *Callee = DtorsAndObjects[e - i - 1].first;
      llvm37::CallInst *CI = Builder.CreateCall(Callee,
                                          DtorsAndObjects[e - i - 1].second);
      // Make sure the call and the callee agree on calling convention.
      if (llvm37::Function *F = dyn_cast<llvm37::Function>(Callee))
        CI->setCallingConv(F->getCallingConv());
    }
  }

  FinishFunction();
}

/// generateDestroyHelper - Generates a helper function which, when
/// invoked, destroys the given object.
llvm37::Function *CodeGenFunction::generateDestroyHelper(
    llvm37::Constant *addr, QualType type, Destroyer *destroyer,
    bool useEHCleanupForArray, const VarDecl *VD) {
  FunctionArgList args;
  ImplicitParamDecl dst(getContext(), nullptr, SourceLocation(), nullptr,
                        getContext().VoidPtrTy);
  args.push_back(&dst);

  const CGFunctionInfo &FI = CGM.getTypes().arrangeFreeFunctionDeclaration(
      getContext().VoidTy, args, FunctionType::ExtInfo(), /*variadic=*/false);
  llvm37::FunctionType *FTy = CGM.getTypes().GetFunctionType(FI);
  llvm37::Function *fn = CGM.CreateGlobalInitOrDestructFunction(
      FTy, "__cxx_global_array_dtor", VD->getLocation());

  CurEHLocation = VD->getLocStart();

  StartFunction(VD, getContext().VoidTy, fn, FI, args);

  emitDestroy(addr, type, destroyer, useEHCleanupForArray);
  
  FinishFunction();
  
  return fn;
}
