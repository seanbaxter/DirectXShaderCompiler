//===------- ItaniumCXXABI.cpp - Emit LLVM Code from ASTs for a Module ----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This provides C++ code generation targeting the Itanium C++ ABI.  The class
// in this file generates structures that follow the Itanium C++ ABI, which is
// documented at:
//  http://www.codesourcery.com/public/cxx-abi/abi.html
//  http://www.codesourcery.com/public/cxx-abi/abi-eh.html
//
// It also supports the closely-related ARM ABI, documented at:
// http://infocenter.arm.com/help/topic/com.arm.doc.ihi0041c/IHI0041C_cppabi.pdf
//
//===----------------------------------------------------------------------===//

#include "CGCXXABI.h"
#include "CGCleanup.h"
#include "CGRecordLayout.h"
#include "CGVTables.h"
#include "CodeGenFunction.h"
#include "CodeGenModule.h"
#include "TargetInfo.h"
#include "clang/AST/Mangle.h"
#include "clang/AST/Type.h"
#include "clang/AST/StmtCXX.h"
#include "llvm37/IR/CallSite.h"
#include "llvm37/IR/DataLayout.h"
#include "llvm37/IR/Instructions.h"
#include "llvm37/IR/Intrinsics.h"
#include "llvm37/IR/Value.h"

using namespace clang;
using namespace CodeGen;

namespace {
class ItaniumCXXABI : public CodeGen::CGCXXABI {
  /// VTables - All the vtables which have been defined.
  llvm37::DenseMap<const CXXRecordDecl *, llvm37::GlobalVariable *> VTables;

protected:
  bool UseARMMethodPtrABI;
  bool UseARMGuardVarABI;

  ItaniumMangleContext &getMangleContext() {
    return cast<ItaniumMangleContext>(CodeGen::CGCXXABI::getMangleContext());
  }

public:
  ItaniumCXXABI(CodeGen::CodeGenModule &CGM,
                bool UseARMMethodPtrABI = false,
                bool UseARMGuardVarABI = false) :
    CGCXXABI(CGM), UseARMMethodPtrABI(UseARMMethodPtrABI),
    UseARMGuardVarABI(UseARMGuardVarABI) { }

  bool classifyReturnType(CGFunctionInfo &FI) const override;

  RecordArgABI getRecordArgABI(const CXXRecordDecl *RD) const override {
    // Structures with either a non-trivial destructor or a non-trivial
    // copy constructor are always indirect.
    // FIXME: Use canCopyArgument() when it is fixed to handle lazily declared
    // special members.
    if (RD->hasNonTrivialDestructor() || RD->hasNonTrivialCopyConstructor())
      return RAA_Indirect;
    return RAA_Default;
  }

  bool isZeroInitializable(const MemberPointerType *MPT) override;

  llvm37::Type *ConvertMemberPointerType(const MemberPointerType *MPT) override;

  llvm37::Value *
    EmitLoadOfMemberFunctionPointer(CodeGenFunction &CGF,
                                    const Expr *E,
                                    llvm37::Value *&This,
                                    llvm37::Value *MemFnPtr,
                                    const MemberPointerType *MPT) override;

  llvm37::Value *
    EmitMemberDataPointerAddress(CodeGenFunction &CGF, const Expr *E,
                                 llvm37::Value *Base,
                                 llvm37::Value *MemPtr,
                                 const MemberPointerType *MPT) override;

  llvm37::Value *EmitMemberPointerConversion(CodeGenFunction &CGF,
                                           const CastExpr *E,
                                           llvm37::Value *Src) override;
  llvm37::Constant *EmitMemberPointerConversion(const CastExpr *E,
                                              llvm37::Constant *Src) override;

  llvm37::Constant *EmitNullMemberPointer(const MemberPointerType *MPT) override;

  llvm37::Constant *EmitMemberFunctionPointer(const CXXMethodDecl *MD) override;
  llvm37::Constant *EmitMemberDataPointer(const MemberPointerType *MPT,
                                        CharUnits offset) override;
  llvm37::Constant *EmitMemberPointer(const APValue &MP, QualType MPT) override;
  llvm37::Constant *BuildMemberPointer(const CXXMethodDecl *MD,
                                     CharUnits ThisAdjustment);

  llvm37::Value *EmitMemberPointerComparison(CodeGenFunction &CGF,
                                           llvm37::Value *L, llvm37::Value *R,
                                           const MemberPointerType *MPT,
                                           bool Inequality) override;

  llvm37::Value *EmitMemberPointerIsNotNull(CodeGenFunction &CGF,
                                         llvm37::Value *Addr,
                                         const MemberPointerType *MPT) override;

  void emitVirtualObjectDelete(CodeGenFunction &CGF, const CXXDeleteExpr *DE,
                               llvm37::Value *Ptr, QualType ElementType,
                               const CXXDestructorDecl *Dtor) override;

  void emitRethrow(CodeGenFunction &CGF, bool isNoReturn) override;
  void emitThrow(CodeGenFunction &CGF, const CXXThrowExpr *E) override;

  void emitBeginCatch(CodeGenFunction &CGF, const CXXCatchStmt *C) override;

  llvm37::CallInst *
  emitTerminateForUnexpectedException(CodeGenFunction &CGF,
                                      llvm37::Value *Exn) override;

  void EmitFundamentalRTTIDescriptor(QualType Type);
  void EmitFundamentalRTTIDescriptors();
  llvm37::Constant *getAddrOfRTTIDescriptor(QualType Ty) override;
  llvm37::Constant *
  getAddrOfCXXCatchHandlerType(QualType Ty,
                               QualType CatchHandlerType) override {
    return getAddrOfRTTIDescriptor(Ty);
  }

  bool shouldTypeidBeNullChecked(bool IsDeref, QualType SrcRecordTy) override;
  void EmitBadTypeidCall(CodeGenFunction &CGF) override;
  llvm37::Value *EmitTypeid(CodeGenFunction &CGF, QualType SrcRecordTy,
                          llvm37::Value *ThisPtr,
                          llvm37::Type *StdTypeInfoPtrTy) override;

  bool shouldDynamicCastCallBeNullChecked(bool SrcIsPtr,
                                          QualType SrcRecordTy) override;

  llvm37::Value *EmitDynamicCastCall(CodeGenFunction &CGF, llvm37::Value *Value,
                                   QualType SrcRecordTy, QualType DestTy,
                                   QualType DestRecordTy,
                                   llvm37::BasicBlock *CastEnd) override;

  llvm37::Value *EmitDynamicCastToVoid(CodeGenFunction &CGF, llvm37::Value *Value,
                                     QualType SrcRecordTy,
                                     QualType DestTy) override;

  bool EmitBadCastCall(CodeGenFunction &CGF) override;

  llvm37::Value *
    GetVirtualBaseClassOffset(CodeGenFunction &CGF, llvm37::Value *This,
                              const CXXRecordDecl *ClassDecl,
                              const CXXRecordDecl *BaseClassDecl) override;

  void EmitCXXConstructors(const CXXConstructorDecl *D) override;

  void buildStructorSignature(const CXXMethodDecl *MD, StructorType T,
                              SmallVectorImpl<CanQualType> &ArgTys) override;

  bool useThunkForDtorVariant(const CXXDestructorDecl *Dtor,
                              CXXDtorType DT) const override {
    // Itanium does not emit any destructor variant as an inline thunk.
    // Delegating may occur as an optimization, but all variants are either
    // emitted with external linkage or as linkonce if they are inline and used.
    return false;
  }

  void EmitCXXDestructors(const CXXDestructorDecl *D) override;

  void addImplicitStructorParams(CodeGenFunction &CGF, QualType &ResTy,
                                 FunctionArgList &Params) override;

  void EmitInstanceFunctionProlog(CodeGenFunction &CGF) override;

  unsigned addImplicitConstructorArgs(CodeGenFunction &CGF,
                                      const CXXConstructorDecl *D,
                                      CXXCtorType Type, bool ForVirtualBase,
                                      bool Delegating,
                                      CallArgList &Args) override;

  void EmitDestructorCall(CodeGenFunction &CGF, const CXXDestructorDecl *DD,
                          CXXDtorType Type, bool ForVirtualBase,
                          bool Delegating, llvm37::Value *This) override;

  void emitVTableDefinitions(CodeGenVTables &CGVT,
                             const CXXRecordDecl *RD) override;

  llvm37::Value *getVTableAddressPointInStructor(
      CodeGenFunction &CGF, const CXXRecordDecl *VTableClass,
      BaseSubobject Base, const CXXRecordDecl *NearestVBase,
      bool &NeedsVirtualOffset) override;

  llvm37::Constant *
  getVTableAddressPointForConstExpr(BaseSubobject Base,
                                    const CXXRecordDecl *VTableClass) override;

  llvm37::GlobalVariable *getAddrOfVTable(const CXXRecordDecl *RD,
                                        CharUnits VPtrOffset) override;

  llvm37::Value *getVirtualFunctionPointer(CodeGenFunction &CGF, GlobalDecl GD,
                                         llvm37::Value *This,
                                         llvm37::Type *Ty,
                                         SourceLocation Loc) override;

  llvm37::Value *EmitVirtualDestructorCall(CodeGenFunction &CGF,
                                         const CXXDestructorDecl *Dtor,
                                         CXXDtorType DtorType,
                                         llvm37::Value *This,
                                         const CXXMemberCallExpr *CE) override;

  void emitVirtualInheritanceTables(const CXXRecordDecl *RD) override;

  void setThunkLinkage(llvm37::Function *Thunk, bool ForVTable, GlobalDecl GD,
                       bool ReturnAdjustment) override {
    // Allow inlining of thunks by emitting them with available_externally
    // linkage together with vtables when needed.
    if (ForVTable && !Thunk->hasLocalLinkage())
      Thunk->setLinkage(llvm37::GlobalValue::AvailableExternallyLinkage);
  }

  llvm37::Value *performThisAdjustment(CodeGenFunction &CGF, llvm37::Value *This,
                                     const ThisAdjustment &TA) override;

  llvm37::Value *performReturnAdjustment(CodeGenFunction &CGF, llvm37::Value *Ret,
                                       const ReturnAdjustment &RA) override;

  size_t getSrcArgforCopyCtor(const CXXConstructorDecl *,
                              FunctionArgList &Args) const override {
    assert(!Args.empty() && "expected the arglist to not be empty!");
    return Args.size() - 1;
  }

  StringRef GetPureVirtualCallName() override { return "__cxa_pure_virtual"; }
  StringRef GetDeletedVirtualCallName() override
    { return "__cxa_deleted_virtual"; }

  CharUnits getArrayCookieSizeImpl(QualType elementType) override;
  llvm37::Value *InitializeArrayCookie(CodeGenFunction &CGF,
                                     llvm37::Value *NewPtr,
                                     llvm37::Value *NumElements,
                                     const CXXNewExpr *expr,
                                     QualType ElementType) override;
  llvm37::Value *readArrayCookieImpl(CodeGenFunction &CGF,
                                   llvm37::Value *allocPtr,
                                   CharUnits cookieSize) override;

  void EmitGuardedInit(CodeGenFunction &CGF, const VarDecl &D,
                       llvm37::GlobalVariable *DeclPtr,
                       bool PerformInit) override;
  void registerGlobalDtor(CodeGenFunction &CGF, const VarDecl &D,
                          llvm37::Constant *dtor, llvm37::Constant *addr) override;

  llvm37::Function *getOrCreateThreadLocalWrapper(const VarDecl *VD,
                                                llvm37::Value *Val);
  void EmitThreadLocalInitFuncs(
      CodeGenModule &CGM,
      ArrayRef<std::pair<const VarDecl *, llvm37::GlobalVariable *>>
          CXXThreadLocals,
      ArrayRef<llvm37::Function *> CXXThreadLocalInits,
      ArrayRef<llvm37::GlobalVariable *> CXXThreadLocalInitVars) override;

  bool usesThreadWrapperFunction() const override { return true; }
  LValue EmitThreadLocalVarDeclLValue(CodeGenFunction &CGF, const VarDecl *VD,
                                      QualType LValType) override;

  bool NeedsVTTParameter(GlobalDecl GD) override;

  /**************************** RTTI Uniqueness ******************************/

protected:
  /// Returns true if the ABI requires RTTI type_info objects to be unique
  /// across a program.
  virtual bool shouldRTTIBeUnique() const { return true; }

public:
  /// What sort of unique-RTTI behavior should we use?
  enum RTTIUniquenessKind {
    /// We are guaranteeing, or need to guarantee, that the RTTI string
    /// is unique.
    RUK_Unique,

    /// We are not guaranteeing uniqueness for the RTTI string, so we
    /// can demote to hidden visibility but must use string comparisons.
    RUK_NonUniqueHidden,

    /// We are not guaranteeing uniqueness for the RTTI string, so we
    /// have to use string comparisons, but we also have to emit it with
    /// non-hidden visibility.
    RUK_NonUniqueVisible
  };

  /// Return the required visibility status for the given type and linkage in
  /// the current ABI.
  RTTIUniquenessKind
  classifyRTTIUniqueness(QualType CanTy,
                         llvm37::GlobalValue::LinkageTypes Linkage) const;
  friend class ItaniumRTTIBuilder;

  void emitCXXStructor(const CXXMethodDecl *MD, StructorType Type) override;
};

class ARMCXXABI : public ItaniumCXXABI {
public:
  ARMCXXABI(CodeGen::CodeGenModule &CGM) :
    ItaniumCXXABI(CGM, /* UseARMMethodPtrABI = */ true,
                  /* UseARMGuardVarABI = */ true) {}

  bool HasThisReturn(GlobalDecl GD) const override {
    return (isa<CXXConstructorDecl>(GD.getDecl()) || (
              isa<CXXDestructorDecl>(GD.getDecl()) &&
              GD.getDtorType() != Dtor_Deleting));
  }

  void EmitReturnFromThunk(CodeGenFunction &CGF, RValue RV,
                           QualType ResTy) override;

  CharUnits getArrayCookieSizeImpl(QualType elementType) override;
  llvm37::Value *InitializeArrayCookie(CodeGenFunction &CGF,
                                     llvm37::Value *NewPtr,
                                     llvm37::Value *NumElements,
                                     const CXXNewExpr *expr,
                                     QualType ElementType) override;
  llvm37::Value *readArrayCookieImpl(CodeGenFunction &CGF, llvm37::Value *allocPtr,
                                   CharUnits cookieSize) override;
};

class iOS64CXXABI : public ARMCXXABI {
public:
  iOS64CXXABI(CodeGen::CodeGenModule &CGM) : ARMCXXABI(CGM) {}

  // ARM64 libraries are prepared for non-unique RTTI.
  bool shouldRTTIBeUnique() const override { return false; }
};
}

CodeGen::CGCXXABI *CodeGen::CreateItaniumCXXABI(CodeGenModule &CGM) {
  switch (CGM.getTarget().getCXXABI().getKind()) {
  // For IR-generation purposes, there's no significant difference
  // between the ARM and iOS ABIs.
  case TargetCXXABI::GenericARM:
  case TargetCXXABI::iOS:
    return new ARMCXXABI(CGM);

  case TargetCXXABI::iOS64:
    return new iOS64CXXABI(CGM);

  // Note that AArch64 uses the generic ItaniumCXXABI class since it doesn't
  // include the other 32-bit ARM oddities: constructor/destructor return values
  // and array cookies.
  case TargetCXXABI::GenericAArch64:
    return new ItaniumCXXABI(CGM, /* UseARMMethodPtrABI = */ true,
                             /* UseARMGuardVarABI = */ true);

  case TargetCXXABI::GenericMIPS:
    return new ItaniumCXXABI(CGM, /* UseARMMethodPtrABI = */ true);

  case TargetCXXABI::GenericItanium:
    if (CGM.getContext().getTargetInfo().getTriple().getArch()
        == llvm37::Triple::le32) {
      // For PNaCl, use ARM-style method pointers so that PNaCl code
      // does not assume anything about the alignment of function
      // pointers.
      return new ItaniumCXXABI(CGM, /* UseARMMethodPtrABI = */ true,
                               /* UseARMGuardVarABI = */ false);
    }
    return new ItaniumCXXABI(CGM);

  case TargetCXXABI::Microsoft:
    llvm_unreachable("Microsoft ABI is not Itanium-based");
  }
  llvm_unreachable("bad ABI kind");
}

llvm37::Type *
ItaniumCXXABI::ConvertMemberPointerType(const MemberPointerType *MPT) {
  if (MPT->isMemberDataPointer())
    return CGM.PtrDiffTy;
  return llvm37::StructType::get(CGM.PtrDiffTy, CGM.PtrDiffTy, nullptr);
}

/// In the Itanium and ARM ABIs, method pointers have the form:
///   struct { ptrdiff_t ptr; ptrdiff_t adj; } memptr;
///
/// In the Itanium ABI:
///  - method pointers are virtual if (memptr.ptr & 1) is nonzero
///  - the this-adjustment is (memptr.adj)
///  - the virtual offset is (memptr.ptr - 1)
///
/// In the ARM ABI:
///  - method pointers are virtual if (memptr.adj & 1) is nonzero
///  - the this-adjustment is (memptr.adj >> 1)
///  - the virtual offset is (memptr.ptr)
/// ARM uses 'adj' for the virtual flag because Thumb functions
/// may be only single-byte aligned.
///
/// If the member is virtual, the adjusted 'this' pointer points
/// to a vtable pointer from which the virtual offset is applied.
///
/// If the member is non-virtual, memptr.ptr is the address of
/// the function to call.
llvm37::Value *ItaniumCXXABI::EmitLoadOfMemberFunctionPointer(
    CodeGenFunction &CGF, const Expr *E, llvm37::Value *&This,
    llvm37::Value *MemFnPtr, const MemberPointerType *MPT) {
  CGBuilderTy &Builder = CGF.Builder;

  const FunctionProtoType *FPT = 
    MPT->getPointeeType()->getAs<FunctionProtoType>();
  const CXXRecordDecl *RD = 
    cast<CXXRecordDecl>(MPT->getClass()->getAs<RecordType>()->getDecl());

  llvm37::FunctionType *FTy = 
    CGM.getTypes().GetFunctionType(
      CGM.getTypes().arrangeCXXMethodType(RD, FPT));

  llvm37::Constant *ptrdiff_1 = llvm37::ConstantInt::get(CGM.PtrDiffTy, 1);

  llvm37::BasicBlock *FnVirtual = CGF.createBasicBlock("memptr.virtual");
  llvm37::BasicBlock *FnNonVirtual = CGF.createBasicBlock("memptr.nonvirtual");
  llvm37::BasicBlock *FnEnd = CGF.createBasicBlock("memptr.end");

  // Extract memptr.adj, which is in the second field.
  llvm37::Value *RawAdj = Builder.CreateExtractValue(MemFnPtr, 1, "memptr.adj");

  // Compute the true adjustment.
  llvm37::Value *Adj = RawAdj;
  if (UseARMMethodPtrABI)
    Adj = Builder.CreateAShr(Adj, ptrdiff_1, "memptr.adj.shifted");

  // Apply the adjustment and cast back to the original struct type
  // for consistency.
  llvm37::Value *Ptr = Builder.CreateBitCast(This, Builder.getInt8PtrTy());
  Ptr = Builder.CreateInBoundsGEP(Ptr, Adj);
  This = Builder.CreateBitCast(Ptr, This->getType(), "this.adjusted");
  
  // Load the function pointer.
  llvm37::Value *FnAsInt = Builder.CreateExtractValue(MemFnPtr, 0, "memptr.ptr");
  
  // If the LSB in the function pointer is 1, the function pointer points to
  // a virtual function.
  llvm37::Value *IsVirtual;
  if (UseARMMethodPtrABI)
    IsVirtual = Builder.CreateAnd(RawAdj, ptrdiff_1);
  else
    IsVirtual = Builder.CreateAnd(FnAsInt, ptrdiff_1);
  IsVirtual = Builder.CreateIsNotNull(IsVirtual, "memptr.isvirtual");
  Builder.CreateCondBr(IsVirtual, FnVirtual, FnNonVirtual);

  // In the virtual path, the adjustment left 'This' pointing to the
  // vtable of the correct base subobject.  The "function pointer" is an
  // offset within the vtable (+1 for the virtual flag on non-ARM).
  CGF.EmitBlock(FnVirtual);

  // Cast the adjusted this to a pointer to vtable pointer and load.
  llvm37::Type *VTableTy = Builder.getInt8PtrTy();
  llvm37::Value *VTable = CGF.GetVTablePtr(This, VTableTy);

  // Apply the offset.
  llvm37::Value *VTableOffset = FnAsInt;
  if (!UseARMMethodPtrABI)
    VTableOffset = Builder.CreateSub(VTableOffset, ptrdiff_1);
  VTable = Builder.CreateGEP(VTable, VTableOffset);

  // Load the virtual function to call.
  VTable = Builder.CreateBitCast(VTable, FTy->getPointerTo()->getPointerTo());
  llvm37::Value *VirtualFn = Builder.CreateLoad(VTable, "memptr.virtualfn");
  CGF.EmitBranch(FnEnd);

  // In the non-virtual path, the function pointer is actually a
  // function pointer.
  CGF.EmitBlock(FnNonVirtual);
  llvm37::Value *NonVirtualFn =
    Builder.CreateIntToPtr(FnAsInt, FTy->getPointerTo(), "memptr.nonvirtualfn");
  
  // We're done.
  CGF.EmitBlock(FnEnd);
  llvm37::PHINode *Callee = Builder.CreatePHI(FTy->getPointerTo(), 2);
  Callee->addIncoming(VirtualFn, FnVirtual);
  Callee->addIncoming(NonVirtualFn, FnNonVirtual);
  return Callee;
}

/// Compute an l-value by applying the given pointer-to-member to a
/// base object.
llvm37::Value *ItaniumCXXABI::EmitMemberDataPointerAddress(
    CodeGenFunction &CGF, const Expr *E, llvm37::Value *Base, llvm37::Value *MemPtr,
    const MemberPointerType *MPT) {
  assert(MemPtr->getType() == CGM.PtrDiffTy);

  CGBuilderTy &Builder = CGF.Builder;

  unsigned AS = Base->getType()->getPointerAddressSpace();

  // Cast to char*.
  Base = Builder.CreateBitCast(Base, Builder.getInt8Ty()->getPointerTo(AS));

  // Apply the offset, which we assume is non-null.
  llvm37::Value *Addr = Builder.CreateInBoundsGEP(Base, MemPtr, "memptr.offset");

  // Cast the address to the appropriate pointer type, adopting the
  // address space of the base pointer.
  llvm37::Type *PType
    = CGF.ConvertTypeForMem(MPT->getPointeeType())->getPointerTo(AS);
  return Builder.CreateBitCast(Addr, PType);
}

/// Perform a bitcast, derived-to-base, or base-to-derived member pointer
/// conversion.
///
/// Bitcast conversions are always a no-op under Itanium.
///
/// Obligatory offset/adjustment diagram:
///         <-- offset -->          <-- adjustment -->
///   |--------------------------|----------------------|--------------------|
///   ^Derived address point     ^Base address point    ^Member address point
///
/// So when converting a base member pointer to a derived member pointer,
/// we add the offset to the adjustment because the address point has
/// decreased;  and conversely, when converting a derived MP to a base MP
/// we subtract the offset from the adjustment because the address point
/// has increased.
///
/// The standard forbids (at compile time) conversion to and from
/// virtual bases, which is why we don't have to consider them here.
///
/// The standard forbids (at run time) casting a derived MP to a base
/// MP when the derived MP does not point to a member of the base.
/// This is why -1 is a reasonable choice for null data member
/// pointers.
llvm37::Value *
ItaniumCXXABI::EmitMemberPointerConversion(CodeGenFunction &CGF,
                                           const CastExpr *E,
                                           llvm37::Value *src) {
  assert(E->getCastKind() == CK_DerivedToBaseMemberPointer ||
         E->getCastKind() == CK_BaseToDerivedMemberPointer ||
         E->getCastKind() == CK_ReinterpretMemberPointer);

  // Under Itanium, reinterprets don't require any additional processing.
  if (E->getCastKind() == CK_ReinterpretMemberPointer) return src;

  // Use constant emission if we can.
  if (isa<llvm37::Constant>(src))
    return EmitMemberPointerConversion(E, cast<llvm37::Constant>(src));

  llvm37::Constant *adj = getMemberPointerAdjustment(E);
  if (!adj) return src;

  CGBuilderTy &Builder = CGF.Builder;
  bool isDerivedToBase = (E->getCastKind() == CK_DerivedToBaseMemberPointer);

  const MemberPointerType *destTy =
    E->getType()->castAs<MemberPointerType>();

  // For member data pointers, this is just a matter of adding the
  // offset if the source is non-null.
  if (destTy->isMemberDataPointer()) {
    llvm37::Value *dst;
    if (isDerivedToBase)
      dst = Builder.CreateNSWSub(src, adj, "adj");
    else
      dst = Builder.CreateNSWAdd(src, adj, "adj");

    // Null check.
    llvm37::Value *null = llvm37::Constant::getAllOnesValue(src->getType());
    llvm37::Value *isNull = Builder.CreateICmpEQ(src, null, "memptr.isnull");
    return Builder.CreateSelect(isNull, src, dst);
  }

  // The this-adjustment is left-shifted by 1 on ARM.
  if (UseARMMethodPtrABI) {
    uint64_t offset = cast<llvm37::ConstantInt>(adj)->getZExtValue();
    offset <<= 1;
    adj = llvm37::ConstantInt::get(adj->getType(), offset);
  }

  llvm37::Value *srcAdj = Builder.CreateExtractValue(src, 1, "src.adj");
  llvm37::Value *dstAdj;
  if (isDerivedToBase)
    dstAdj = Builder.CreateNSWSub(srcAdj, adj, "adj");
  else
    dstAdj = Builder.CreateNSWAdd(srcAdj, adj, "adj");

  return Builder.CreateInsertValue(src, dstAdj, 1);
}

llvm37::Constant *
ItaniumCXXABI::EmitMemberPointerConversion(const CastExpr *E,
                                           llvm37::Constant *src) {
  assert(E->getCastKind() == CK_DerivedToBaseMemberPointer ||
         E->getCastKind() == CK_BaseToDerivedMemberPointer ||
         E->getCastKind() == CK_ReinterpretMemberPointer);

  // Under Itanium, reinterprets don't require any additional processing.
  if (E->getCastKind() == CK_ReinterpretMemberPointer) return src;

  // If the adjustment is trivial, we don't need to do anything.
  llvm37::Constant *adj = getMemberPointerAdjustment(E);
  if (!adj) return src;

  bool isDerivedToBase = (E->getCastKind() == CK_DerivedToBaseMemberPointer);

  const MemberPointerType *destTy =
    E->getType()->castAs<MemberPointerType>();

  // For member data pointers, this is just a matter of adding the
  // offset if the source is non-null.
  if (destTy->isMemberDataPointer()) {
    // null maps to null.
    if (src->isAllOnesValue()) return src;

    if (isDerivedToBase)
      return llvm37::ConstantExpr::getNSWSub(src, adj);
    else
      return llvm37::ConstantExpr::getNSWAdd(src, adj);
  }

  // The this-adjustment is left-shifted by 1 on ARM.
  if (UseARMMethodPtrABI) {
    uint64_t offset = cast<llvm37::ConstantInt>(adj)->getZExtValue();
    offset <<= 1;
    adj = llvm37::ConstantInt::get(adj->getType(), offset);
  }

  llvm37::Constant *srcAdj = llvm37::ConstantExpr::getExtractValue(src, 1);
  llvm37::Constant *dstAdj;
  if (isDerivedToBase)
    dstAdj = llvm37::ConstantExpr::getNSWSub(srcAdj, adj);
  else
    dstAdj = llvm37::ConstantExpr::getNSWAdd(srcAdj, adj);

  return llvm37::ConstantExpr::getInsertValue(src, dstAdj, 1);
}

llvm37::Constant *
ItaniumCXXABI::EmitNullMemberPointer(const MemberPointerType *MPT) {
  // Itanium C++ ABI 2.3:
  //   A NULL pointer is represented as -1.
  if (MPT->isMemberDataPointer()) 
    return llvm37::ConstantInt::get(CGM.PtrDiffTy, -1ULL, /*isSigned=*/true);

  llvm37::Constant *Zero = llvm37::ConstantInt::get(CGM.PtrDiffTy, 0);
  llvm37::Constant *Values[2] = { Zero, Zero };
  return llvm37::ConstantStruct::getAnon(Values);
}

llvm37::Constant *
ItaniumCXXABI::EmitMemberDataPointer(const MemberPointerType *MPT,
                                     CharUnits offset) {
  // Itanium C++ ABI 2.3:
  //   A pointer to data member is an offset from the base address of
  //   the class object containing it, represented as a ptrdiff_t
  return llvm37::ConstantInt::get(CGM.PtrDiffTy, offset.getQuantity());
}

llvm37::Constant *
ItaniumCXXABI::EmitMemberFunctionPointer(const CXXMethodDecl *MD) {
  return BuildMemberPointer(MD, CharUnits::Zero());
}

llvm37::Constant *ItaniumCXXABI::BuildMemberPointer(const CXXMethodDecl *MD,
                                                  CharUnits ThisAdjustment) {
  assert(MD->isInstance() && "Member function must not be static!");
  MD = MD->getCanonicalDecl();

  CodeGenTypes &Types = CGM.getTypes();

  // Get the function pointer (or index if this is a virtual function).
  llvm37::Constant *MemPtr[2];
  if (MD->isVirtual()) {
    uint64_t Index = CGM.getItaniumVTableContext().getMethodVTableIndex(MD);

    const ASTContext &Context = getContext();
    CharUnits PointerWidth =
      Context.toCharUnitsFromBits(Context.getTargetInfo().getPointerWidth(0));
    uint64_t VTableOffset = (Index * PointerWidth.getQuantity());

    if (UseARMMethodPtrABI) {
      // ARM C++ ABI 3.2.1:
      //   This ABI specifies that adj contains twice the this
      //   adjustment, plus 1 if the member function is virtual. The
      //   least significant bit of adj then makes exactly the same
      //   discrimination as the least significant bit of ptr does for
      //   Itanium.
      MemPtr[0] = llvm37::ConstantInt::get(CGM.PtrDiffTy, VTableOffset);
      MemPtr[1] = llvm37::ConstantInt::get(CGM.PtrDiffTy,
                                         2 * ThisAdjustment.getQuantity() + 1);
    } else {
      // Itanium C++ ABI 2.3:
      //   For a virtual function, [the pointer field] is 1 plus the
      //   virtual table offset (in bytes) of the function,
      //   represented as a ptrdiff_t.
      MemPtr[0] = llvm37::ConstantInt::get(CGM.PtrDiffTy, VTableOffset + 1);
      MemPtr[1] = llvm37::ConstantInt::get(CGM.PtrDiffTy,
                                         ThisAdjustment.getQuantity());
    }
  } else {
    const FunctionProtoType *FPT = MD->getType()->castAs<FunctionProtoType>();
    llvm37::Type *Ty;
    // Check whether the function has a computable LLVM signature.
    if (Types.isFuncTypeConvertible(FPT)) {
      // The function has a computable LLVM signature; use the correct type.
      Ty = Types.GetFunctionType(Types.arrangeCXXMethodDeclaration(MD));
    } else {
      // Use an arbitrary non-function type to tell GetAddrOfFunction that the
      // function type is incomplete.
      Ty = CGM.PtrDiffTy;
    }
    llvm37::Constant *addr = CGM.GetAddrOfFunction(MD, Ty);

    MemPtr[0] = llvm37::ConstantExpr::getPtrToInt(addr, CGM.PtrDiffTy);
    MemPtr[1] = llvm37::ConstantInt::get(CGM.PtrDiffTy,
                                       (UseARMMethodPtrABI ? 2 : 1) *
                                       ThisAdjustment.getQuantity());
  }
  
  return llvm37::ConstantStruct::getAnon(MemPtr);
}

llvm37::Constant *ItaniumCXXABI::EmitMemberPointer(const APValue &MP,
                                                 QualType MPType) {
  const MemberPointerType *MPT = MPType->castAs<MemberPointerType>();
  const ValueDecl *MPD = MP.getMemberPointerDecl();
  if (!MPD)
    return EmitNullMemberPointer(MPT);

  CharUnits ThisAdjustment = getMemberPointerPathAdjustment(MP);

  if (const CXXMethodDecl *MD = dyn_cast<CXXMethodDecl>(MPD))
    return BuildMemberPointer(MD, ThisAdjustment);

  CharUnits FieldOffset =
    getContext().toCharUnitsFromBits(getContext().getFieldOffset(MPD));
  return EmitMemberDataPointer(MPT, ThisAdjustment + FieldOffset);
}

/// The comparison algorithm is pretty easy: the member pointers are
/// the same if they're either bitwise identical *or* both null.
///
/// ARM is different here only because null-ness is more complicated.
llvm37::Value *
ItaniumCXXABI::EmitMemberPointerComparison(CodeGenFunction &CGF,
                                           llvm37::Value *L,
                                           llvm37::Value *R,
                                           const MemberPointerType *MPT,
                                           bool Inequality) {
  CGBuilderTy &Builder = CGF.Builder;

  llvm37::ICmpInst::Predicate Eq;
  llvm37::Instruction::BinaryOps And, Or;
  if (Inequality) {
    Eq = llvm37::ICmpInst::ICMP_NE;
    And = llvm37::Instruction::Or;
    Or = llvm37::Instruction::And;
  } else {
    Eq = llvm37::ICmpInst::ICMP_EQ;
    And = llvm37::Instruction::And;
    Or = llvm37::Instruction::Or;
  }

  // Member data pointers are easy because there's a unique null
  // value, so it just comes down to bitwise equality.
  if (MPT->isMemberDataPointer())
    return Builder.CreateICmp(Eq, L, R);

  // For member function pointers, the tautologies are more complex.
  // The Itanium tautology is:
  //   (L == R) <==> (L.ptr == R.ptr && (L.ptr == 0 || L.adj == R.adj))
  // The ARM tautology is:
  //   (L == R) <==> (L.ptr == R.ptr &&
  //                  (L.adj == R.adj ||
  //                   (L.ptr == 0 && ((L.adj|R.adj) & 1) == 0)))
  // The inequality tautologies have exactly the same structure, except
  // applying De Morgan's laws.
  
  llvm37::Value *LPtr = Builder.CreateExtractValue(L, 0, "lhs.memptr.ptr");
  llvm37::Value *RPtr = Builder.CreateExtractValue(R, 0, "rhs.memptr.ptr");

  // This condition tests whether L.ptr == R.ptr.  This must always be
  // true for equality to hold.
  llvm37::Value *PtrEq = Builder.CreateICmp(Eq, LPtr, RPtr, "cmp.ptr");

  // This condition, together with the assumption that L.ptr == R.ptr,
  // tests whether the pointers are both null.  ARM imposes an extra
  // condition.
  llvm37::Value *Zero = llvm37::Constant::getNullValue(LPtr->getType());
  llvm37::Value *EqZero = Builder.CreateICmp(Eq, LPtr, Zero, "cmp.ptr.null");

  // This condition tests whether L.adj == R.adj.  If this isn't
  // true, the pointers are unequal unless they're both null.
  llvm37::Value *LAdj = Builder.CreateExtractValue(L, 1, "lhs.memptr.adj");
  llvm37::Value *RAdj = Builder.CreateExtractValue(R, 1, "rhs.memptr.adj");
  llvm37::Value *AdjEq = Builder.CreateICmp(Eq, LAdj, RAdj, "cmp.adj");

  // Null member function pointers on ARM clear the low bit of Adj,
  // so the zero condition has to check that neither low bit is set.
  if (UseARMMethodPtrABI) {
    llvm37::Value *One = llvm37::ConstantInt::get(LPtr->getType(), 1);

    // Compute (l.adj | r.adj) & 1 and test it against zero.
    llvm37::Value *OrAdj = Builder.CreateOr(LAdj, RAdj, "or.adj");
    llvm37::Value *OrAdjAnd1 = Builder.CreateAnd(OrAdj, One);
    llvm37::Value *OrAdjAnd1EqZero = Builder.CreateICmp(Eq, OrAdjAnd1, Zero,
                                                      "cmp.or.adj");
    EqZero = Builder.CreateBinOp(And, EqZero, OrAdjAnd1EqZero);
  }

  // Tie together all our conditions.
  llvm37::Value *Result = Builder.CreateBinOp(Or, EqZero, AdjEq);
  Result = Builder.CreateBinOp(And, PtrEq, Result,
                               Inequality ? "memptr.ne" : "memptr.eq");
  return Result;
}

llvm37::Value *
ItaniumCXXABI::EmitMemberPointerIsNotNull(CodeGenFunction &CGF,
                                          llvm37::Value *MemPtr,
                                          const MemberPointerType *MPT) {
  CGBuilderTy &Builder = CGF.Builder;

  /// For member data pointers, this is just a check against -1.
  if (MPT->isMemberDataPointer()) {
    assert(MemPtr->getType() == CGM.PtrDiffTy);
    llvm37::Value *NegativeOne =
      llvm37::Constant::getAllOnesValue(MemPtr->getType());
    return Builder.CreateICmpNE(MemPtr, NegativeOne, "memptr.tobool");
  }
  
  // In Itanium, a member function pointer is not null if 'ptr' is not null.
  llvm37::Value *Ptr = Builder.CreateExtractValue(MemPtr, 0, "memptr.ptr");

  llvm37::Constant *Zero = llvm37::ConstantInt::get(Ptr->getType(), 0);
  llvm37::Value *Result = Builder.CreateICmpNE(Ptr, Zero, "memptr.tobool");

  // On ARM, a member function pointer is also non-null if the low bit of 'adj'
  // (the virtual bit) is set.
  if (UseARMMethodPtrABI) {
    llvm37::Constant *One = llvm37::ConstantInt::get(Ptr->getType(), 1);
    llvm37::Value *Adj = Builder.CreateExtractValue(MemPtr, 1, "memptr.adj");
    llvm37::Value *VirtualBit = Builder.CreateAnd(Adj, One, "memptr.virtualbit");
    llvm37::Value *IsVirtual = Builder.CreateICmpNE(VirtualBit, Zero,
                                                  "memptr.isvirtual");
    Result = Builder.CreateOr(Result, IsVirtual);
  }

  return Result;
}

bool ItaniumCXXABI::classifyReturnType(CGFunctionInfo &FI) const {
  const CXXRecordDecl *RD = FI.getReturnType()->getAsCXXRecordDecl();
  if (!RD)
    return false;

  // Return indirectly if we have a non-trivial copy ctor or non-trivial dtor.
  // FIXME: Use canCopyArgument() when it is fixed to handle lazily declared
  // special members.
  if (RD->hasNonTrivialDestructor() || RD->hasNonTrivialCopyConstructor()) {
    FI.getReturnInfo() = ABIArgInfo::getIndirect(0, /*ByVal=*/false);
    return true;
  }
  return false;
}

/// The Itanium ABI requires non-zero initialization only for data
/// member pointers, for which '0' is a valid offset.
bool ItaniumCXXABI::isZeroInitializable(const MemberPointerType *MPT) {
  return MPT->isMemberFunctionPointer();
}

/// The Itanium ABI always places an offset to the complete object
/// at entry -2 in the vtable.
void ItaniumCXXABI::emitVirtualObjectDelete(CodeGenFunction &CGF,
                                            const CXXDeleteExpr *DE,
                                            llvm37::Value *Ptr,
                                            QualType ElementType,
                                            const CXXDestructorDecl *Dtor) {
  bool UseGlobalDelete = DE->isGlobalDelete();
  if (UseGlobalDelete) {
    // Derive the complete-object pointer, which is what we need
    // to pass to the deallocation function.

    // Grab the vtable pointer as an intptr_t*.
    llvm37::Value *VTable = CGF.GetVTablePtr(Ptr, CGF.IntPtrTy->getPointerTo());

    // Track back to entry -2 and pull out the offset there.
    llvm37::Value *OffsetPtr = CGF.Builder.CreateConstInBoundsGEP1_64(
        VTable, -2, "complete-offset.ptr");
    llvm37::LoadInst *Offset = CGF.Builder.CreateLoad(OffsetPtr);
    Offset->setAlignment(CGF.PointerAlignInBytes);

    // Apply the offset.
    llvm37::Value *CompletePtr = CGF.Builder.CreateBitCast(Ptr, CGF.Int8PtrTy);
    CompletePtr = CGF.Builder.CreateInBoundsGEP(CompletePtr, Offset);

    // If we're supposed to call the global delete, make sure we do so
    // even if the destructor throws.
    CGF.pushCallObjectDeleteCleanup(DE->getOperatorDelete(), CompletePtr,
                                    ElementType);
  }

  // FIXME: Provide a source location here even though there's no
  // CXXMemberCallExpr for dtor call.
  CXXDtorType DtorType = UseGlobalDelete ? Dtor_Complete : Dtor_Deleting;
  EmitVirtualDestructorCall(CGF, Dtor, DtorType, Ptr, /*CE=*/nullptr);

  if (UseGlobalDelete)
    CGF.PopCleanupBlock();
}

void ItaniumCXXABI::emitRethrow(CodeGenFunction &CGF, bool isNoReturn) {
  // void __cxa_rethrow();

  llvm37::FunctionType *FTy =
    llvm37::FunctionType::get(CGM.VoidTy, /*IsVarArgs=*/false);

  llvm37::Constant *Fn = CGM.CreateRuntimeFunction(FTy, "__cxa_rethrow");

  if (isNoReturn)
    CGF.EmitNoreturnRuntimeCallOrInvoke(Fn, None);
  else
    CGF.EmitRuntimeCallOrInvoke(Fn);
}

#if 0 // HLSL Change Starts
static llvm37::Constant *getAllocateExceptionFn(CodeGenModule &CGM) {
  // void *__cxa_allocate_exception(size_t thrown_size);

  llvm37::FunctionType *FTy =
    llvm37::FunctionType::get(CGM.Int8PtrTy, CGM.SizeTy, /*IsVarArgs=*/false);

  return CGM.CreateRuntimeFunction(FTy, "__cxa_allocate_exception");
}

static llvm37::Constant *getThrowFn(CodeGenModule &CGM) {
  // void __cxa_throw(void *thrown_exception, std::type_info *tinfo,
  //                  void (*dest) (void *));

  llvm37::Type *Args[3] = { CGM.Int8PtrTy, CGM.Int8PtrTy, CGM.Int8PtrTy };
  llvm37::FunctionType *FTy =
    llvm37::FunctionType::get(CGM.VoidTy, Args, /*IsVarArgs=*/false);

  return CGM.CreateRuntimeFunction(FTy, "__cxa_throw");
}
#endif

void ItaniumCXXABI::emitThrow(CodeGenFunction &CGF, const CXXThrowExpr *E) {
#if 1 // HLSL Change Starts
  llvm_unreachable("HLSL does not support throw.");
#else // HLSL Change Ends
    QualType ThrowType = E->getSubExpr()->getType();
  // Now allocate the exception object.
  llvm37::Type *SizeTy = CGF.ConvertType(getContext().getSizeType());
  uint64_t TypeSize = getContext().getTypeSizeInChars(ThrowType).getQuantity();

  llvm37::Constant *AllocExceptionFn = getAllocateExceptionFn(CGM);
  llvm37::CallInst *ExceptionPtr = CGF.EmitNounwindRuntimeCall(
      AllocExceptionFn, llvm37::ConstantInt::get(SizeTy, TypeSize), "exception");

  CGF.EmitAnyExprToExn(E->getSubExpr(), ExceptionPtr);

  // Now throw the exception.
  llvm37::Constant *TypeInfo = CGM.GetAddrOfRTTIDescriptor(ThrowType,
                                                         /*ForEH=*/true);

  // The address of the destructor.  If the exception type has a
  // trivial destructor (or isn't a record), we just pass null.
  llvm37::Constant *Dtor = nullptr;
  if (const RecordType *RecordTy = ThrowType->getAs<RecordType>()) {
    CXXRecordDecl *Record = cast<CXXRecordDecl>(RecordTy->getDecl());
    if (!Record->hasTrivialDestructor()) {
      CXXDestructorDecl *DtorD = Record->getDestructor();
      Dtor = CGM.getAddrOfCXXStructor(DtorD, StructorType::Complete);
      Dtor = llvm37::ConstantExpr::getBitCast(Dtor, CGM.Int8PtrTy);
    }
  }
  if (!Dtor) Dtor = llvm37::Constant::getNullValue(CGM.Int8PtrTy);

  llvm37::Value *args[] = { ExceptionPtr, TypeInfo, Dtor };
  CGF.EmitNoreturnRuntimeCallOrInvoke(getThrowFn(CGM), args);
#endif // HLSL Change
}

static llvm37::Constant *getItaniumDynamicCastFn(CodeGenFunction &CGF) {
  // void *__dynamic_cast(const void *sub,
  //                      const abi::__class_type_info *src,
  //                      const abi::__class_type_info *dst,
  //                      std::ptrdiff_t src2dst_offset);
  
  llvm37::Type *Int8PtrTy = CGF.Int8PtrTy;
  llvm37::Type *PtrDiffTy = 
    CGF.ConvertType(CGF.getContext().getPointerDiffType());

  llvm37::Type *Args[4] = { Int8PtrTy, Int8PtrTy, Int8PtrTy, PtrDiffTy };

  llvm37::FunctionType *FTy = llvm37::FunctionType::get(Int8PtrTy, Args, false);

  // Mark the function as nounwind readonly.
  llvm37::Attribute::AttrKind FuncAttrs[] = { llvm37::Attribute::NoUnwind,
                                            llvm37::Attribute::ReadOnly };
  llvm37::AttributeSet Attrs = llvm37::AttributeSet::get(
      CGF.getLLVMContext(), llvm37::AttributeSet::FunctionIndex, FuncAttrs);

  return CGF.CGM.CreateRuntimeFunction(FTy, "__dynamic_cast", Attrs);
}

static llvm37::Constant *getBadCastFn(CodeGenFunction &CGF) {
  // void __cxa_bad_cast();
  llvm37::FunctionType *FTy = llvm37::FunctionType::get(CGF.VoidTy, false);
  return CGF.CGM.CreateRuntimeFunction(FTy, "__cxa_bad_cast");
}

/// \brief Compute the src2dst_offset hint as described in the
/// Itanium C++ ABI [2.9.7]
static CharUnits computeOffsetHint(ASTContext &Context,
                                   const CXXRecordDecl *Src,
                                   const CXXRecordDecl *Dst) {
  CXXBasePaths Paths(/*FindAmbiguities=*/true, /*RecordPaths=*/true,
                     /*DetectVirtual=*/false);

  // If Dst is not derived from Src we can skip the whole computation below and
  // return that Src is not a public base of Dst.  Record all inheritance paths.
  if (!Dst->isDerivedFrom(Src, Paths))
    return CharUnits::fromQuantity(-2ULL);

  unsigned NumPublicPaths = 0;
  CharUnits Offset;

  // Now walk all possible inheritance paths.
  for (CXXBasePaths::paths_iterator I = Paths.begin(), E = Paths.end(); I != E;
       ++I) {
    if (I->Access != AS_public) // Ignore non-public inheritance.
      continue;

    ++NumPublicPaths;

    for (CXXBasePath::iterator J = I->begin(), JE = I->end(); J != JE; ++J) {
      // If the path contains a virtual base class we can't give any hint.
      // -1: no hint.
      if (J->Base->isVirtual())
        return CharUnits::fromQuantity(-1ULL);

      if (NumPublicPaths > 1) // Won't use offsets, skip computation.
        continue;

      // Accumulate the base class offsets.
      const ASTRecordLayout &L = Context.getASTRecordLayout(J->Class);
      Offset += L.getBaseClassOffset(J->Base->getType()->getAsCXXRecordDecl());
    }
  }

  // -2: Src is not a public base of Dst.
  if (NumPublicPaths == 0)
    return CharUnits::fromQuantity(-2ULL);

  // -3: Src is a multiple public base type but never a virtual base type.
  if (NumPublicPaths > 1)
    return CharUnits::fromQuantity(-3ULL);

  // Otherwise, the Src type is a unique public nonvirtual base type of Dst.
  // Return the offset of Src from the origin of Dst.
  return Offset;
}

static llvm37::Constant *getBadTypeidFn(CodeGenFunction &CGF) {
  // void __cxa_bad_typeid();
  llvm37::FunctionType *FTy = llvm37::FunctionType::get(CGF.VoidTy, false);

  return CGF.CGM.CreateRuntimeFunction(FTy, "__cxa_bad_typeid");
}

bool ItaniumCXXABI::shouldTypeidBeNullChecked(bool IsDeref,
                                              QualType SrcRecordTy) {
  return IsDeref;
}

void ItaniumCXXABI::EmitBadTypeidCall(CodeGenFunction &CGF) {
  llvm37::Value *Fn = getBadTypeidFn(CGF);
  CGF.EmitRuntimeCallOrInvoke(Fn).setDoesNotReturn();
  CGF.Builder.CreateUnreachable();
}

llvm37::Value *ItaniumCXXABI::EmitTypeid(CodeGenFunction &CGF,
                                       QualType SrcRecordTy,
                                       llvm37::Value *ThisPtr,
                                       llvm37::Type *StdTypeInfoPtrTy) {
  llvm37::Value *Value =
      CGF.GetVTablePtr(ThisPtr, StdTypeInfoPtrTy->getPointerTo());

  // Load the type info.
  Value = CGF.Builder.CreateConstInBoundsGEP1_64(Value, -1ULL);
  return CGF.Builder.CreateLoad(Value);
}

bool ItaniumCXXABI::shouldDynamicCastCallBeNullChecked(bool SrcIsPtr,
                                                       QualType SrcRecordTy) {
  return SrcIsPtr;
}

llvm37::Value *ItaniumCXXABI::EmitDynamicCastCall(
    CodeGenFunction &CGF, llvm37::Value *Value, QualType SrcRecordTy,
    QualType DestTy, QualType DestRecordTy, llvm37::BasicBlock *CastEnd) {
  llvm37::Type *PtrDiffLTy =
      CGF.ConvertType(CGF.getContext().getPointerDiffType());
  llvm37::Type *DestLTy = CGF.ConvertType(DestTy);

  llvm37::Value *SrcRTTI =
      CGF.CGM.GetAddrOfRTTIDescriptor(SrcRecordTy.getUnqualifiedType());
  llvm37::Value *DestRTTI =
      CGF.CGM.GetAddrOfRTTIDescriptor(DestRecordTy.getUnqualifiedType());

  // Compute the offset hint.
  const CXXRecordDecl *SrcDecl = SrcRecordTy->getAsCXXRecordDecl();
  const CXXRecordDecl *DestDecl = DestRecordTy->getAsCXXRecordDecl();
  llvm37::Value *OffsetHint = llvm37::ConstantInt::get(
      PtrDiffLTy,
      computeOffsetHint(CGF.getContext(), SrcDecl, DestDecl).getQuantity());

  // Emit the call to __dynamic_cast.
  Value = CGF.EmitCastToVoidPtr(Value);

  llvm37::Value *args[] = {Value, SrcRTTI, DestRTTI, OffsetHint};
  Value = CGF.EmitNounwindRuntimeCall(getItaniumDynamicCastFn(CGF), args);
  Value = CGF.Builder.CreateBitCast(Value, DestLTy);

  /// C++ [expr.dynamic.cast]p9:
  ///   A failed cast to reference type throws std::bad_cast
  if (DestTy->isReferenceType()) {
    llvm37::BasicBlock *BadCastBlock =
        CGF.createBasicBlock("dynamic_cast.bad_cast");

    llvm37::Value *IsNull = CGF.Builder.CreateIsNull(Value);
    CGF.Builder.CreateCondBr(IsNull, BadCastBlock, CastEnd);

    CGF.EmitBlock(BadCastBlock);
    EmitBadCastCall(CGF);
  }

  return Value;
}

llvm37::Value *ItaniumCXXABI::EmitDynamicCastToVoid(CodeGenFunction &CGF,
                                                  llvm37::Value *Value,
                                                  QualType SrcRecordTy,
                                                  QualType DestTy) {
  llvm37::Type *PtrDiffLTy =
      CGF.ConvertType(CGF.getContext().getPointerDiffType());
  llvm37::Type *DestLTy = CGF.ConvertType(DestTy);

  // Get the vtable pointer.
  llvm37::Value *VTable = CGF.GetVTablePtr(Value, PtrDiffLTy->getPointerTo());

  // Get the offset-to-top from the vtable.
  llvm37::Value *OffsetToTop =
      CGF.Builder.CreateConstInBoundsGEP1_64(VTable, -2ULL);
  OffsetToTop = CGF.Builder.CreateLoad(OffsetToTop, "offset.to.top");

  // Finally, add the offset to the pointer.
  Value = CGF.EmitCastToVoidPtr(Value);
  Value = CGF.Builder.CreateInBoundsGEP(Value, OffsetToTop);

  return CGF.Builder.CreateBitCast(Value, DestLTy);
}

bool ItaniumCXXABI::EmitBadCastCall(CodeGenFunction &CGF) {
  llvm37::Value *Fn = getBadCastFn(CGF);
  CGF.EmitRuntimeCallOrInvoke(Fn).setDoesNotReturn();
  CGF.Builder.CreateUnreachable();
  return true;
}

llvm37::Value *
ItaniumCXXABI::GetVirtualBaseClassOffset(CodeGenFunction &CGF,
                                         llvm37::Value *This,
                                         const CXXRecordDecl *ClassDecl,
                                         const CXXRecordDecl *BaseClassDecl) {
  llvm37::Value *VTablePtr = CGF.GetVTablePtr(This, CGM.Int8PtrTy);
  CharUnits VBaseOffsetOffset =
      CGM.getItaniumVTableContext().getVirtualBaseOffsetOffset(ClassDecl,
                                                               BaseClassDecl);

  llvm37::Value *VBaseOffsetPtr =
    CGF.Builder.CreateConstGEP1_64(VTablePtr, VBaseOffsetOffset.getQuantity(),
                                   "vbase.offset.ptr");
  VBaseOffsetPtr = CGF.Builder.CreateBitCast(VBaseOffsetPtr,
                                             CGM.PtrDiffTy->getPointerTo());

  llvm37::Value *VBaseOffset =
    CGF.Builder.CreateLoad(VBaseOffsetPtr, "vbase.offset");

  return VBaseOffset;
}

void ItaniumCXXABI::EmitCXXConstructors(const CXXConstructorDecl *D) {
  // Just make sure we're in sync with TargetCXXABI.
  assert(CGM.getTarget().getCXXABI().hasConstructorVariants());

  // The constructor used for constructing this as a base class;
  // ignores virtual bases.
  CGM.EmitGlobal(GlobalDecl(D, Ctor_Base));

  // The constructor used for constructing this as a complete class;
  // constructs the virtual bases, then calls the base constructor.
  if (!D->getParent()->isAbstract()) {
    // We don't need to emit the complete ctor if the class is abstract.
    CGM.EmitGlobal(GlobalDecl(D, Ctor_Complete));
  }
}

void
ItaniumCXXABI::buildStructorSignature(const CXXMethodDecl *MD, StructorType T,
                                      SmallVectorImpl<CanQualType> &ArgTys) {
  ASTContext &Context = getContext();

  // All parameters are already in place except VTT, which goes after 'this'.
  // These are Clang types, so we don't need to worry about sret yet.

  // Check if we need to add a VTT parameter (which has type void **).
  if (T == StructorType::Base && MD->getParent()->getNumVBases() != 0)
    ArgTys.insert(ArgTys.begin() + 1,
                  Context.getPointerType(Context.VoidPtrTy));
}

void ItaniumCXXABI::EmitCXXDestructors(const CXXDestructorDecl *D) {
  // The destructor used for destructing this as a base class; ignores
  // virtual bases.
  CGM.EmitGlobal(GlobalDecl(D, Dtor_Base));

  // The destructor used for destructing this as a most-derived class;
  // call the base destructor and then destructs any virtual bases.
  CGM.EmitGlobal(GlobalDecl(D, Dtor_Complete));

  // The destructor in a virtual table is always a 'deleting'
  // destructor, which calls the complete destructor and then uses the
  // appropriate operator delete.
  if (D->isVirtual())
    CGM.EmitGlobal(GlobalDecl(D, Dtor_Deleting));
}

void ItaniumCXXABI::addImplicitStructorParams(CodeGenFunction &CGF,
                                              QualType &ResTy,
                                              FunctionArgList &Params) {
  const CXXMethodDecl *MD = cast<CXXMethodDecl>(CGF.CurGD.getDecl());
  assert(isa<CXXConstructorDecl>(MD) || isa<CXXDestructorDecl>(MD));

  // Check if we need a VTT parameter as well.
  if (NeedsVTTParameter(CGF.CurGD)) {
    ASTContext &Context = getContext();

    // FIXME: avoid the fake decl
    QualType T = Context.getPointerType(Context.VoidPtrTy);
    ImplicitParamDecl *VTTDecl
      = ImplicitParamDecl::Create(Context, nullptr, MD->getLocation(),
                                  &Context.Idents.get("vtt"), T);
    Params.insert(Params.begin() + 1, VTTDecl);
    getStructorImplicitParamDecl(CGF) = VTTDecl;
  }
}

void ItaniumCXXABI::EmitInstanceFunctionProlog(CodeGenFunction &CGF) {
  /// Initialize the 'this' slot.
  EmitThisParam(CGF);

  /// Initialize the 'vtt' slot if needed.
  if (getStructorImplicitParamDecl(CGF)) {
    getStructorImplicitParamValue(CGF) = CGF.Builder.CreateLoad(
        CGF.GetAddrOfLocalVar(getStructorImplicitParamDecl(CGF)), "vtt");
  }

  /// If this is a function that the ABI specifies returns 'this', initialize
  /// the return slot to 'this' at the start of the function.
  ///
  /// Unlike the setting of return types, this is done within the ABI
  /// implementation instead of by clients of CGCXXABI because:
  /// 1) getThisValue is currently protected
  /// 2) in theory, an ABI could implement 'this' returns some other way;
  ///    HasThisReturn only specifies a contract, not the implementation
  if (HasThisReturn(CGF.CurGD))
    CGF.Builder.CreateStore(getThisValue(CGF), CGF.ReturnValue);
}

unsigned ItaniumCXXABI::addImplicitConstructorArgs(
    CodeGenFunction &CGF, const CXXConstructorDecl *D, CXXCtorType Type,
    bool ForVirtualBase, bool Delegating, CallArgList &Args) {
  if (!NeedsVTTParameter(GlobalDecl(D, Type)))
    return 0;

  // Insert the implicit 'vtt' argument as the second argument.
  llvm37::Value *VTT =
      CGF.GetVTTParameter(GlobalDecl(D, Type), ForVirtualBase, Delegating);
  QualType VTTTy = getContext().getPointerType(getContext().VoidPtrTy);
  Args.insert(Args.begin() + 1,
              CallArg(RValue::get(VTT), VTTTy, /*needscopy=*/false));
  return 1;  // Added one arg.
}

void ItaniumCXXABI::EmitDestructorCall(CodeGenFunction &CGF,
                                       const CXXDestructorDecl *DD,
                                       CXXDtorType Type, bool ForVirtualBase,
                                       bool Delegating, llvm37::Value *This) {
  GlobalDecl GD(DD, Type);
  llvm37::Value *VTT = CGF.GetVTTParameter(GD, ForVirtualBase, Delegating);
  QualType VTTTy = getContext().getPointerType(getContext().VoidPtrTy);

  llvm37::Value *Callee = nullptr;
  if (getContext().getLangOpts().AppleKext)
    Callee = CGF.BuildAppleKextVirtualDestructorCall(DD, Type, DD->getParent());

  if (!Callee)
    Callee = CGM.getAddrOfCXXStructor(DD, getFromDtorType(Type));

  CGF.EmitCXXMemberOrOperatorCall(DD, Callee, ReturnValueSlot(), This, VTT,
                                  VTTTy, nullptr);
}

void ItaniumCXXABI::emitVTableDefinitions(CodeGenVTables &CGVT,
                                          const CXXRecordDecl *RD) {
  llvm37::GlobalVariable *VTable = getAddrOfVTable(RD, CharUnits());
  if (VTable->hasInitializer())
    return;

  ItaniumVTableContext &VTContext = CGM.getItaniumVTableContext();
  const VTableLayout &VTLayout = VTContext.getVTableLayout(RD);
  llvm37::GlobalVariable::LinkageTypes Linkage = CGM.getVTableLinkage(RD);
  llvm37::Constant *RTTI =
      CGM.GetAddrOfRTTIDescriptor(CGM.getContext().getTagDeclType(RD));

  // Create and set the initializer.
  llvm37::Constant *Init = CGVT.CreateVTableInitializer(
      RD, VTLayout.vtable_component_begin(), VTLayout.getNumVTableComponents(),
      VTLayout.vtable_thunk_begin(), VTLayout.getNumVTableThunks(), RTTI);
  VTable->setInitializer(Init);

  // Set the correct linkage.
  VTable->setLinkage(Linkage);

  if (CGM.supportsCOMDAT() && VTable->isWeakForLinker())
    VTable->setComdat(CGM.getModule().getOrInsertComdat(VTable->getName()));

  // Set the right visibility.
  CGM.setGlobalVisibility(VTable, RD);

  // Use pointer alignment for the vtable. Otherwise we would align them based
  // on the size of the initializer which doesn't make sense as only single
  // values are read.
  unsigned PAlign = CGM.getTarget().getPointerAlign(0);
  VTable->setAlignment(getContext().toCharUnitsFromBits(PAlign).getQuantity());

  // If this is the magic class __cxxabiv1::__fundamental_type_info,
  // we will emit the typeinfo for the fundamental types. This is the
  // same behaviour as GCC.
  const DeclContext *DC = RD->getDeclContext();
  if (RD->getIdentifier() &&
      RD->getIdentifier()->isStr("__fundamental_type_info") &&
      isa<NamespaceDecl>(DC) && cast<NamespaceDecl>(DC)->getIdentifier() &&
      cast<NamespaceDecl>(DC)->getIdentifier()->isStr("__cxxabiv1") &&
      DC->getParent()->isTranslationUnit())
    EmitFundamentalRTTIDescriptors();

  CGM.EmitVTableBitSetEntries(VTable, VTLayout);
}

llvm37::Value *ItaniumCXXABI::getVTableAddressPointInStructor(
    CodeGenFunction &CGF, const CXXRecordDecl *VTableClass, BaseSubobject Base,
    const CXXRecordDecl *NearestVBase, bool &NeedsVirtualOffset) {
  bool NeedsVTTParam = CGM.getCXXABI().NeedsVTTParameter(CGF.CurGD);
  NeedsVirtualOffset = (NeedsVTTParam && NearestVBase);

  llvm37::Value *VTableAddressPoint;
  if (NeedsVTTParam && (Base.getBase()->getNumVBases() || NearestVBase)) {
    // Get the secondary vpointer index.
    uint64_t VirtualPointerIndex =
        CGM.getVTables().getSecondaryVirtualPointerIndex(VTableClass, Base);

    /// Load the VTT.
    llvm37::Value *VTT = CGF.LoadCXXVTT();
    if (VirtualPointerIndex)
      VTT = CGF.Builder.CreateConstInBoundsGEP1_64(VTT, VirtualPointerIndex);

    // And load the address point from the VTT.
    VTableAddressPoint = CGF.Builder.CreateLoad(VTT);
  } else {
    llvm37::Constant *VTable =
        CGM.getCXXABI().getAddrOfVTable(VTableClass, CharUnits());
    uint64_t AddressPoint = CGM.getItaniumVTableContext()
                                .getVTableLayout(VTableClass)
                                .getAddressPoint(Base);
    VTableAddressPoint =
        CGF.Builder.CreateConstInBoundsGEP2_64(VTable, 0, AddressPoint);
  }

  return VTableAddressPoint;
}

llvm37::Constant *ItaniumCXXABI::getVTableAddressPointForConstExpr(
    BaseSubobject Base, const CXXRecordDecl *VTableClass) {
  auto *VTable = getAddrOfVTable(VTableClass, CharUnits());

  // Find the appropriate vtable within the vtable group.
  uint64_t AddressPoint = CGM.getItaniumVTableContext()
                              .getVTableLayout(VTableClass)
                              .getAddressPoint(Base);
  llvm37::Value *Indices[] = {
    llvm37::ConstantInt::get(CGM.Int64Ty, 0),
    llvm37::ConstantInt::get(CGM.Int64Ty, AddressPoint)
  };

  return llvm37::ConstantExpr::getInBoundsGetElementPtr(VTable->getValueType(),
                                                      VTable, Indices);
}

llvm37::GlobalVariable *ItaniumCXXABI::getAddrOfVTable(const CXXRecordDecl *RD,
                                                     CharUnits VPtrOffset) {
  assert(VPtrOffset.isZero() && "Itanium ABI only supports zero vptr offsets");

  llvm37::GlobalVariable *&VTable = VTables[RD];
  if (VTable)
    return VTable;

  // Queue up this v-table for possible deferred emission.
  CGM.addDeferredVTable(RD);

  SmallString<256> OutName;
  llvm37::raw_svector_ostream Out(OutName);
  getMangleContext().mangleCXXVTable(RD, Out);
  Out.flush();
  StringRef Name = OutName.str();

  ItaniumVTableContext &VTContext = CGM.getItaniumVTableContext();
  llvm37::ArrayType *ArrayType = llvm37::ArrayType::get(
      CGM.Int8PtrTy, VTContext.getVTableLayout(RD).getNumVTableComponents());

  VTable = CGM.CreateOrReplaceCXXRuntimeVariable(
      Name, ArrayType, llvm37::GlobalValue::ExternalLinkage);
  VTable->setUnnamedAddr(true);

  if (RD->hasAttr<DLLImportAttr>())
    VTable->setDLLStorageClass(llvm37::GlobalValue::DLLImportStorageClass);
  else if (RD->hasAttr<DLLExportAttr>())
    VTable->setDLLStorageClass(llvm37::GlobalValue::DLLExportStorageClass);

  return VTable;
}

llvm37::Value *ItaniumCXXABI::getVirtualFunctionPointer(CodeGenFunction &CGF,
                                                      GlobalDecl GD,
                                                      llvm37::Value *This,
                                                      llvm37::Type *Ty,
                                                      SourceLocation Loc) {
  GD = GD.getCanonicalDecl();
  Ty = Ty->getPointerTo()->getPointerTo();
  llvm37::Value *VTable = CGF.GetVTablePtr(This, Ty);

  if (CGF.SanOpts.has(SanitizerKind::CFIVCall))
    CGF.EmitVTablePtrCheckForCall(cast<CXXMethodDecl>(GD.getDecl()), VTable,
                                  CodeGenFunction::CFITCK_VCall, Loc);

  uint64_t VTableIndex = CGM.getItaniumVTableContext().getMethodVTableIndex(GD);
  llvm37::Value *VFuncPtr =
      CGF.Builder.CreateConstInBoundsGEP1_64(VTable, VTableIndex, "vfn");
  return CGF.Builder.CreateLoad(VFuncPtr);
}

llvm37::Value *ItaniumCXXABI::EmitVirtualDestructorCall(
    CodeGenFunction &CGF, const CXXDestructorDecl *Dtor, CXXDtorType DtorType,
    llvm37::Value *This, const CXXMemberCallExpr *CE) {
  assert(CE == nullptr || CE->arg_begin() == CE->arg_end());
  assert(DtorType == Dtor_Deleting || DtorType == Dtor_Complete);

  const CGFunctionInfo *FInfo = &CGM.getTypes().arrangeCXXStructorDeclaration(
      Dtor, getFromDtorType(DtorType));
  llvm37::Type *Ty = CGF.CGM.getTypes().GetFunctionType(*FInfo);
  llvm37::Value *Callee =
      getVirtualFunctionPointer(CGF, GlobalDecl(Dtor, DtorType), This, Ty,
                                CE ? CE->getLocStart() : SourceLocation());

  CGF.EmitCXXMemberOrOperatorCall(Dtor, Callee, ReturnValueSlot(), This,
                                  /*ImplicitParam=*/nullptr, QualType(), CE);
  return nullptr;
}

void ItaniumCXXABI::emitVirtualInheritanceTables(const CXXRecordDecl *RD) {
  CodeGenVTables &VTables = CGM.getVTables();
  llvm37::GlobalVariable *VTT = VTables.GetAddrOfVTT(RD);
  VTables.EmitVTTDefinition(VTT, CGM.getVTableLinkage(RD), RD);
}

static llvm37::Value *performTypeAdjustment(CodeGenFunction &CGF,
                                          llvm37::Value *Ptr,
                                          int64_t NonVirtualAdjustment,
                                          int64_t VirtualAdjustment,
                                          bool IsReturnAdjustment) {
  if (!NonVirtualAdjustment && !VirtualAdjustment)
    return Ptr;

  llvm37::Type *Int8PtrTy = CGF.Int8PtrTy;
  llvm37::Value *V = CGF.Builder.CreateBitCast(Ptr, Int8PtrTy);

  if (NonVirtualAdjustment && !IsReturnAdjustment) {
    // Perform the non-virtual adjustment for a base-to-derived cast.
    V = CGF.Builder.CreateConstInBoundsGEP1_64(V, NonVirtualAdjustment);
  }

  if (VirtualAdjustment) {
    llvm37::Type *PtrDiffTy =
        CGF.ConvertType(CGF.getContext().getPointerDiffType());

    // Perform the virtual adjustment.
    llvm37::Value *VTablePtrPtr =
        CGF.Builder.CreateBitCast(V, Int8PtrTy->getPointerTo());

    llvm37::Value *VTablePtr = CGF.Builder.CreateLoad(VTablePtrPtr);

    llvm37::Value *OffsetPtr =
        CGF.Builder.CreateConstInBoundsGEP1_64(VTablePtr, VirtualAdjustment);

    OffsetPtr = CGF.Builder.CreateBitCast(OffsetPtr, PtrDiffTy->getPointerTo());

    // Load the adjustment offset from the vtable.
    llvm37::Value *Offset = CGF.Builder.CreateLoad(OffsetPtr);

    // Adjust our pointer.
    V = CGF.Builder.CreateInBoundsGEP(V, Offset);
  }

  if (NonVirtualAdjustment && IsReturnAdjustment) {
    // Perform the non-virtual adjustment for a derived-to-base cast.
    V = CGF.Builder.CreateConstInBoundsGEP1_64(V, NonVirtualAdjustment);
  }

  // Cast back to the original type.
  return CGF.Builder.CreateBitCast(V, Ptr->getType());
}

llvm37::Value *ItaniumCXXABI::performThisAdjustment(CodeGenFunction &CGF,
                                                  llvm37::Value *This,
                                                  const ThisAdjustment &TA) {
  return performTypeAdjustment(CGF, This, TA.NonVirtual,
                               TA.Virtual.Itanium.VCallOffsetOffset,
                               /*IsReturnAdjustment=*/false);
}

llvm37::Value *
ItaniumCXXABI::performReturnAdjustment(CodeGenFunction &CGF, llvm37::Value *Ret,
                                       const ReturnAdjustment &RA) {
  return performTypeAdjustment(CGF, Ret, RA.NonVirtual,
                               RA.Virtual.Itanium.VBaseOffsetOffset,
                               /*IsReturnAdjustment=*/true);
}

void ARMCXXABI::EmitReturnFromThunk(CodeGenFunction &CGF,
                                    RValue RV, QualType ResultType) {
  if (!isa<CXXDestructorDecl>(CGF.CurGD.getDecl()))
    return ItaniumCXXABI::EmitReturnFromThunk(CGF, RV, ResultType);

  // Destructor thunks in the ARM ABI have indeterminate results.
  llvm37::Type *T =
    cast<llvm37::PointerType>(CGF.ReturnValue->getType())->getElementType();
  RValue Undef = RValue::get(llvm37::UndefValue::get(T));
  return ItaniumCXXABI::EmitReturnFromThunk(CGF, Undef, ResultType);
}

/************************** Array allocation cookies **************************/

CharUnits ItaniumCXXABI::getArrayCookieSizeImpl(QualType elementType) {
  // The array cookie is a size_t; pad that up to the element alignment.
  // The cookie is actually right-justified in that space.
  return std::max(CharUnits::fromQuantity(CGM.SizeSizeInBytes),
                  CGM.getContext().getTypeAlignInChars(elementType));
}

llvm37::Value *ItaniumCXXABI::InitializeArrayCookie(CodeGenFunction &CGF,
                                                  llvm37::Value *NewPtr,
                                                  llvm37::Value *NumElements,
                                                  const CXXNewExpr *expr,
                                                  QualType ElementType) {
  assert(requiresArrayCookie(expr));

  unsigned AS = NewPtr->getType()->getPointerAddressSpace();

  ASTContext &Ctx = getContext();
  QualType SizeTy = Ctx.getSizeType();
  CharUnits SizeSize = Ctx.getTypeSizeInChars(SizeTy);

  // The size of the cookie.
  CharUnits CookieSize =
    std::max(SizeSize, Ctx.getTypeAlignInChars(ElementType));
  assert(CookieSize == getArrayCookieSizeImpl(ElementType));

  // Compute an offset to the cookie.
  llvm37::Value *CookiePtr = NewPtr;
  CharUnits CookieOffset = CookieSize - SizeSize;
  if (!CookieOffset.isZero())
    CookiePtr = CGF.Builder.CreateConstInBoundsGEP1_64(CookiePtr,
                                                 CookieOffset.getQuantity());

  // Write the number of elements into the appropriate slot.
  llvm37::Type *NumElementsTy = CGF.ConvertType(SizeTy)->getPointerTo(AS);
  llvm37::Value *NumElementsPtr =
      CGF.Builder.CreateBitCast(CookiePtr, NumElementsTy);
  llvm37::Instruction *SI = CGF.Builder.CreateStore(NumElements, NumElementsPtr);
  if (CGM.getLangOpts().Sanitize.has(SanitizerKind::Address) && AS == 0 &&
      expr->getOperatorNew()->isReplaceableGlobalAllocationFunction()) {
    // The store to the CookiePtr does not need to be instrumented.
    CGM.getSanitizerMetadata()->disableSanitizerForInstruction(SI);
    llvm37::FunctionType *FTy =
        llvm37::FunctionType::get(CGM.VoidTy, NumElementsTy, false);
    llvm37::Constant *F =
        CGM.CreateRuntimeFunction(FTy, "__asan_poison_cxx_array_cookie");
    CGF.Builder.CreateCall(F, NumElementsPtr);
  }

  // Finally, compute a pointer to the actual data buffer by skipping
  // over the cookie completely.
  return CGF.Builder.CreateConstInBoundsGEP1_64(NewPtr,
                                                CookieSize.getQuantity());  
}

llvm37::Value *ItaniumCXXABI::readArrayCookieImpl(CodeGenFunction &CGF,
                                                llvm37::Value *allocPtr,
                                                CharUnits cookieSize) {
  // The element size is right-justified in the cookie.
  llvm37::Value *numElementsPtr = allocPtr;
  CharUnits numElementsOffset =
    cookieSize - CharUnits::fromQuantity(CGF.SizeSizeInBytes);
  if (!numElementsOffset.isZero())
    numElementsPtr =
      CGF.Builder.CreateConstInBoundsGEP1_64(numElementsPtr,
                                             numElementsOffset.getQuantity());

  unsigned AS = allocPtr->getType()->getPointerAddressSpace();
  numElementsPtr = 
    CGF.Builder.CreateBitCast(numElementsPtr, CGF.SizeTy->getPointerTo(AS));
  if (!CGM.getLangOpts().Sanitize.has(SanitizerKind::Address) || AS != 0)
    return CGF.Builder.CreateLoad(numElementsPtr);
  // In asan mode emit a function call instead of a regular load and let the
  // run-time deal with it: if the shadow is properly poisoned return the
  // cookie, otherwise return 0 to avoid an infinite loop calling DTORs.
  // We can't simply ignore this load using nosanitize metadata because
  // the metadata may be lost.
  llvm37::FunctionType *FTy =
      llvm37::FunctionType::get(CGF.SizeTy, CGF.SizeTy->getPointerTo(0), false);
  llvm37::Constant *F =
      CGM.CreateRuntimeFunction(FTy, "__asan_load_cxx_array_cookie");
  return CGF.Builder.CreateCall(F, numElementsPtr);
}

CharUnits ARMCXXABI::getArrayCookieSizeImpl(QualType elementType) {
  // ARM says that the cookie is always:
  //   struct array_cookie {
  //     std::size_t element_size; // element_size != 0
  //     std::size_t element_count;
  //   };
  // But the base ABI doesn't give anything an alignment greater than
  // 8, so we can dismiss this as typical ABI-author blindness to
  // actual language complexity and round up to the element alignment.
  return std::max(CharUnits::fromQuantity(2 * CGM.SizeSizeInBytes),
                  CGM.getContext().getTypeAlignInChars(elementType));
}

llvm37::Value *ARMCXXABI::InitializeArrayCookie(CodeGenFunction &CGF,
                                              llvm37::Value *newPtr,
                                              llvm37::Value *numElements,
                                              const CXXNewExpr *expr,
                                              QualType elementType) {
  assert(requiresArrayCookie(expr));

  // NewPtr is a char*, but we generalize to arbitrary addrspaces.
  unsigned AS = newPtr->getType()->getPointerAddressSpace();

  // The cookie is always at the start of the buffer.
  llvm37::Value *cookie = newPtr;

  // The first element is the element size.
  cookie = CGF.Builder.CreateBitCast(cookie, CGF.SizeTy->getPointerTo(AS));
  llvm37::Value *elementSize = llvm37::ConstantInt::get(CGF.SizeTy,
                 getContext().getTypeSizeInChars(elementType).getQuantity());
  CGF.Builder.CreateStore(elementSize, cookie);

  // The second element is the element count.
  cookie = CGF.Builder.CreateConstInBoundsGEP1_32(CGF.SizeTy, cookie, 1);
  CGF.Builder.CreateStore(numElements, cookie);

  // Finally, compute a pointer to the actual data buffer by skipping
  // over the cookie completely.
  CharUnits cookieSize = ARMCXXABI::getArrayCookieSizeImpl(elementType);
  return CGF.Builder.CreateConstInBoundsGEP1_64(newPtr,
                                                cookieSize.getQuantity());
}

llvm37::Value *ARMCXXABI::readArrayCookieImpl(CodeGenFunction &CGF,
                                            llvm37::Value *allocPtr,
                                            CharUnits cookieSize) {
  // The number of elements is at offset sizeof(size_t) relative to
  // the allocated pointer.
  llvm37::Value *numElementsPtr
    = CGF.Builder.CreateConstInBoundsGEP1_64(allocPtr, CGF.SizeSizeInBytes);

  unsigned AS = allocPtr->getType()->getPointerAddressSpace();
  numElementsPtr = 
    CGF.Builder.CreateBitCast(numElementsPtr, CGF.SizeTy->getPointerTo(AS));
  return CGF.Builder.CreateLoad(numElementsPtr);
}

/*********************** Static local initialization **************************/

static llvm37::Constant *getGuardAcquireFn(CodeGenModule &CGM,
                                         llvm37::PointerType *GuardPtrTy) {
  // int __cxa_guard_acquire(__guard *guard_object);
  llvm37::FunctionType *FTy =
    llvm37::FunctionType::get(CGM.getTypes().ConvertType(CGM.getContext().IntTy),
                            GuardPtrTy, /*isVarArg=*/false);
  return CGM.CreateRuntimeFunction(FTy, "__cxa_guard_acquire",
                                   llvm37::AttributeSet::get(CGM.getLLVMContext(),
                                              llvm37::AttributeSet::FunctionIndex,
                                                 llvm37::Attribute::NoUnwind));
}

static llvm37::Constant *getGuardReleaseFn(CodeGenModule &CGM,
                                         llvm37::PointerType *GuardPtrTy) {
  // void __cxa_guard_release(__guard *guard_object);
  llvm37::FunctionType *FTy =
    llvm37::FunctionType::get(CGM.VoidTy, GuardPtrTy, /*isVarArg=*/false);
  return CGM.CreateRuntimeFunction(FTy, "__cxa_guard_release",
                                   llvm37::AttributeSet::get(CGM.getLLVMContext(),
                                              llvm37::AttributeSet::FunctionIndex,
                                                 llvm37::Attribute::NoUnwind));
}

static llvm37::Constant *getGuardAbortFn(CodeGenModule &CGM,
                                       llvm37::PointerType *GuardPtrTy) {
  // void __cxa_guard_abort(__guard *guard_object);
  llvm37::FunctionType *FTy =
    llvm37::FunctionType::get(CGM.VoidTy, GuardPtrTy, /*isVarArg=*/false);
  return CGM.CreateRuntimeFunction(FTy, "__cxa_guard_abort",
                                   llvm37::AttributeSet::get(CGM.getLLVMContext(),
                                              llvm37::AttributeSet::FunctionIndex,
                                                 llvm37::Attribute::NoUnwind));
}

namespace {
  struct CallGuardAbort : EHScopeStack::Cleanup {
    llvm37::GlobalVariable *Guard;
    CallGuardAbort(llvm37::GlobalVariable *Guard) : Guard(Guard) {}

    void Emit(CodeGenFunction &CGF, Flags flags) override {
      CGF.EmitNounwindRuntimeCall(getGuardAbortFn(CGF.CGM, Guard->getType()),
                                  Guard);
    }
  };
}

/// The ARM code here follows the Itanium code closely enough that we
/// just special-case it at particular places.
void ItaniumCXXABI::EmitGuardedInit(CodeGenFunction &CGF,
                                    const VarDecl &D,
                                    llvm37::GlobalVariable *var,
                                    bool shouldPerformInit) {
  CGBuilderTy &Builder = CGF.Builder;

  // We only need to use thread-safe statics for local non-TLS variables;
  // global initialization is always single-threaded.
  bool threadsafe = getContext().getLangOpts().ThreadsafeStatics &&
                    D.isLocalVarDecl() && !D.getTLSKind();

  // If we have a global variable with internal linkage and thread-safe statics
  // are disabled, we can just let the guard variable be of type i8.
  bool useInt8GuardVariable = !threadsafe && var->hasInternalLinkage();

  llvm37::IntegerType *guardTy;
  if (useInt8GuardVariable) {
    guardTy = CGF.Int8Ty;
  } else {
    // Guard variables are 64 bits in the generic ABI and size width on ARM
    // (i.e. 32-bit on AArch32, 64-bit on AArch64).
    guardTy = (UseARMGuardVarABI ? CGF.SizeTy : CGF.Int64Ty);
  }
  llvm37::PointerType *guardPtrTy = guardTy->getPointerTo();

  // Create the guard variable if we don't already have it (as we
  // might if we're double-emitting this function body).
  llvm37::GlobalVariable *guard = CGM.getStaticLocalDeclGuardAddress(&D);
  if (!guard) {
    // Mangle the name for the guard.
    SmallString<256> guardName;
    {
      llvm37::raw_svector_ostream out(guardName);
      getMangleContext().mangleStaticGuardVariable(&D, out);
      out.flush();
    }

    // Create the guard variable with a zero-initializer.
    // Just absorb linkage and visibility from the guarded variable.
    guard = new llvm37::GlobalVariable(CGM.getModule(), guardTy,
                                     false, var->getLinkage(),
                                     llvm37::ConstantInt::get(guardTy, 0),
                                     guardName.str());
    guard->setVisibility(var->getVisibility());
    // If the variable is thread-local, so is its guard variable.
    guard->setThreadLocalMode(var->getThreadLocalMode());

    // The ABI says: It is suggested that it be emitted in the same COMDAT group
    // as the associated data object
    llvm37::Comdat *C = var->getComdat();
    if (!D.isLocalVarDecl() && C) {
      guard->setComdat(C);
      CGF.CurFn->setComdat(C);
    } else if (CGM.supportsCOMDAT() && guard->isWeakForLinker()) {
      guard->setComdat(CGM.getModule().getOrInsertComdat(guard->getName()));
    }

    CGM.setStaticLocalDeclGuardAddress(&D, guard);
  }

  // Test whether the variable has completed initialization.
  //
  // Itanium C++ ABI 3.3.2:
  //   The following is pseudo-code showing how these functions can be used:
  //     if (obj_guard.first_byte == 0) {
  //       if ( __cxa_guard_acquire (&obj_guard) ) {
  //         try {
  //           ... initialize the object ...;
  //         } catch (...) {
  //            __cxa_guard_abort (&obj_guard);
  //            throw;
  //         }
  //         ... queue object destructor with __cxa_atexit() ...;
  //         __cxa_guard_release (&obj_guard);
  //       }
  //     }

  // Load the first byte of the guard variable.
  llvm37::LoadInst *LI =
      Builder.CreateLoad(Builder.CreateBitCast(guard, CGM.Int8PtrTy));
  LI->setAlignment(1);

  // Itanium ABI:
  //   An implementation supporting thread-safety on multiprocessor
  //   systems must also guarantee that references to the initialized
  //   object do not occur before the load of the initialization flag.
  //
  // In LLVM, we do this by marking the load Acquire.
  if (threadsafe)
    LI->setAtomic(llvm37::Acquire);

  // For ARM, we should only check the first bit, rather than the entire byte:
  //
  // ARM C++ ABI 3.2.3.1:
  //   To support the potential use of initialization guard variables
  //   as semaphores that are the target of ARM SWP and LDREX/STREX
  //   synchronizing instructions we define a static initialization
  //   guard variable to be a 4-byte aligned, 4-byte word with the
  //   following inline access protocol.
  //     #define INITIALIZED 1
  //     if ((obj_guard & INITIALIZED) != INITIALIZED) {
  //       if (__cxa_guard_acquire(&obj_guard))
  //         ...
  //     }
  //
  // and similarly for ARM64:
  //
  // ARM64 C++ ABI 3.2.2:
  //   This ABI instead only specifies the value bit 0 of the static guard
  //   variable; all other bits are platform defined. Bit 0 shall be 0 when the
  //   variable is not initialized and 1 when it is.
  llvm37::Value *V =
      (UseARMGuardVarABI && !useInt8GuardVariable)
          ? Builder.CreateAnd(LI, llvm37::ConstantInt::get(CGM.Int8Ty, 1))
          : LI;
  llvm37::Value *isInitialized = Builder.CreateIsNull(V, "guard.uninitialized");

  llvm37::BasicBlock *InitCheckBlock = CGF.createBasicBlock("init.check");
  llvm37::BasicBlock *EndBlock = CGF.createBasicBlock("init.end");

  // Check if the first byte of the guard variable is zero.
  Builder.CreateCondBr(isInitialized, InitCheckBlock, EndBlock);

  CGF.EmitBlock(InitCheckBlock);

  // Variables used when coping with thread-safe statics and exceptions.
  if (threadsafe) {    
    // Call __cxa_guard_acquire.
    llvm37::Value *V
      = CGF.EmitNounwindRuntimeCall(getGuardAcquireFn(CGM, guardPtrTy), guard);
               
    llvm37::BasicBlock *InitBlock = CGF.createBasicBlock("init");
  
    Builder.CreateCondBr(Builder.CreateIsNotNull(V, "tobool"),
                         InitBlock, EndBlock);
  
    // Call __cxa_guard_abort along the exceptional edge.
    CGF.EHStack.pushCleanup<CallGuardAbort>(EHCleanup, guard);
    
    CGF.EmitBlock(InitBlock);
  }

  // Emit the initializer and add a global destructor if appropriate.
  CGF.EmitCXXGlobalVarDeclInit(D, var, shouldPerformInit);

  if (threadsafe) {
    // Pop the guard-abort cleanup if we pushed one.
    CGF.PopCleanupBlock();

    // Call __cxa_guard_release.  This cannot throw.
    CGF.EmitNounwindRuntimeCall(getGuardReleaseFn(CGM, guardPtrTy), guard);
  } else {
    Builder.CreateStore(llvm37::ConstantInt::get(guardTy, 1), guard);
  }

  CGF.EmitBlock(EndBlock);
}

/// Register a global destructor using __cxa_atexit.
static void emitGlobalDtorWithCXAAtExit(CodeGenFunction &CGF,
                                        llvm37::Constant *dtor,
                                        llvm37::Constant *addr,
                                        bool TLS) {
  const char *Name = "__cxa_atexit";
  if (TLS) {
    const llvm37::Triple &T = CGF.getTarget().getTriple();
    Name = T.isMacOSX() ?  "_tlv_atexit" : "__cxa_thread_atexit";
  }

  // We're assuming that the destructor function is something we can
  // reasonably call with the default CC.  Go ahead and cast it to the
  // right prototype.
  llvm37::Type *dtorTy =
    llvm37::FunctionType::get(CGF.VoidTy, CGF.Int8PtrTy, false)->getPointerTo();

  // extern "C" int __cxa_atexit(void (*f)(void *), void *p, void *d);
  llvm37::Type *paramTys[] = { dtorTy, CGF.Int8PtrTy, CGF.Int8PtrTy };
  llvm37::FunctionType *atexitTy =
    llvm37::FunctionType::get(CGF.IntTy, paramTys, false);

  // Fetch the actual function.
  llvm37::Constant *atexit = CGF.CGM.CreateRuntimeFunction(atexitTy, Name);
  if (llvm37::Function *fn = dyn_cast<llvm37::Function>(atexit))
    fn->setDoesNotThrow();

  // Create a variable that binds the atexit to this shared object.
  llvm37::Constant *handle =
    CGF.CGM.CreateRuntimeVariable(CGF.Int8Ty, "__dso_handle");

  llvm37::Value *args[] = {
    llvm37::ConstantExpr::getBitCast(dtor, dtorTy),
    llvm37::ConstantExpr::getBitCast(addr, CGF.Int8PtrTy),
    handle
  };
  CGF.EmitNounwindRuntimeCall(atexit, args);
}

/// Register a global destructor as best as we know how.
void ItaniumCXXABI::registerGlobalDtor(CodeGenFunction &CGF,
                                       const VarDecl &D,
                                       llvm37::Constant *dtor,
                                       llvm37::Constant *addr) {
  // Use __cxa_atexit if available.
  if (CGM.getCodeGenOpts().CXAAtExit)
    return emitGlobalDtorWithCXAAtExit(CGF, dtor, addr, D.getTLSKind());

  if (D.getTLSKind())
    CGM.ErrorUnsupported(&D, "non-trivial TLS destruction");

  // In Apple kexts, we want to add a global destructor entry.
  // FIXME: shouldn't this be guarded by some variable?
  if (CGM.getLangOpts().AppleKext) {
    // Generate a global destructor entry.
    return CGM.AddCXXDtorEntry(dtor, addr);
  }

  CGF.registerGlobalDtorWithAtExit(D, dtor, addr);
}

static bool isThreadWrapperReplaceable(const VarDecl *VD,
                                       CodeGen::CodeGenModule &CGM) {
  assert(!VD->isStaticLocal() && "static local VarDecls don't need wrappers!");
  // OS X prefers to have references to thread local variables to go through
  // the thread wrapper instead of directly referencing the backing variable.
  return VD->getTLSKind() == VarDecl::TLS_Dynamic &&
         CGM.getTarget().getTriple().isMacOSX();
}

/// Get the appropriate linkage for the wrapper function. This is essentially
/// the weak form of the variable's linkage; every translation unit which needs
/// the wrapper emits a copy, and we want the linker to merge them.
static llvm37::GlobalValue::LinkageTypes
getThreadLocalWrapperLinkage(const VarDecl *VD, CodeGen::CodeGenModule &CGM) {
  llvm37::GlobalValue::LinkageTypes VarLinkage =
      CGM.getLLVMLinkageVarDefinition(VD, /*isConstant=*/false);

  // For internal linkage variables, we don't need an external or weak wrapper.
  if (llvm37::GlobalValue::isLocalLinkage(VarLinkage))
    return VarLinkage;

  // If the thread wrapper is replaceable, give it appropriate linkage.
  if (isThreadWrapperReplaceable(VD, CGM)) {
    if (llvm37::GlobalVariable::isLinkOnceLinkage(VarLinkage) ||
        llvm37::GlobalVariable::isWeakODRLinkage(VarLinkage))
      return llvm37::GlobalVariable::WeakAnyLinkage;
    return VarLinkage;
  }
  return llvm37::GlobalValue::WeakODRLinkage;
}

llvm37::Function *
ItaniumCXXABI::getOrCreateThreadLocalWrapper(const VarDecl *VD,
                                             llvm37::Value *Val) {
  // Mangle the name for the thread_local wrapper function.
  SmallString<256> WrapperName;
  {
    llvm37::raw_svector_ostream Out(WrapperName);
    getMangleContext().mangleItaniumThreadLocalWrapper(VD, Out);
    Out.flush();
  }

  if (llvm37::Value *V = CGM.getModule().getNamedValue(WrapperName))
    return cast<llvm37::Function>(V);

  llvm37::Type *RetTy = Val->getType();
  if (VD->getType()->isReferenceType())
    RetTy = RetTy->getPointerElementType();

  llvm37::FunctionType *FnTy = llvm37::FunctionType::get(RetTy, false);
  llvm37::Function *Wrapper =
      llvm37::Function::Create(FnTy, getThreadLocalWrapperLinkage(VD, CGM),
                             WrapperName.str(), &CGM.getModule());
  // Always resolve references to the wrapper at link time.
  if (!Wrapper->hasLocalLinkage() && !isThreadWrapperReplaceable(VD, CGM))
    Wrapper->setVisibility(llvm37::GlobalValue::HiddenVisibility);
  return Wrapper;
}

void ItaniumCXXABI::EmitThreadLocalInitFuncs(
    CodeGenModule &CGM,
    ArrayRef<std::pair<const VarDecl *, llvm37::GlobalVariable *>>
        CXXThreadLocals, ArrayRef<llvm37::Function *> CXXThreadLocalInits,
    ArrayRef<llvm37::GlobalVariable *> CXXThreadLocalInitVars) {
  llvm37::Function *InitFunc = nullptr;
  if (!CXXThreadLocalInits.empty()) {
    // Generate a guarded initialization function.
    llvm37::FunctionType *FTy =
        llvm37::FunctionType::get(CGM.VoidTy, /*isVarArg=*/false);
    InitFunc = CGM.CreateGlobalInitOrDestructFunction(FTy, "__tls_init",
                                                      SourceLocation(),
                                                      /*TLS=*/true);
    llvm37::GlobalVariable *Guard = new llvm37::GlobalVariable(
        CGM.getModule(), CGM.Int8Ty, /*isConstant=*/false,
        llvm37::GlobalVariable::InternalLinkage,
        llvm37::ConstantInt::get(CGM.Int8Ty, 0), "__tls_guard");
    Guard->setThreadLocal(true);
    CodeGenFunction(CGM)
        .GenerateCXXGlobalInitFunc(InitFunc, CXXThreadLocalInits, Guard);
  }
  for (unsigned I = 0, N = CXXThreadLocals.size(); I != N; ++I) {
    const VarDecl *VD = CXXThreadLocals[I].first;
    llvm37::GlobalVariable *Var = CXXThreadLocals[I].second;

    // Some targets require that all access to thread local variables go through
    // the thread wrapper.  This means that we cannot attempt to create a thread
    // wrapper or a thread helper.
    if (isThreadWrapperReplaceable(VD, CGM) && !VD->hasDefinition())
      continue;

    // Mangle the name for the thread_local initialization function.
    SmallString<256> InitFnName;
    {
      llvm37::raw_svector_ostream Out(InitFnName);
      getMangleContext().mangleItaniumThreadLocalInit(VD, Out);
      Out.flush();
    }

    // If we have a definition for the variable, emit the initialization
    // function as an alias to the global Init function (if any). Otherwise,
    // produce a declaration of the initialization function.
    llvm37::GlobalValue *Init = nullptr;
    bool InitIsInitFunc = false;
    if (VD->hasDefinition()) {
      InitIsInitFunc = true;
      if (InitFunc)
        Init = llvm37::GlobalAlias::create(Var->getLinkage(), InitFnName.str(),
                                         InitFunc);
    } else {
      // Emit a weak global function referring to the initialization function.
      // This function will not exist if the TU defining the thread_local
      // variable in question does not need any dynamic initialization for
      // its thread_local variables.
      llvm37::FunctionType *FnTy = llvm37::FunctionType::get(CGM.VoidTy, false);
      Init = llvm37::Function::Create(
          FnTy, llvm37::GlobalVariable::ExternalWeakLinkage, InitFnName.str(),
          &CGM.getModule());
    }

    if (Init)
      Init->setVisibility(Var->getVisibility());

    llvm37::Function *Wrapper = getOrCreateThreadLocalWrapper(VD, Var);
    llvm37::LLVMContext &Context = CGM.getModule().getContext();
    llvm37::BasicBlock *Entry = llvm37::BasicBlock::Create(Context, "", Wrapper);
    CGBuilderTy Builder(Entry);
    if (InitIsInitFunc) {
      if (Init)
        Builder.CreateCall(Init);
    } else {
      // Don't know whether we have an init function. Call it if it exists.
      llvm37::Value *Have = Builder.CreateIsNotNull(Init);
      llvm37::BasicBlock *InitBB = llvm37::BasicBlock::Create(Context, "", Wrapper);
      llvm37::BasicBlock *ExitBB = llvm37::BasicBlock::Create(Context, "", Wrapper);
      Builder.CreateCondBr(Have, InitBB, ExitBB);

      Builder.SetInsertPoint(InitBB);
      Builder.CreateCall(Init);
      Builder.CreateBr(ExitBB);

      Builder.SetInsertPoint(ExitBB);
    }

    // For a reference, the result of the wrapper function is a pointer to
    // the referenced object.
    llvm37::Value *Val = Var;
    if (VD->getType()->isReferenceType()) {
      llvm37::LoadInst *LI = Builder.CreateLoad(Val);
      LI->setAlignment(CGM.getContext().getDeclAlign(VD).getQuantity());
      Val = LI;
    }
    if (Val->getType() != Wrapper->getReturnType())
      Val = Builder.CreatePointerBitCastOrAddrSpaceCast(
          Val, Wrapper->getReturnType(), "");
    Builder.CreateRet(Val);
  }
}

LValue ItaniumCXXABI::EmitThreadLocalVarDeclLValue(CodeGenFunction &CGF,
                                                   const VarDecl *VD,
                                                   QualType LValType) {
  QualType T = VD->getType();
  llvm37::Type *Ty = CGF.getTypes().ConvertTypeForMem(T);
  llvm37::Value *Val = CGF.CGM.GetAddrOfGlobalVar(VD, Ty);
  llvm37::Function *Wrapper = getOrCreateThreadLocalWrapper(VD, Val);

  Val = CGF.Builder.CreateCall(Wrapper);

  LValue LV;
  if (VD->getType()->isReferenceType())
    LV = CGF.MakeNaturalAlignAddrLValue(Val, LValType);
  else
    LV = CGF.MakeAddrLValue(Val, LValType, CGF.getContext().getDeclAlign(VD));
  // FIXME: need setObjCGCLValueClass?
  return LV;
}

/// Return whether the given global decl needs a VTT parameter, which it does
/// if it's a base constructor or destructor with virtual bases.
bool ItaniumCXXABI::NeedsVTTParameter(GlobalDecl GD) {
  const CXXMethodDecl *MD = cast<CXXMethodDecl>(GD.getDecl());
  
  // We don't have any virtual bases, just return early.
  if (!MD->getParent()->getNumVBases())
    return false;
  
  // Check if we have a base constructor.
  if (isa<CXXConstructorDecl>(MD) && GD.getCtorType() == Ctor_Base)
    return true;

  // Check if we have a base destructor.
  if (isa<CXXDestructorDecl>(MD) && GD.getDtorType() == Dtor_Base)
    return true;
  
  return false;
}

namespace {
class ItaniumRTTIBuilder {
  CodeGenModule &CGM;  // Per-module state.
  llvm37::LLVMContext &VMContext;
  const ItaniumCXXABI &CXXABI;  // Per-module state.

  /// Fields - The fields of the RTTI descriptor currently being built.
  SmallVector<llvm37::Constant *, 16> Fields;

  /// GetAddrOfTypeName - Returns the mangled type name of the given type.
  llvm37::GlobalVariable *
  GetAddrOfTypeName(QualType Ty, llvm37::GlobalVariable::LinkageTypes Linkage);

  /// GetAddrOfExternalRTTIDescriptor - Returns the constant for the RTTI
  /// descriptor of the given type.
  llvm37::Constant *GetAddrOfExternalRTTIDescriptor(QualType Ty);

  /// BuildVTablePointer - Build the vtable pointer for the given type.
  void BuildVTablePointer(const Type *Ty);

  /// BuildSIClassTypeInfo - Build an abi::__si_class_type_info, used for single
  /// inheritance, according to the Itanium C++ ABI, 2.9.5p6b.
  void BuildSIClassTypeInfo(const CXXRecordDecl *RD);

  /// BuildVMIClassTypeInfo - Build an abi::__vmi_class_type_info, used for
  /// classes with bases that do not satisfy the abi::__si_class_type_info
  /// constraints, according ti the Itanium C++ ABI, 2.9.5p5c.
  void BuildVMIClassTypeInfo(const CXXRecordDecl *RD);

  /// BuildPointerTypeInfo - Build an abi::__pointer_type_info struct, used
  /// for pointer types.
  void BuildPointerTypeInfo(QualType PointeeTy);

  /// BuildObjCObjectTypeInfo - Build the appropriate kind of
  /// type_info for an object type.
  void BuildObjCObjectTypeInfo(const ObjCObjectType *Ty);

  /// BuildPointerToMemberTypeInfo - Build an abi::__pointer_to_member_type_info
  /// struct, used for member pointer types.
  void BuildPointerToMemberTypeInfo(const MemberPointerType *Ty);

public:
  ItaniumRTTIBuilder(const ItaniumCXXABI &ABI)
      : CGM(ABI.CGM), VMContext(CGM.getModule().getContext()), CXXABI(ABI) {}

  // Pointer type info flags.
  enum {
    /// PTI_Const - Type has const qualifier.
    PTI_Const = 0x1,

    /// PTI_Volatile - Type has volatile qualifier.
    PTI_Volatile = 0x2,

    /// PTI_Restrict - Type has restrict qualifier.
    PTI_Restrict = 0x4,

    /// PTI_Incomplete - Type is incomplete.
    PTI_Incomplete = 0x8,

    /// PTI_ContainingClassIncomplete - Containing class is incomplete.
    /// (in pointer to member).
    PTI_ContainingClassIncomplete = 0x10
  };

  // VMI type info flags.
  enum {
    /// VMI_NonDiamondRepeat - Class has non-diamond repeated inheritance.
    VMI_NonDiamondRepeat = 0x1,

    /// VMI_DiamondShaped - Class is diamond shaped.
    VMI_DiamondShaped = 0x2
  };

  // Base class type info flags.
  enum {
    /// BCTI_Virtual - Base class is virtual.
    BCTI_Virtual = 0x1,

    /// BCTI_Public - Base class is public.
    BCTI_Public = 0x2
  };

  /// BuildTypeInfo - Build the RTTI type info struct for the given type.
  ///
  /// \param Force - true to force the creation of this RTTI value
  llvm37::Constant *BuildTypeInfo(QualType Ty, bool Force = false);
};
}

llvm37::GlobalVariable *ItaniumRTTIBuilder::GetAddrOfTypeName(
    QualType Ty, llvm37::GlobalVariable::LinkageTypes Linkage) {
  SmallString<256> OutName;
  llvm37::raw_svector_ostream Out(OutName);
  CGM.getCXXABI().getMangleContext().mangleCXXRTTIName(Ty, Out);
  Out.flush();
  StringRef Name = OutName.str();

  // We know that the mangled name of the type starts at index 4 of the
  // mangled name of the typename, so we can just index into it in order to
  // get the mangled name of the type.
  llvm37::Constant *Init = llvm37::ConstantDataArray::getString(VMContext,
                                                            Name.substr(4));

  llvm37::GlobalVariable *GV =
    CGM.CreateOrReplaceCXXRuntimeVariable(Name, Init->getType(), Linkage);

  GV->setInitializer(Init);

  return GV;
}

llvm37::Constant *
ItaniumRTTIBuilder::GetAddrOfExternalRTTIDescriptor(QualType Ty) {
  // Mangle the RTTI name.
  SmallString<256> OutName;
  llvm37::raw_svector_ostream Out(OutName);
  CGM.getCXXABI().getMangleContext().mangleCXXRTTI(Ty, Out);
  Out.flush();
  StringRef Name = OutName.str();

  // Look for an existing global.
  llvm37::GlobalVariable *GV = CGM.getModule().getNamedGlobal(Name);

  if (!GV) {
    // Create a new global variable.
    GV = new llvm37::GlobalVariable(CGM.getModule(), CGM.Int8PtrTy,
                                  /*Constant=*/true,
                                  llvm37::GlobalValue::ExternalLinkage, nullptr,
                                  Name);
    if (const RecordType *RecordTy = dyn_cast<RecordType>(Ty)) {
      const CXXRecordDecl *RD = cast<CXXRecordDecl>(RecordTy->getDecl());
      if (RD->hasAttr<DLLImportAttr>())
        GV->setDLLStorageClass(llvm37::GlobalVariable::DLLImportStorageClass);
    }
  }

  return llvm37::ConstantExpr::getBitCast(GV, CGM.Int8PtrTy);
}

/// TypeInfoIsInStandardLibrary - Given a builtin type, returns whether the type
/// info for that type is defined in the standard library.
static bool TypeInfoIsInStandardLibrary(const BuiltinType *Ty) {
  // Itanium C++ ABI 2.9.2:
  //   Basic type information (e.g. for "int", "bool", etc.) will be kept in
  //   the run-time support library. Specifically, the run-time support
  //   library should contain type_info objects for the types X, X* and
  //   X const*, for every X in: void, std::nullptr_t, bool, wchar_t, char,
  //   unsigned char, signed char, short, unsigned short, int, unsigned int,
  //   long, unsigned long, long long, unsigned long long, float, double,
  //   long double, char16_t, char32_t, and the IEEE 754r decimal and
  //   half-precision floating point types.
  switch (Ty->getKind()) {
    case BuiltinType::Void:
    case BuiltinType::NullPtr:
    case BuiltinType::Bool:
    case BuiltinType::WChar_S:
    case BuiltinType::WChar_U:
    case BuiltinType::Char_U:
    case BuiltinType::Char_S:
    case BuiltinType::UChar:
    case BuiltinType::SChar:
    case BuiltinType::Short:
    case BuiltinType::UShort:
    case BuiltinType::Int:
    case BuiltinType::UInt:
    case BuiltinType::Long:
    case BuiltinType::ULong:
    case BuiltinType::LongLong:
    case BuiltinType::ULongLong:
    case BuiltinType::Half:
    case BuiltinType::Float:
    case BuiltinType::Double:
    case BuiltinType::LongDouble:
    case BuiltinType::Char16:
    case BuiltinType::Char32:
    case BuiltinType::Int128:
    case BuiltinType::UInt128:
    case BuiltinType::OCLImage1d:
    case BuiltinType::OCLImage1dArray:
    case BuiltinType::OCLImage1dBuffer:
    case BuiltinType::OCLImage2d:
    case BuiltinType::OCLImage2dArray:
    case BuiltinType::OCLImage3d:
    case BuiltinType::OCLSampler:
    case BuiltinType::OCLEvent:
      return true;

    case BuiltinType::Dependent:
#define BUILTIN_TYPE(Id, SingletonId)
#define PLACEHOLDER_TYPE(Id, SingletonId) \
    case BuiltinType::Id:
#include "clang/AST/BuiltinTypes.def"
      llvm_unreachable("asking for RRTI for a placeholder type!");

    case BuiltinType::ObjCId:
    case BuiltinType::ObjCClass:
    case BuiltinType::ObjCSel:
      llvm_unreachable("FIXME: Objective-C types are unsupported!");
    case BuiltinType::Min12Int:
    case BuiltinType::Min16Int:
    case BuiltinType::Min16UInt:
    case BuiltinType::LitInt:
    case BuiltinType::Min10Float:
    case BuiltinType::Min16Float:
    case BuiltinType::HalfFloat:
    case BuiltinType::LitFloat:
    case BuiltinType::Int8_4Packed:
    case BuiltinType::UInt8_4Packed:
      llvm_unreachable("FIXME: HLSL types are unsupported!");
      break;
  }

  llvm_unreachable("Invalid BuiltinType Kind!");
}

static bool TypeInfoIsInStandardLibrary(const PointerType *PointerTy) {
  QualType PointeeTy = PointerTy->getPointeeType();
  const BuiltinType *BuiltinTy = dyn_cast<BuiltinType>(PointeeTy);
  if (!BuiltinTy)
    return false;

  // Check the qualifiers.
  Qualifiers Quals = PointeeTy.getQualifiers();
  Quals.removeConst();

  if (!Quals.empty())
    return false;

  return TypeInfoIsInStandardLibrary(BuiltinTy);
}

/// IsStandardLibraryRTTIDescriptor - Returns whether the type
/// information for the given type exists in the standard library.
static bool IsStandardLibraryRTTIDescriptor(QualType Ty) {
  // Type info for builtin types is defined in the standard library.
  if (const BuiltinType *BuiltinTy = dyn_cast<BuiltinType>(Ty))
    return TypeInfoIsInStandardLibrary(BuiltinTy);

  // Type info for some pointer types to builtin types is defined in the
  // standard library.
  if (const PointerType *PointerTy = dyn_cast<PointerType>(Ty))
    return TypeInfoIsInStandardLibrary(PointerTy);

  return false;
}

/// ShouldUseExternalRTTIDescriptor - Returns whether the type information for
/// the given type exists somewhere else, and that we should not emit the type
/// information in this translation unit.  Assumes that it is not a
/// standard-library type.
static bool ShouldUseExternalRTTIDescriptor(CodeGenModule &CGM,
                                            QualType Ty) {
  ASTContext &Context = CGM.getContext();

  // If RTTI is disabled, assume it might be disabled in the
  // translation unit that defines any potential key function, too.
  if (!Context.getLangOpts().RTTI) return false;

  if (const RecordType *RecordTy = dyn_cast<RecordType>(Ty)) {
    const CXXRecordDecl *RD = cast<CXXRecordDecl>(RecordTy->getDecl());
    if (!RD->hasDefinition())
      return false;

    if (!RD->isDynamicClass())
      return false;

    // FIXME: this may need to be reconsidered if the key function
    // changes.
    // N.B. We must always emit the RTTI data ourselves if there exists a key
    // function.
    bool IsDLLImport = RD->hasAttr<DLLImportAttr>();
    if (CGM.getVTables().isVTableExternal(RD))
      return IsDLLImport ? false : true;

    if (IsDLLImport)
      return true;
  }

  return false;
}

/// IsIncompleteClassType - Returns whether the given record type is incomplete.
static bool IsIncompleteClassType(const RecordType *RecordTy) {
  return !RecordTy->getDecl()->isCompleteDefinition();
}

/// ContainsIncompleteClassType - Returns whether the given type contains an
/// incomplete class type. This is true if
///
///   * The given type is an incomplete class type.
///   * The given type is a pointer type whose pointee type contains an
///     incomplete class type.
///   * The given type is a member pointer type whose class is an incomplete
///     class type.
///   * The given type is a member pointer type whoise pointee type contains an
///     incomplete class type.
/// is an indirect or direct pointer to an incomplete class type.
static bool ContainsIncompleteClassType(QualType Ty) {
  if (const RecordType *RecordTy = dyn_cast<RecordType>(Ty)) {
    if (IsIncompleteClassType(RecordTy))
      return true;
  }

  if (const PointerType *PointerTy = dyn_cast<PointerType>(Ty))
    return ContainsIncompleteClassType(PointerTy->getPointeeType());

  if (const MemberPointerType *MemberPointerTy =
      dyn_cast<MemberPointerType>(Ty)) {
    // Check if the class type is incomplete.
    const RecordType *ClassType = cast<RecordType>(MemberPointerTy->getClass());
    if (IsIncompleteClassType(ClassType))
      return true;

    return ContainsIncompleteClassType(MemberPointerTy->getPointeeType());
  }

  return false;
}

// CanUseSingleInheritance - Return whether the given record decl has a "single,
// public, non-virtual base at offset zero (i.e. the derived class is dynamic
// iff the base is)", according to Itanium C++ ABI, 2.95p6b.
static bool CanUseSingleInheritance(const CXXRecordDecl *RD) {
  // Check the number of bases.
  if (RD->getNumBases() != 1)
    return false;

  // Get the base.
  CXXRecordDecl::base_class_const_iterator Base = RD->bases_begin();

  // Check that the base is not virtual.
  if (Base->isVirtual())
    return false;

  // Check that the base is public.
  if (Base->getAccessSpecifier() != AS_public)
    return false;

  // Check that the class is dynamic iff the base is.
  const CXXRecordDecl *BaseDecl =
    cast<CXXRecordDecl>(Base->getType()->getAs<RecordType>()->getDecl());
  if (!BaseDecl->isEmpty() &&
      BaseDecl->isDynamicClass() != RD->isDynamicClass())
    return false;

  return true;
}

void ItaniumRTTIBuilder::BuildVTablePointer(const Type *Ty) {
  // abi::__class_type_info.
  static const char * const ClassTypeInfo =
    "_ZTVN10__cxxabiv117__class_type_infoE";
  // abi::__si_class_type_info.
  static const char * const SIClassTypeInfo =
    "_ZTVN10__cxxabiv120__si_class_type_infoE";
  // abi::__vmi_class_type_info.
  static const char * const VMIClassTypeInfo =
    "_ZTVN10__cxxabiv121__vmi_class_type_infoE";

  const char *VTableName = nullptr;

  switch (Ty->getTypeClass()) {
#define TYPE(Class, Base)
#define ABSTRACT_TYPE(Class, Base)
#define NON_CANONICAL_UNLESS_DEPENDENT_TYPE(Class, Base) case Type::Class:
#define NON_CANONICAL_TYPE(Class, Base) case Type::Class:
#define DEPENDENT_TYPE(Class, Base) case Type::Class:
#include "clang/AST/TypeNodes.def"
    llvm_unreachable("Non-canonical and dependent types shouldn't get here");

  case Type::LValueReference:
  case Type::RValueReference:
    llvm_unreachable("References shouldn't get here");

  case Type::Auto:
    llvm_unreachable("Undeduced auto type shouldn't get here");

  case Type::Builtin:
  // GCC treats vector and complex types as fundamental types.
  case Type::Vector:
  case Type::ExtVector:
  case Type::Complex:
  case Type::Atomic:
  // FIXME: GCC treats block pointers as fundamental types?!
  case Type::BlockPointer:
    // abi::__fundamental_type_info.
    VTableName = "_ZTVN10__cxxabiv123__fundamental_type_infoE";
    break;

  case Type::ConstantArray:
  case Type::IncompleteArray:
  case Type::VariableArray:
    // abi::__array_type_info.
    VTableName = "_ZTVN10__cxxabiv117__array_type_infoE";
    break;

  case Type::FunctionNoProto:
  case Type::FunctionProto:
    // abi::__function_type_info.
    VTableName = "_ZTVN10__cxxabiv120__function_type_infoE";
    break;

  case Type::Enum:
    // abi::__enum_type_info.
    VTableName = "_ZTVN10__cxxabiv116__enum_type_infoE";
    break;

  case Type::Record: {
    const CXXRecordDecl *RD =
      cast<CXXRecordDecl>(cast<RecordType>(Ty)->getDecl());

    if (!RD->hasDefinition() || !RD->getNumBases()) {
      VTableName = ClassTypeInfo;
    } else if (CanUseSingleInheritance(RD)) {
      VTableName = SIClassTypeInfo;
    } else {
      VTableName = VMIClassTypeInfo;
    }

    break;
  }

  case Type::ObjCObject:
    // Ignore protocol qualifiers.
    Ty = cast<ObjCObjectType>(Ty)->getBaseType().getTypePtr();

    // Handle id and Class.
    if (isa<BuiltinType>(Ty)) {
      VTableName = ClassTypeInfo;
      break;
    }

    assert(isa<ObjCInterfaceType>(Ty));
    // Fall through.

  case Type::ObjCInterface:
    if (cast<ObjCInterfaceType>(Ty)->getDecl()->getSuperClass()) {
      VTableName = SIClassTypeInfo;
    } else {
      VTableName = ClassTypeInfo;
    }
    break;

  case Type::ObjCObjectPointer:
  case Type::Pointer:
    // abi::__pointer_type_info.
    VTableName = "_ZTVN10__cxxabiv119__pointer_type_infoE";
    break;

  case Type::MemberPointer:
    // abi::__pointer_to_member_type_info.
    VTableName = "_ZTVN10__cxxabiv129__pointer_to_member_type_infoE";
    break;
  }

  llvm37::Constant *VTable =
    CGM.getModule().getOrInsertGlobal(VTableName, CGM.Int8PtrTy);

  llvm37::Type *PtrDiffTy =
    CGM.getTypes().ConvertType(CGM.getContext().getPointerDiffType());

  // The vtable address point is 2.
  llvm37::Constant *Two = llvm37::ConstantInt::get(PtrDiffTy, 2);
  VTable =
      llvm37::ConstantExpr::getInBoundsGetElementPtr(CGM.Int8PtrTy, VTable, Two);
  VTable = llvm37::ConstantExpr::getBitCast(VTable, CGM.Int8PtrTy);

  Fields.push_back(VTable);
}

/// \brief Return the linkage that the type info and type info name constants
/// should have for the given type.
static llvm37::GlobalVariable::LinkageTypes getTypeInfoLinkage(CodeGenModule &CGM,
                                                             QualType Ty) {
  // Itanium C++ ABI 2.9.5p7:
  //   In addition, it and all of the intermediate abi::__pointer_type_info
  //   structs in the chain down to the abi::__class_type_info for the
  //   incomplete class type must be prevented from resolving to the
  //   corresponding type_info structs for the complete class type, possibly
  //   by making them local static objects. Finally, a dummy class RTTI is
  //   generated for the incomplete type that will not resolve to the final
  //   complete class RTTI (because the latter need not exist), possibly by
  //   making it a local static object.
  if (ContainsIncompleteClassType(Ty))
    return llvm37::GlobalValue::InternalLinkage;

  switch (Ty->getLinkage()) {
  case NoLinkage:
  case InternalLinkage:
  case UniqueExternalLinkage:
    return llvm37::GlobalValue::InternalLinkage;

  case VisibleNoLinkage:
  case ExternalLinkage:
    if (!CGM.getLangOpts().RTTI) {
      // RTTI is not enabled, which means that this type info struct is going
      // to be used for exception handling. Give it linkonce_odr linkage.
      return llvm37::GlobalValue::LinkOnceODRLinkage;
    }

    if (const RecordType *Record = dyn_cast<RecordType>(Ty)) {
      const CXXRecordDecl *RD = cast<CXXRecordDecl>(Record->getDecl());
      if (RD->hasAttr<WeakAttr>())
        return llvm37::GlobalValue::WeakODRLinkage;
      if (RD->isDynamicClass()) {
        llvm37::GlobalValue::LinkageTypes LT = CGM.getVTableLinkage(RD);
        // MinGW won't export the RTTI information when there is a key function.
        // Make sure we emit our own copy instead of attempting to dllimport it.
        if (RD->hasAttr<DLLImportAttr>() &&
            llvm37::GlobalValue::isAvailableExternallyLinkage(LT))
          LT = llvm37::GlobalValue::LinkOnceODRLinkage;
        return LT;
      }
    }

    return llvm37::GlobalValue::LinkOnceODRLinkage;
  }

  llvm_unreachable("Invalid linkage!");
}

llvm37::Constant *ItaniumRTTIBuilder::BuildTypeInfo(QualType Ty, bool Force) {
  // We want to operate on the canonical type.
  Ty = CGM.getContext().getCanonicalType(Ty);

  // Check if we've already emitted an RTTI descriptor for this type.
  SmallString<256> OutName;
  llvm37::raw_svector_ostream Out(OutName);
  CGM.getCXXABI().getMangleContext().mangleCXXRTTI(Ty, Out);
  Out.flush();
  StringRef Name = OutName.str();

  llvm37::GlobalVariable *OldGV = CGM.getModule().getNamedGlobal(Name);
  if (OldGV && !OldGV->isDeclaration()) {
    assert(!OldGV->hasAvailableExternallyLinkage() &&
           "available_externally typeinfos not yet implemented");

    return llvm37::ConstantExpr::getBitCast(OldGV, CGM.Int8PtrTy);
  }

  // Check if there is already an external RTTI descriptor for this type.
  bool IsStdLib = IsStandardLibraryRTTIDescriptor(Ty);
  if (!Force && (IsStdLib || ShouldUseExternalRTTIDescriptor(CGM, Ty)))
    return GetAddrOfExternalRTTIDescriptor(Ty);

  // Emit the standard library with external linkage.
  llvm37::GlobalVariable::LinkageTypes Linkage;
  if (IsStdLib)
    Linkage = llvm37::GlobalValue::ExternalLinkage;
  else
    Linkage = getTypeInfoLinkage(CGM, Ty);

  // Add the vtable pointer.
  BuildVTablePointer(cast<Type>(Ty));

  // And the name.
  llvm37::GlobalVariable *TypeName = GetAddrOfTypeName(Ty, Linkage);
  llvm37::Constant *TypeNameField;

  // If we're supposed to demote the visibility, be sure to set a flag
  // to use a string comparison for type_info comparisons.
  ItaniumCXXABI::RTTIUniquenessKind RTTIUniqueness =
      CXXABI.classifyRTTIUniqueness(Ty, Linkage);
  if (RTTIUniqueness != ItaniumCXXABI::RUK_Unique) {
    // The flag is the sign bit, which on ARM64 is defined to be clear
    // for global pointers.  This is very ARM64-specific.
    TypeNameField = llvm37::ConstantExpr::getPtrToInt(TypeName, CGM.Int64Ty);
    llvm37::Constant *flag =
        llvm37::ConstantInt::get(CGM.Int64Ty, ((uint64_t)1) << 63);
    TypeNameField = llvm37::ConstantExpr::getAdd(TypeNameField, flag);
    TypeNameField =
        llvm37::ConstantExpr::getIntToPtr(TypeNameField, CGM.Int8PtrTy);
  } else {
    TypeNameField = llvm37::ConstantExpr::getBitCast(TypeName, CGM.Int8PtrTy);
  }
  Fields.push_back(TypeNameField);

  switch (Ty->getTypeClass()) {
#define TYPE(Class, Base)
#define ABSTRACT_TYPE(Class, Base)
#define NON_CANONICAL_UNLESS_DEPENDENT_TYPE(Class, Base) case Type::Class:
#define NON_CANONICAL_TYPE(Class, Base) case Type::Class:
#define DEPENDENT_TYPE(Class, Base) case Type::Class:
#include "clang/AST/TypeNodes.def"
    llvm_unreachable("Non-canonical and dependent types shouldn't get here");

  // GCC treats vector types as fundamental types.
  case Type::Builtin:
  case Type::Vector:
  case Type::ExtVector:
  case Type::Complex:
  case Type::BlockPointer:
    // Itanium C++ ABI 2.9.5p4:
    // abi::__fundamental_type_info adds no data members to std::type_info.
    break;

  case Type::LValueReference:
  case Type::RValueReference:
    llvm_unreachable("References shouldn't get here");

  case Type::Auto:
    llvm_unreachable("Undeduced auto type shouldn't get here");

  case Type::ConstantArray:
  case Type::IncompleteArray:
  case Type::VariableArray:
    // Itanium C++ ABI 2.9.5p5:
    // abi::__array_type_info adds no data members to std::type_info.
    break;

  case Type::FunctionNoProto:
  case Type::FunctionProto:
    // Itanium C++ ABI 2.9.5p5:
    // abi::__function_type_info adds no data members to std::type_info.
    break;

  case Type::Enum:
    // Itanium C++ ABI 2.9.5p5:
    // abi::__enum_type_info adds no data members to std::type_info.
    break;

  case Type::Record: {
    const CXXRecordDecl *RD =
      cast<CXXRecordDecl>(cast<RecordType>(Ty)->getDecl());
    if (!RD->hasDefinition() || !RD->getNumBases()) {
      // We don't need to emit any fields.
      break;
    }

    if (CanUseSingleInheritance(RD))
      BuildSIClassTypeInfo(RD);
    else
      BuildVMIClassTypeInfo(RD);

    break;
  }

  case Type::ObjCObject:
  case Type::ObjCInterface:
    BuildObjCObjectTypeInfo(cast<ObjCObjectType>(Ty));
    break;

  case Type::ObjCObjectPointer:
    BuildPointerTypeInfo(cast<ObjCObjectPointerType>(Ty)->getPointeeType());
    break;

  case Type::Pointer:
    BuildPointerTypeInfo(cast<PointerType>(Ty)->getPointeeType());
    break;

  case Type::MemberPointer:
    BuildPointerToMemberTypeInfo(cast<MemberPointerType>(Ty));
    break;

  case Type::Atomic:
    // No fields, at least for the moment.
    break;
  }

  llvm37::Constant *Init = llvm37::ConstantStruct::getAnon(Fields);

  llvm37::Module &M = CGM.getModule();
  llvm37::GlobalVariable *GV =
      new llvm37::GlobalVariable(M, Init->getType(),
                               /*Constant=*/true, Linkage, Init, Name);

  if (CGM.supportsCOMDAT() && GV->isWeakForLinker())
    GV->setComdat(M.getOrInsertComdat(GV->getName()));

  // If there's already an old global variable, replace it with the new one.
  if (OldGV) {
    GV->takeName(OldGV);
    llvm37::Constant *NewPtr =
      llvm37::ConstantExpr::getBitCast(GV, OldGV->getType());
    OldGV->replaceAllUsesWith(NewPtr);
    OldGV->eraseFromParent();
  }

  // The Itanium ABI specifies that type_info objects must be globally
  // unique, with one exception: if the type is an incomplete class
  // type or a (possibly indirect) pointer to one.  That exception
  // affects the general case of comparing type_info objects produced
  // by the typeid operator, which is why the comparison operators on
  // std::type_info generally use the type_info name pointers instead
  // of the object addresses.  However, the language's built-in uses
  // of RTTI generally require class types to be complete, even when
  // manipulating pointers to those class types.  This allows the
  // implementation of dynamic_cast to rely on address equality tests,
  // which is much faster.

  // All of this is to say that it's important that both the type_info
  // object and the type_info name be uniqued when weakly emitted.

  // Give the type_info object and name the formal visibility of the
  // type itself.
  llvm37::GlobalValue::VisibilityTypes llvmVisibility;
  if (llvm37::GlobalValue::isLocalLinkage(Linkage))
    // If the linkage is local, only default visibility makes sense.
    llvmVisibility = llvm37::GlobalValue::DefaultVisibility;
  else if (RTTIUniqueness == ItaniumCXXABI::RUK_NonUniqueHidden)
    llvmVisibility = llvm37::GlobalValue::HiddenVisibility;
  else
    llvmVisibility = CodeGenModule::GetLLVMVisibility(Ty->getVisibility());
  TypeName->setVisibility(llvmVisibility);
  GV->setVisibility(llvmVisibility);

  return llvm37::ConstantExpr::getBitCast(GV, CGM.Int8PtrTy);
}

/// ComputeQualifierFlags - Compute the pointer type info flags from the
/// given qualifier.
static unsigned ComputeQualifierFlags(Qualifiers Quals) {
  unsigned Flags = 0;

  if (Quals.hasConst())
    Flags |= ItaniumRTTIBuilder::PTI_Const;
  if (Quals.hasVolatile())
    Flags |= ItaniumRTTIBuilder::PTI_Volatile;
  if (Quals.hasRestrict())
    Flags |= ItaniumRTTIBuilder::PTI_Restrict;

  return Flags;
}

/// BuildObjCObjectTypeInfo - Build the appropriate kind of type_info
/// for the given Objective-C object type.
void ItaniumRTTIBuilder::BuildObjCObjectTypeInfo(const ObjCObjectType *OT) {
  // Drop qualifiers.
  const Type *T = OT->getBaseType().getTypePtr();
  assert(isa<BuiltinType>(T) || isa<ObjCInterfaceType>(T));

  // The builtin types are abi::__class_type_infos and don't require
  // extra fields.
  if (isa<BuiltinType>(T)) return;

  ObjCInterfaceDecl *Class = cast<ObjCInterfaceType>(T)->getDecl();
  ObjCInterfaceDecl *Super = Class->getSuperClass();

  // Root classes are also __class_type_info.
  if (!Super) return;

  QualType SuperTy = CGM.getContext().getObjCInterfaceType(Super);

  // Everything else is single inheritance.
  llvm37::Constant *BaseTypeInfo =
      ItaniumRTTIBuilder(CXXABI).BuildTypeInfo(SuperTy);
  Fields.push_back(BaseTypeInfo);
}

/// BuildSIClassTypeInfo - Build an abi::__si_class_type_info, used for single
/// inheritance, according to the Itanium C++ ABI, 2.95p6b.
void ItaniumRTTIBuilder::BuildSIClassTypeInfo(const CXXRecordDecl *RD) {
  // Itanium C++ ABI 2.9.5p6b:
  // It adds to abi::__class_type_info a single member pointing to the
  // type_info structure for the base type,
  llvm37::Constant *BaseTypeInfo =
    ItaniumRTTIBuilder(CXXABI).BuildTypeInfo(RD->bases_begin()->getType());
  Fields.push_back(BaseTypeInfo);
}

namespace {
  /// SeenBases - Contains virtual and non-virtual bases seen when traversing
  /// a class hierarchy.
  struct SeenBases {
    llvm37::SmallPtrSet<const CXXRecordDecl *, 16> NonVirtualBases;
    llvm37::SmallPtrSet<const CXXRecordDecl *, 16> VirtualBases;
  };
}

/// ComputeVMIClassTypeInfoFlags - Compute the value of the flags member in
/// abi::__vmi_class_type_info.
///
static unsigned ComputeVMIClassTypeInfoFlags(const CXXBaseSpecifier *Base,
                                             SeenBases &Bases) {

  unsigned Flags = 0;

  const CXXRecordDecl *BaseDecl =
    cast<CXXRecordDecl>(Base->getType()->getAs<RecordType>()->getDecl());

  if (Base->isVirtual()) {
    // Mark the virtual base as seen.
    if (!Bases.VirtualBases.insert(BaseDecl).second) {
      // If this virtual base has been seen before, then the class is diamond
      // shaped.
      Flags |= ItaniumRTTIBuilder::VMI_DiamondShaped;
    } else {
      if (Bases.NonVirtualBases.count(BaseDecl))
        Flags |= ItaniumRTTIBuilder::VMI_NonDiamondRepeat;
    }
  } else {
    // Mark the non-virtual base as seen.
    if (!Bases.NonVirtualBases.insert(BaseDecl).second) {
      // If this non-virtual base has been seen before, then the class has non-
      // diamond shaped repeated inheritance.
      Flags |= ItaniumRTTIBuilder::VMI_NonDiamondRepeat;
    } else {
      if (Bases.VirtualBases.count(BaseDecl))
        Flags |= ItaniumRTTIBuilder::VMI_NonDiamondRepeat;
    }
  }

  // Walk all bases.
  for (const auto &I : BaseDecl->bases())
    Flags |= ComputeVMIClassTypeInfoFlags(&I, Bases);

  return Flags;
}

static unsigned ComputeVMIClassTypeInfoFlags(const CXXRecordDecl *RD) {
  unsigned Flags = 0;
  SeenBases Bases;

  // Walk all bases.
  for (const auto &I : RD->bases())
    Flags |= ComputeVMIClassTypeInfoFlags(&I, Bases);

  return Flags;
}

/// BuildVMIClassTypeInfo - Build an abi::__vmi_class_type_info, used for
/// classes with bases that do not satisfy the abi::__si_class_type_info
/// constraints, according ti the Itanium C++ ABI, 2.9.5p5c.
void ItaniumRTTIBuilder::BuildVMIClassTypeInfo(const CXXRecordDecl *RD) {
  llvm37::Type *UnsignedIntLTy =
    CGM.getTypes().ConvertType(CGM.getContext().UnsignedIntTy);

  // Itanium C++ ABI 2.9.5p6c:
  //   __flags is a word with flags describing details about the class
  //   structure, which may be referenced by using the __flags_masks
  //   enumeration. These flags refer to both direct and indirect bases.
  unsigned Flags = ComputeVMIClassTypeInfoFlags(RD);
  Fields.push_back(llvm37::ConstantInt::get(UnsignedIntLTy, Flags));

  // Itanium C++ ABI 2.9.5p6c:
  //   __base_count is a word with the number of direct proper base class
  //   descriptions that follow.
  Fields.push_back(llvm37::ConstantInt::get(UnsignedIntLTy, RD->getNumBases()));

  if (!RD->getNumBases())
    return;

  llvm37::Type *LongLTy =
    CGM.getTypes().ConvertType(CGM.getContext().LongTy);

  // Now add the base class descriptions.

  // Itanium C++ ABI 2.9.5p6c:
  //   __base_info[] is an array of base class descriptions -- one for every
  //   direct proper base. Each description is of the type:
  //
  //   struct abi::__base_class_type_info {
  //   public:
  //     const __class_type_info *__base_type;
  //     long __offset_flags;
  //
  //     enum __offset_flags_masks {
  //       __virtual_mask = 0x1,
  //       __public_mask = 0x2,
  //       __offset_shift = 8
  //     };
  //   };
  for (const auto &Base : RD->bases()) {
    // The __base_type member points to the RTTI for the base type.
    Fields.push_back(ItaniumRTTIBuilder(CXXABI).BuildTypeInfo(Base.getType()));

    const CXXRecordDecl *BaseDecl =
      cast<CXXRecordDecl>(Base.getType()->getAs<RecordType>()->getDecl());

    int64_t OffsetFlags = 0;

    // All but the lower 8 bits of __offset_flags are a signed offset.
    // For a non-virtual base, this is the offset in the object of the base
    // subobject. For a virtual base, this is the offset in the virtual table of
    // the virtual base offset for the virtual base referenced (negative).
    CharUnits Offset;
    if (Base.isVirtual())
      Offset =
        CGM.getItaniumVTableContext().getVirtualBaseOffsetOffset(RD, BaseDecl);
    else {
      const ASTRecordLayout &Layout = CGM.getContext().getASTRecordLayout(RD);
      Offset = Layout.getBaseClassOffset(BaseDecl);
    };

    OffsetFlags = uint64_t(Offset.getQuantity()) << 8;

    // The low-order byte of __offset_flags contains flags, as given by the
    // masks from the enumeration __offset_flags_masks.
    if (Base.isVirtual())
      OffsetFlags |= BCTI_Virtual;
    if (Base.getAccessSpecifier() == AS_public)
      OffsetFlags |= BCTI_Public;

    Fields.push_back(llvm37::ConstantInt::get(LongLTy, OffsetFlags));
  }
}

/// BuildPointerTypeInfo - Build an abi::__pointer_type_info struct,
/// used for pointer types.
void ItaniumRTTIBuilder::BuildPointerTypeInfo(QualType PointeeTy) {
  Qualifiers Quals;
  QualType UnqualifiedPointeeTy =
    CGM.getContext().getUnqualifiedArrayType(PointeeTy, Quals);

  // Itanium C++ ABI 2.9.5p7:
  //   __flags is a flag word describing the cv-qualification and other
  //   attributes of the type pointed to
  unsigned Flags = ComputeQualifierFlags(Quals);

  // Itanium C++ ABI 2.9.5p7:
  //   When the abi::__pbase_type_info is for a direct or indirect pointer to an
  //   incomplete class type, the incomplete target type flag is set.
  if (ContainsIncompleteClassType(UnqualifiedPointeeTy))
    Flags |= PTI_Incomplete;

  llvm37::Type *UnsignedIntLTy =
    CGM.getTypes().ConvertType(CGM.getContext().UnsignedIntTy);
  Fields.push_back(llvm37::ConstantInt::get(UnsignedIntLTy, Flags));

  // Itanium C++ ABI 2.9.5p7:
  //  __pointee is a pointer to the std::type_info derivation for the
  //  unqualified type being pointed to.
  llvm37::Constant *PointeeTypeInfo =
    ItaniumRTTIBuilder(CXXABI).BuildTypeInfo(UnqualifiedPointeeTy);
  Fields.push_back(PointeeTypeInfo);
}

/// BuildPointerToMemberTypeInfo - Build an abi::__pointer_to_member_type_info
/// struct, used for member pointer types.
void
ItaniumRTTIBuilder::BuildPointerToMemberTypeInfo(const MemberPointerType *Ty) {
  QualType PointeeTy = Ty->getPointeeType();

  Qualifiers Quals;
  QualType UnqualifiedPointeeTy =
    CGM.getContext().getUnqualifiedArrayType(PointeeTy, Quals);

  // Itanium C++ ABI 2.9.5p7:
  //   __flags is a flag word describing the cv-qualification and other
  //   attributes of the type pointed to.
  unsigned Flags = ComputeQualifierFlags(Quals);

  const RecordType *ClassType = cast<RecordType>(Ty->getClass());

  // Itanium C++ ABI 2.9.5p7:
  //   When the abi::__pbase_type_info is for a direct or indirect pointer to an
  //   incomplete class type, the incomplete target type flag is set.
  if (ContainsIncompleteClassType(UnqualifiedPointeeTy))
    Flags |= PTI_Incomplete;

  if (IsIncompleteClassType(ClassType))
    Flags |= PTI_ContainingClassIncomplete;

  llvm37::Type *UnsignedIntLTy =
    CGM.getTypes().ConvertType(CGM.getContext().UnsignedIntTy);
  Fields.push_back(llvm37::ConstantInt::get(UnsignedIntLTy, Flags));

  // Itanium C++ ABI 2.9.5p7:
  //   __pointee is a pointer to the std::type_info derivation for the
  //   unqualified type being pointed to.
  llvm37::Constant *PointeeTypeInfo =
    ItaniumRTTIBuilder(CXXABI).BuildTypeInfo(UnqualifiedPointeeTy);
  Fields.push_back(PointeeTypeInfo);

  // Itanium C++ ABI 2.9.5p9:
  //   __context is a pointer to an abi::__class_type_info corresponding to the
  //   class type containing the member pointed to
  //   (e.g., the "A" in "int A::*").
  Fields.push_back(
      ItaniumRTTIBuilder(CXXABI).BuildTypeInfo(QualType(ClassType, 0)));
}

llvm37::Constant *ItaniumCXXABI::getAddrOfRTTIDescriptor(QualType Ty) {
  return ItaniumRTTIBuilder(*this).BuildTypeInfo(Ty);
}

void ItaniumCXXABI::EmitFundamentalRTTIDescriptor(QualType Type) {
  QualType PointerType = getContext().getPointerType(Type);
  QualType PointerTypeConst = getContext().getPointerType(Type.withConst());
  ItaniumRTTIBuilder(*this).BuildTypeInfo(Type, true);
  ItaniumRTTIBuilder(*this).BuildTypeInfo(PointerType, true);
  ItaniumRTTIBuilder(*this).BuildTypeInfo(PointerTypeConst, true);
}

void ItaniumCXXABI::EmitFundamentalRTTIDescriptors() {
  QualType FundamentalTypes[] = {
      getContext().VoidTy,             getContext().NullPtrTy,
      getContext().BoolTy,             getContext().WCharTy,
      getContext().CharTy,             getContext().UnsignedCharTy,
      getContext().SignedCharTy,       getContext().ShortTy,
      getContext().UnsignedShortTy,    getContext().IntTy,
      getContext().UnsignedIntTy,      getContext().LongTy,
      getContext().UnsignedLongTy,     getContext().LongLongTy,
      getContext().UnsignedLongLongTy, getContext().HalfTy,
      getContext().FloatTy,            getContext().DoubleTy,
      getContext().LongDoubleTy,       getContext().Char16Ty,
      getContext().Char32Ty,
  };
  for (const QualType &FundamentalType : FundamentalTypes)
    EmitFundamentalRTTIDescriptor(FundamentalType);
}

/// What sort of uniqueness rules should we use for the RTTI for the
/// given type?
ItaniumCXXABI::RTTIUniquenessKind ItaniumCXXABI::classifyRTTIUniqueness(
    QualType CanTy, llvm37::GlobalValue::LinkageTypes Linkage) const {
  if (shouldRTTIBeUnique())
    return RUK_Unique;

  // It's only necessary for linkonce_odr or weak_odr linkage.
  if (Linkage != llvm37::GlobalValue::LinkOnceODRLinkage &&
      Linkage != llvm37::GlobalValue::WeakODRLinkage)
    return RUK_Unique;

  // It's only necessary with default visibility.
  if (CanTy->getVisibility() != DefaultVisibility)
    return RUK_Unique;

  // If we're not required to publish this symbol, hide it.
  if (Linkage == llvm37::GlobalValue::LinkOnceODRLinkage)
    return RUK_NonUniqueHidden;

  // If we're required to publish this symbol, as we might be under an
  // explicit instantiation, leave it with default visibility but
  // enable string-comparisons.
  assert(Linkage == llvm37::GlobalValue::WeakODRLinkage);
  return RUK_NonUniqueVisible;
}

// Find out how to codegen the complete destructor and constructor
namespace {
enum class StructorCodegen { Emit, RAUW, Alias, COMDAT };
}
static StructorCodegen getCodegenToUse(CodeGenModule &CGM,
                                       const CXXMethodDecl *MD) {
  if (!CGM.getCodeGenOpts().CXXCtorDtorAliases)
    return StructorCodegen::Emit;

  // The complete and base structors are not equivalent if there are any virtual
  // bases, so emit separate functions.
  if (MD->getParent()->getNumVBases())
    return StructorCodegen::Emit;

  GlobalDecl AliasDecl;
  if (const auto *DD = dyn_cast<CXXDestructorDecl>(MD)) {
    AliasDecl = GlobalDecl(DD, Dtor_Complete);
  } else {
    const auto *CD = cast<CXXConstructorDecl>(MD);
    AliasDecl = GlobalDecl(CD, Ctor_Complete);
  }
  llvm37::GlobalValue::LinkageTypes Linkage = CGM.getFunctionLinkage(AliasDecl);

  if (llvm37::GlobalValue::isDiscardableIfUnused(Linkage))
    return StructorCodegen::RAUW;

  // FIXME: Should we allow available_externally aliases?
  if (!llvm37::GlobalAlias::isValidLinkage(Linkage))
    return StructorCodegen::RAUW;

  if (llvm37::GlobalValue::isWeakForLinker(Linkage)) {
    // Only ELF supports COMDATs with arbitrary names (C5/D5).
    if (CGM.getTarget().getTriple().isOSBinFormatELF())
      return StructorCodegen::COMDAT;
    return StructorCodegen::Emit;
  }

  return StructorCodegen::Alias;
}

static void emitConstructorDestructorAlias(CodeGenModule &CGM,
                                           GlobalDecl AliasDecl,
                                           GlobalDecl TargetDecl) {
  llvm37::GlobalValue::LinkageTypes Linkage = CGM.getFunctionLinkage(AliasDecl);

  StringRef MangledName = CGM.getMangledName(AliasDecl);
  llvm37::GlobalValue *Entry = CGM.GetGlobalValue(MangledName);
  if (Entry && !Entry->isDeclaration())
    return;

  auto *Aliasee = cast<llvm37::GlobalValue>(CGM.GetAddrOfGlobal(TargetDecl));
  llvm37::PointerType *AliasType = Aliasee->getType();

  // Create the alias with no name.
  auto *Alias = llvm37::GlobalAlias::create(AliasType, Linkage, "", Aliasee,
                                          &CGM.getModule());

  // Switch any previous uses to the alias.
  if (Entry) {
    assert(Entry->getType() == AliasType &&
           "declaration exists with different type");
    Alias->takeName(Entry);
    Entry->replaceAllUsesWith(Alias);
    Entry->eraseFromParent();
  } else {
    Alias->setName(MangledName);
  }

  // Finally, set up the alias with its proper name and attributes.
  CGM.setAliasAttributes(cast<NamedDecl>(AliasDecl.getDecl()), Alias);
}

void ItaniumCXXABI::emitCXXStructor(const CXXMethodDecl *MD,
                                    StructorType Type) {
  auto *CD = dyn_cast<CXXConstructorDecl>(MD);
  const CXXDestructorDecl *DD = CD ? nullptr : cast<CXXDestructorDecl>(MD);

  StructorCodegen CGType = getCodegenToUse(CGM, MD);

  if (Type == StructorType::Complete) {
    GlobalDecl CompleteDecl;
    GlobalDecl BaseDecl;
    if (CD) {
      CompleteDecl = GlobalDecl(CD, Ctor_Complete);
      BaseDecl = GlobalDecl(CD, Ctor_Base);
    } else {
      CompleteDecl = GlobalDecl(DD, Dtor_Complete);
      BaseDecl = GlobalDecl(DD, Dtor_Base);
    }

    if (CGType == StructorCodegen::Alias || CGType == StructorCodegen::COMDAT) {
      emitConstructorDestructorAlias(CGM, CompleteDecl, BaseDecl);
      return;
    }

    if (CGType == StructorCodegen::RAUW) {
      StringRef MangledName = CGM.getMangledName(CompleteDecl);
      auto *Aliasee = cast<llvm37::GlobalValue>(CGM.GetAddrOfGlobal(BaseDecl));
      CGM.addReplacement(MangledName, Aliasee);
      return;
    }
  }

  // The base destructor is equivalent to the base destructor of its
  // base class if there is exactly one non-virtual base class with a
  // non-trivial destructor, there are no fields with a non-trivial
  // destructor, and the body of the destructor is trivial.
  if (DD && Type == StructorType::Base && CGType != StructorCodegen::COMDAT &&
      !CGM.TryEmitBaseDestructorAsAlias(DD))
    return;

  llvm37::Function *Fn = CGM.codegenCXXStructor(MD, Type);

  if (CGType == StructorCodegen::COMDAT) {
    SmallString<256> Buffer;
    llvm37::raw_svector_ostream Out(Buffer);
    if (DD)
      getMangleContext().mangleCXXDtorComdat(DD, Out);
    else
      getMangleContext().mangleCXXCtorComdat(CD, Out);
    llvm37::Comdat *C = CGM.getModule().getOrInsertComdat(Out.str());
    Fn->setComdat(C);
  } else {
    CGM.maybeSetTrivialComdat(*MD, *Fn);
  }
}

static llvm37::Constant *getBeginCatchFn(CodeGenModule &CGM) {
  // void *__cxa_begin_catch(void*);
  llvm37::FunctionType *FTy = llvm37::FunctionType::get(
      CGM.Int8PtrTy, CGM.Int8PtrTy, /*IsVarArgs=*/false);

  return CGM.CreateRuntimeFunction(FTy, "__cxa_begin_catch");
}

static llvm37::Constant *getEndCatchFn(CodeGenModule &CGM) {
  // void __cxa_end_catch();
  llvm37::FunctionType *FTy =
      llvm37::FunctionType::get(CGM.VoidTy, /*IsVarArgs=*/false);

  return CGM.CreateRuntimeFunction(FTy, "__cxa_end_catch");
}

static llvm37::Constant *getGetExceptionPtrFn(CodeGenModule &CGM) {
  // void *__cxa_get_exception_ptr(void*);
  llvm37::FunctionType *FTy = llvm37::FunctionType::get(
      CGM.Int8PtrTy, CGM.Int8PtrTy, /*IsVarArgs=*/false);

  return CGM.CreateRuntimeFunction(FTy, "__cxa_get_exception_ptr");
}

namespace {
  /// A cleanup to call __cxa_end_catch.  In many cases, the caught
  /// exception type lets us state definitively that the thrown exception
  /// type does not have a destructor.  In particular:
  ///   - Catch-alls tell us nothing, so we have to conservatively
  ///     assume that the thrown exception might have a destructor.
  ///   - Catches by reference behave according to their base types.
  ///   - Catches of non-record types will only trigger for exceptions
  ///     of non-record types, which never have destructors.
  ///   - Catches of record types can trigger for arbitrary subclasses
  ///     of the caught type, so we have to assume the actual thrown
  ///     exception type might have a throwing destructor, even if the
  ///     caught type's destructor is trivial or nothrow.
  struct CallEndCatch : EHScopeStack::Cleanup {
    CallEndCatch(bool MightThrow) : MightThrow(MightThrow) {}
    bool MightThrow;

    void Emit(CodeGenFunction &CGF, Flags flags) override {
      if (!MightThrow) {
        CGF.EmitNounwindRuntimeCall(getEndCatchFn(CGF.CGM));
        return;
      }

      CGF.EmitRuntimeCallOrInvoke(getEndCatchFn(CGF.CGM));
    }
  };
}

/// Emits a call to __cxa_begin_catch and enters a cleanup to call
/// __cxa_end_catch.
///
/// \param EndMightThrow - true if __cxa_end_catch might throw
static llvm37::Value *CallBeginCatch(CodeGenFunction &CGF,
                                   llvm37::Value *Exn,
                                   bool EndMightThrow) {
  llvm37::CallInst *call =
    CGF.EmitNounwindRuntimeCall(getBeginCatchFn(CGF.CGM), Exn);

  CGF.EHStack.pushCleanup<CallEndCatch>(NormalAndEHCleanup, EndMightThrow);

  return call;
}

/// A "special initializer" callback for initializing a catch
/// parameter during catch initialization.
static void InitCatchParam(CodeGenFunction &CGF,
                           const VarDecl &CatchParam,
                           llvm37::Value *ParamAddr,
                           SourceLocation Loc) {
  // Load the exception from where the landing pad saved it.
  llvm37::Value *Exn = CGF.getExceptionFromSlot();

  CanQualType CatchType =
    CGF.CGM.getContext().getCanonicalType(CatchParam.getType());
  llvm37::Type *LLVMCatchTy = CGF.ConvertTypeForMem(CatchType);

  // If we're catching by reference, we can just cast the object
  // pointer to the appropriate pointer.
  if (isa<ReferenceType>(CatchType)) {
    QualType CaughtType = cast<ReferenceType>(CatchType)->getPointeeType();
    bool EndCatchMightThrow = CaughtType->isRecordType();

    // __cxa_begin_catch returns the adjusted object pointer.
    llvm37::Value *AdjustedExn = CallBeginCatch(CGF, Exn, EndCatchMightThrow);

    // We have no way to tell the personality function that we're
    // catching by reference, so if we're catching a pointer,
    // __cxa_begin_catch will actually return that pointer by value.
    if (const PointerType *PT = dyn_cast<PointerType>(CaughtType)) {
      QualType PointeeType = PT->getPointeeType();

      // When catching by reference, generally we should just ignore
      // this by-value pointer and use the exception object instead.
      if (!PointeeType->isRecordType()) {

        // Exn points to the struct _Unwind_Exception header, which
        // we have to skip past in order to reach the exception data.
        unsigned HeaderSize =
          CGF.CGM.getTargetCodeGenInfo().getSizeOfUnwindException();
        AdjustedExn = CGF.Builder.CreateConstGEP1_32(Exn, HeaderSize);

      // However, if we're catching a pointer-to-record type that won't
      // work, because the personality function might have adjusted
      // the pointer.  There's actually no way for us to fully satisfy
      // the language/ABI contract here:  we can't use Exn because it
      // might have the wrong adjustment, but we can't use the by-value
      // pointer because it's off by a level of abstraction.
      //
      // The current solution is to dump the adjusted pointer into an
      // alloca, which breaks language semantics (because changing the
      // pointer doesn't change the exception) but at least works.
      // The better solution would be to filter out non-exact matches
      // and rethrow them, but this is tricky because the rethrow
      // really needs to be catchable by other sites at this landing
      // pad.  The best solution is to fix the personality function.
      } else {
        // Pull the pointer for the reference type off.
        llvm37::Type *PtrTy =
          cast<llvm37::PointerType>(LLVMCatchTy)->getElementType();

        // Create the temporary and write the adjusted pointer into it.
        llvm37::Value *ExnPtrTmp = CGF.CreateTempAlloca(PtrTy, "exn.byref.tmp");
        llvm37::Value *Casted = CGF.Builder.CreateBitCast(AdjustedExn, PtrTy);
        CGF.Builder.CreateStore(Casted, ExnPtrTmp);

        // Bind the reference to the temporary.
        AdjustedExn = ExnPtrTmp;
      }
    }

    llvm37::Value *ExnCast =
      CGF.Builder.CreateBitCast(AdjustedExn, LLVMCatchTy, "exn.byref");
    CGF.Builder.CreateStore(ExnCast, ParamAddr);
    return;
  }

  // Scalars and complexes.
  TypeEvaluationKind TEK = CGF.getEvaluationKind(CatchType);
  if (TEK != TEK_Aggregate) {
    llvm37::Value *AdjustedExn = CallBeginCatch(CGF, Exn, false);

    // If the catch type is a pointer type, __cxa_begin_catch returns
    // the pointer by value.
    if (CatchType->hasPointerRepresentation()) {
      llvm37::Value *CastExn =
        CGF.Builder.CreateBitCast(AdjustedExn, LLVMCatchTy, "exn.casted");

      switch (CatchType.getQualifiers().getObjCLifetime()) {
      case Qualifiers::OCL_Strong:
        CastExn = CGF.EmitARCRetainNonBlock(CastExn);
        // fallthrough

      case Qualifiers::OCL_None:
      case Qualifiers::OCL_ExplicitNone:
      case Qualifiers::OCL_Autoreleasing:
        CGF.Builder.CreateStore(CastExn, ParamAddr);
        return;

      case Qualifiers::OCL_Weak:
        CGF.EmitARCInitWeak(ParamAddr, CastExn);
        return;
      }
      llvm_unreachable("bad ownership qualifier!");
    }

    // Otherwise, it returns a pointer into the exception object.

    llvm37::Type *PtrTy = LLVMCatchTy->getPointerTo(0); // addrspace 0 ok
    llvm37::Value *Cast = CGF.Builder.CreateBitCast(AdjustedExn, PtrTy);

    LValue srcLV = CGF.MakeNaturalAlignAddrLValue(Cast, CatchType);
    LValue destLV = CGF.MakeAddrLValue(ParamAddr, CatchType,
                                  CGF.getContext().getDeclAlign(&CatchParam));
    switch (TEK) {
    case TEK_Complex:
      CGF.EmitStoreOfComplex(CGF.EmitLoadOfComplex(srcLV, Loc), destLV,
                             /*init*/ true);
      return;
    case TEK_Scalar: {
      llvm37::Value *ExnLoad = CGF.EmitLoadOfScalar(srcLV, Loc);
      CGF.EmitStoreOfScalar(ExnLoad, destLV, /*init*/ true);
      return;
    }
    case TEK_Aggregate:
      llvm_unreachable("evaluation kind filtered out!");
    }
    llvm_unreachable("bad evaluation kind");
  }

  assert(isa<RecordType>(CatchType) && "unexpected catch type!");

  llvm37::Type *PtrTy = LLVMCatchTy->getPointerTo(0); // addrspace 0 ok

  // Check for a copy expression.  If we don't have a copy expression,
  // that means a trivial copy is okay.
  const Expr *copyExpr = CatchParam.getInit();
  if (!copyExpr) {
    llvm37::Value *rawAdjustedExn = CallBeginCatch(CGF, Exn, true);
    llvm37::Value *adjustedExn = CGF.Builder.CreateBitCast(rawAdjustedExn, PtrTy);
    CGF.EmitAggregateCopy(ParamAddr, adjustedExn, CatchType);
    return;
  }

  // We have to call __cxa_get_exception_ptr to get the adjusted
  // pointer before copying.
  llvm37::CallInst *rawAdjustedExn =
    CGF.EmitNounwindRuntimeCall(getGetExceptionPtrFn(CGF.CGM), Exn);

  // Cast that to the appropriate type.
  llvm37::Value *adjustedExn = CGF.Builder.CreateBitCast(rawAdjustedExn, PtrTy);

  // The copy expression is defined in terms of an OpaqueValueExpr.
  // Find it and map it to the adjusted expression.
  CodeGenFunction::OpaqueValueMapping
    opaque(CGF, OpaqueValueExpr::findInCopyConstruct(copyExpr),
           CGF.MakeAddrLValue(adjustedExn, CatchParam.getType()));

  // Call the copy ctor in a terminate scope.
  CGF.EHStack.pushTerminate();

  // Perform the copy construction.
  CharUnits Alignment = CGF.getContext().getDeclAlign(&CatchParam);
  CGF.EmitAggExpr(copyExpr,
                  AggValueSlot::forAddr(ParamAddr, Alignment, Qualifiers(),
                                        AggValueSlot::IsNotDestructed,
                                        AggValueSlot::DoesNotNeedGCBarriers,
                                        AggValueSlot::IsNotAliased));

  // Leave the terminate scope.
  CGF.EHStack.popTerminate();

  // Undo the opaque value mapping.
  opaque.pop();

  // Finally we can call __cxa_begin_catch.
  CallBeginCatch(CGF, Exn, true);
}

/// Begins a catch statement by initializing the catch variable and
/// calling __cxa_begin_catch.
void ItaniumCXXABI::emitBeginCatch(CodeGenFunction &CGF,
                                   const CXXCatchStmt *S) {
  // We have to be very careful with the ordering of cleanups here:
  //   C++ [except.throw]p4:
  //     The destruction [of the exception temporary] occurs
  //     immediately after the destruction of the object declared in
  //     the exception-declaration in the handler.
  //
  // So the precise ordering is:
  //   1.  Construct catch variable.
  //   2.  __cxa_begin_catch
  //   3.  Enter __cxa_end_catch cleanup
  //   4.  Enter dtor cleanup
  //
  // We do this by using a slightly abnormal initialization process.
  // Delegation sequence:
  //   - ExitCXXTryStmt opens a RunCleanupsScope
  //     - EmitAutoVarAlloca creates the variable and debug info
  //       - InitCatchParam initializes the variable from the exception
  //       - CallBeginCatch calls __cxa_begin_catch
  //       - CallBeginCatch enters the __cxa_end_catch cleanup
  //     - EmitAutoVarCleanups enters the variable destructor cleanup
  //   - EmitCXXTryStmt emits the code for the catch body
  //   - EmitCXXTryStmt close the RunCleanupsScope

  VarDecl *CatchParam = S->getExceptionDecl();
  if (!CatchParam) {
    llvm37::Value *Exn = CGF.getExceptionFromSlot();
    CallBeginCatch(CGF, Exn, true);
    return;
  }

  // Emit the local.
  CodeGenFunction::AutoVarEmission var = CGF.EmitAutoVarAlloca(*CatchParam);
  InitCatchParam(CGF, *CatchParam, var.getObjectAddress(CGF), S->getLocStart());
  CGF.EmitAutoVarCleanups(var);
}

#if 0 // HLSL Change Start
/// Get or define the following function:
///   void @__clang_call_terminate(i8* %exn) nounwind noreturn
/// This code is used only in C++.
static llvm37::Constant *getClangCallTerminateFn(CodeGenModule &CGM) {
  llvm37::FunctionType *fnTy =
    llvm37::FunctionType::get(CGM.VoidTy, CGM.Int8PtrTy, /*IsVarArgs=*/false);
  llvm37::Constant *fnRef =
    CGM.CreateRuntimeFunction(fnTy, "__clang_call_terminate");

  llvm37::Function *fn = dyn_cast<llvm37::Function>(fnRef);
  if (fn && fn->empty()) {
    fn->setDoesNotThrow();
    fn->setDoesNotReturn();

    // What we really want is to massively penalize inlining without
    // forbidding it completely.  The difference between that and
    // 'noinline' is negligible.
    fn->addFnAttr(llvm37::Attribute::NoInline);

    // Allow this function to be shared across translation units, but
    // we don't want it to turn into an exported symbol.
    fn->setLinkage(llvm37::Function::LinkOnceODRLinkage);
    fn->setVisibility(llvm37::Function::HiddenVisibility);
    if (CGM.supportsCOMDAT())
      fn->setComdat(CGM.getModule().getOrInsertComdat(fn->getName()));

    // Set up the function.
    llvm37::BasicBlock *entry =
      llvm37::BasicBlock::Create(CGM.getLLVMContext(), "", fn);
    CGBuilderTy builder(entry);

    // Pull the exception pointer out of the parameter list.
    llvm37::Value *exn = &*fn->arg_begin();

    // Call __cxa_begin_catch(exn).
    llvm37::CallInst *catchCall = builder.CreateCall(getBeginCatchFn(CGM), exn);
    catchCall->setDoesNotThrow();
    catchCall->setCallingConv(CGM.getRuntimeCC());

    // Call std::terminate().
    llvm37::CallInst *termCall = builder.CreateCall(CGM.getTerminateFn());
    termCall->setDoesNotThrow();
    termCall->setDoesNotReturn();
    termCall->setCallingConv(CGM.getRuntimeCC());

    // std::terminate cannot return.
    builder.CreateUnreachable();
  }

  return fnRef;
}
#endif // HLSL Change End

llvm37::CallInst *
ItaniumCXXABI::emitTerminateForUnexpectedException(CodeGenFunction &CGF,
                                                   llvm37::Value *Exn) {
#if 0 // HLSL Change - no support for exceptions
  // In C++, we want to call __cxa_begin_catch() before terminating.
  if (Exn) {
    assert(CGF.CGM.getLangOpts().CPlusPlus);
    return CGF.EmitNounwindRuntimeCall(getClangCallTerminateFn(CGF.CGM), Exn);
  }
  return CGF.EmitNounwindRuntimeCall(CGF.CGM.getTerminateFn());
#else
  llvm_unreachable("HLSL does not support exception");
#endif // HLSL Change - no support for exceptions
}
