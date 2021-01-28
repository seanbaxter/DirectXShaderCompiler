//===------- CGObjCMac.cpp - Interface to Apple Objective-C Runtime -------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This provides Objective-C code generation targeting the Apple runtime.
//
//===----------------------------------------------------------------------===//

#include "CGObjCRuntime.h"
#include "CGBlocks.h"
#include "CGCleanup.h"
#include "CGRecordLayout.h"
#include "CodeGenFunction.h"
#include "CodeGenModule.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/RecordLayout.h"
#include "clang/AST/StmtObjC.h"
#include "clang/Basic/LangOptions.h"
#include "clang/CodeGen/CGFunctionInfo.h"
#include "clang/Frontend/CodeGenOptions.h"
#include "llvm37/ADT/DenseSet.h"
#include "llvm37/ADT/SetVector.h"
#include "llvm37/ADT/SmallPtrSet.h"
#include "llvm37/ADT/SmallString.h"
#include "llvm37/IR/CallSite.h"
#include "llvm37/IR/DataLayout.h"
#include "llvm37/IR/InlineAsm.h"
#include "llvm37/IR/IntrinsicInst.h"
#include "llvm37/IR/LLVMContext.h"
#include "llvm37/IR/Module.h"
#include "llvm37/Support/raw_ostream.h"
#include <cstdio>

using namespace clang;
using namespace CodeGen;

// HLSL Change Starts
// No ObjC codegen support, so simply skip all of this compilation.
// Here are enough stubs to link the current targets.
#if 0
// HLSL Change Ends

namespace {

// FIXME: We should find a nicer way to make the labels for metadata, string
// concatenation is lame.

class ObjCCommonTypesHelper {
protected:
  llvm37::LLVM37Context &VMContext;

private:
  // The types of these functions don't really matter because we
  // should always bitcast before calling them.

  /// id objc_msgSend (id, SEL, ...)
  /// 
  /// The default messenger, used for sends whose ABI is unchanged from
  /// the all-integer/pointer case.
  llvm37::Constant *getMessageSendFn() const {
    // Add the non-lazy-bind attribute, since objc_msgSend is likely to
    // be called a lot.
    llvm37::Type *params[] = { ObjectPtrTy, SelectorPtrTy };
    return
      CGM.CreateRuntimeFunction(llvm37::FunctionType::get(ObjectPtrTy,
                                                        params, true),
                                "objc_msgSend",
                                llvm37::AttributeSet::get(CGM.getLLVM37Context(),
                                              llvm37::AttributeSet::FunctionIndex,
                                                 llvm37::Attribute::NonLazyBind));
  }

  /// void objc_msgSend_stret (id, SEL, ...)
  ///
  /// The messenger used when the return value is an aggregate returned
  /// by indirect reference in the first argument, and therefore the
  /// self and selector parameters are shifted over by one.
  llvm37::Constant *getMessageSendStretFn() const {
    llvm37::Type *params[] = { ObjectPtrTy, SelectorPtrTy };
    return CGM.CreateRuntimeFunction(llvm37::FunctionType::get(CGM.VoidTy,
                                                             params, true),
                                     "objc_msgSend_stret");

  }

  /// [double | long double] objc_msgSend_fpret(id self, SEL op, ...)
  ///
  /// The messenger used when the return value is returned on the x87
  /// floating-point stack; without a special entrypoint, the nil case
  /// would be unbalanced.
  llvm37::Constant *getMessageSendFpretFn() const {
    llvm37::Type *params[] = { ObjectPtrTy, SelectorPtrTy };
    return CGM.CreateRuntimeFunction(llvm37::FunctionType::get(CGM.DoubleTy,
                                                             params, true),
                                     "objc_msgSend_fpret");

  }

  /// _Complex long double objc_msgSend_fp2ret(id self, SEL op, ...)
  ///
  /// The messenger used when the return value is returned in two values on the
  /// x87 floating point stack; without a special entrypoint, the nil case
  /// would be unbalanced. Only used on 64-bit X86.
  llvm37::Constant *getMessageSendFp2retFn() const {
    llvm37::Type *params[] = { ObjectPtrTy, SelectorPtrTy };
    llvm37::Type *longDoubleType = llvm37::Type::getX86_FP80Ty(VMContext);
    llvm37::Type *resultType = 
      llvm37::StructType::get(longDoubleType, longDoubleType, nullptr);

    return CGM.CreateRuntimeFunction(llvm37::FunctionType::get(resultType,
                                                             params, true),
                                     "objc_msgSend_fp2ret");
  }

  /// id objc_msgSendSuper(struct objc_super *super, SEL op, ...)
  ///
  /// The messenger used for super calls, which have different dispatch
  /// semantics.  The class passed is the superclass of the current
  /// class.
  llvm37::Constant *getMessageSendSuperFn() const {
    llvm37::Type *params[] = { SuperPtrTy, SelectorPtrTy };
    return CGM.CreateRuntimeFunction(llvm37::FunctionType::get(ObjectPtrTy,
                                                             params, true),
                                     "objc_msgSendSuper");
  }

  /// id objc_msgSendSuper2(struct objc_super *super, SEL op, ...)
  ///
  /// A slightly different messenger used for super calls.  The class
  /// passed is the current class.
  llvm37::Constant *getMessageSendSuperFn2() const {
    llvm37::Type *params[] = { SuperPtrTy, SelectorPtrTy };
    return CGM.CreateRuntimeFunction(llvm37::FunctionType::get(ObjectPtrTy,
                                                             params, true),
                                     "objc_msgSendSuper2");
  }

  /// void objc_msgSendSuper_stret(void *stretAddr, struct objc_super *super,
  ///                              SEL op, ...)
  ///
  /// The messenger used for super calls which return an aggregate indirectly.
  llvm37::Constant *getMessageSendSuperStretFn() const {
    llvm37::Type *params[] = { Int8PtrTy, SuperPtrTy, SelectorPtrTy };
    return CGM.CreateRuntimeFunction(
      llvm37::FunctionType::get(CGM.VoidTy, params, true),
      "objc_msgSendSuper_stret");
  }

  /// void objc_msgSendSuper2_stret(void * stretAddr, struct objc_super *super,
  ///                               SEL op, ...)
  ///
  /// objc_msgSendSuper_stret with the super2 semantics.
  llvm37::Constant *getMessageSendSuperStretFn2() const {
    llvm37::Type *params[] = { Int8PtrTy, SuperPtrTy, SelectorPtrTy };
    return CGM.CreateRuntimeFunction(
      llvm37::FunctionType::get(CGM.VoidTy, params, true),
      "objc_msgSendSuper2_stret");
  }

  llvm37::Constant *getMessageSendSuperFpretFn() const {
    // There is no objc_msgSendSuper_fpret? How can that work?
    return getMessageSendSuperFn();
  }

  llvm37::Constant *getMessageSendSuperFpretFn2() const {
    // There is no objc_msgSendSuper_fpret? How can that work?
    return getMessageSendSuperFn2();
  }

protected:
  CodeGen::CodeGenModule &CGM;

public:
  llvm37::Type *ShortTy, *IntTy, *LongTy, *LongLongTy;
  llvm37::Type *Int8PtrTy, *Int8PtrPtrTy;
  llvm37::Type *IvarOffsetVarTy;

  /// ObjectPtrTy - LLVM37 type for object handles (typeof(id))
  llvm37::Type *ObjectPtrTy;

  /// PtrObjectPtrTy - LLVM37 type for id *
  llvm37::Type *PtrObjectPtrTy;

  /// SelectorPtrTy - LLVM37 type for selector handles (typeof(SEL))
  llvm37::Type *SelectorPtrTy;
  
private:
  /// ProtocolPtrTy - LLVM37 type for external protocol handles
  /// (typeof(Protocol))
  llvm37::Type *ExternalProtocolPtrTy;
  
public:
  llvm37::Type *getExternalProtocolPtrTy() {
    if (!ExternalProtocolPtrTy) {
      // FIXME: It would be nice to unify this with the opaque type, so that the
      // IR comes out a bit cleaner.
      CodeGen::CodeGenTypes &Types = CGM.getTypes();
      ASTContext &Ctx = CGM.getContext();
      llvm37::Type *T = Types.ConvertType(Ctx.getObjCProtoType());
      ExternalProtocolPtrTy = llvm37::PointerType::getUnqual(T);
    }
    
    return ExternalProtocolPtrTy;
  }
  
  // SuperCTy - clang type for struct objc_super.
  QualType SuperCTy;
  // SuperPtrCTy - clang type for struct objc_super *.
  QualType SuperPtrCTy;

  /// SuperTy - LLVM37 type for struct objc_super.
  llvm37::StructType *SuperTy;
  /// SuperPtrTy - LLVM37 type for struct objc_super *.
  llvm37::Type *SuperPtrTy;

  /// PropertyTy - LLVM37 type for struct objc_property (struct _prop_t
  /// in GCC parlance).
  llvm37::StructType *PropertyTy;

  /// PropertyListTy - LLVM37 type for struct objc_property_list
  /// (_prop_list_t in GCC parlance).
  llvm37::StructType *PropertyListTy;
  /// PropertyListPtrTy - LLVM37 type for struct objc_property_list*.
  llvm37::Type *PropertyListPtrTy;

  // MethodTy - LLVM37 type for struct objc_method.
  llvm37::StructType *MethodTy;

  /// CacheTy - LLVM37 type for struct objc_cache.
  llvm37::Type *CacheTy;
  /// CachePtrTy - LLVM37 type for struct objc_cache *.
  llvm37::Type *CachePtrTy;
  
  llvm37::Constant *getGetPropertyFn() {
    CodeGen::CodeGenTypes &Types = CGM.getTypes();
    ASTContext &Ctx = CGM.getContext();
    // id objc_getProperty (id, SEL, ptrdiff_t, bool)
    SmallVector<CanQualType,4> Params;
    CanQualType IdType = Ctx.getCanonicalParamType(Ctx.getObjCIdType());
    CanQualType SelType = Ctx.getCanonicalParamType(Ctx.getObjCSelType());
    Params.push_back(IdType);
    Params.push_back(SelType);
    Params.push_back(Ctx.getPointerDiffType()->getCanonicalTypeUnqualified());
    Params.push_back(Ctx.BoolTy);
    llvm37::FunctionType *FTy =
        Types.GetFunctionType(Types.arrangeLLVM37FunctionInfo(
            IdType, false, false, Params, FunctionType::ExtInfo(),
            RequiredArgs::All));
    return CGM.CreateRuntimeFunction(FTy, "objc_getProperty");
  }

  llvm37::Constant *getSetPropertyFn() {
    CodeGen::CodeGenTypes &Types = CGM.getTypes();
    ASTContext &Ctx = CGM.getContext();
    // void objc_setProperty (id, SEL, ptrdiff_t, id, bool, bool)
    SmallVector<CanQualType,6> Params;
    CanQualType IdType = Ctx.getCanonicalParamType(Ctx.getObjCIdType());
    CanQualType SelType = Ctx.getCanonicalParamType(Ctx.getObjCSelType());
    Params.push_back(IdType);
    Params.push_back(SelType);
    Params.push_back(Ctx.getPointerDiffType()->getCanonicalTypeUnqualified());
    Params.push_back(IdType);
    Params.push_back(Ctx.BoolTy);
    Params.push_back(Ctx.BoolTy);
    llvm37::FunctionType *FTy =
        Types.GetFunctionType(Types.arrangeLLVM37FunctionInfo(
            Ctx.VoidTy, false, false, Params, FunctionType::ExtInfo(),
            RequiredArgs::All));
    return CGM.CreateRuntimeFunction(FTy, "objc_setProperty");
  }

  llvm37::Constant *getOptimizedSetPropertyFn(bool atomic, bool copy) {
    CodeGen::CodeGenTypes &Types = CGM.getTypes();
    ASTContext &Ctx = CGM.getContext();
    // void objc_setProperty_atomic(id self, SEL _cmd, 
    //                              id newValue, ptrdiff_t offset);
    // void objc_setProperty_nonatomic(id self, SEL _cmd, 
    //                                 id newValue, ptrdiff_t offset);
    // void objc_setProperty_atomic_copy(id self, SEL _cmd, 
    //                                   id newValue, ptrdiff_t offset);
    // void objc_setProperty_nonatomic_copy(id self, SEL _cmd, 
    //                                      id newValue, ptrdiff_t offset);
    
    SmallVector<CanQualType,4> Params;
    CanQualType IdType = Ctx.getCanonicalParamType(Ctx.getObjCIdType());
    CanQualType SelType = Ctx.getCanonicalParamType(Ctx.getObjCSelType());
    Params.push_back(IdType);
    Params.push_back(SelType);
    Params.push_back(IdType);
    Params.push_back(Ctx.getPointerDiffType()->getCanonicalTypeUnqualified());
    llvm37::FunctionType *FTy =
        Types.GetFunctionType(Types.arrangeLLVM37FunctionInfo(
            Ctx.VoidTy, false, false, Params, FunctionType::ExtInfo(),
            RequiredArgs::All));
    const char *name;
    if (atomic && copy)
      name = "objc_setProperty_atomic_copy";
    else if (atomic && !copy)
      name = "objc_setProperty_atomic";
    else if (!atomic && copy)
      name = "objc_setProperty_nonatomic_copy";
    else
      name = "objc_setProperty_nonatomic";
      
    return CGM.CreateRuntimeFunction(FTy, name);
  }
  
  llvm37::Constant *getCopyStructFn() {
    CodeGen::CodeGenTypes &Types = CGM.getTypes();
    ASTContext &Ctx = CGM.getContext();
    // void objc_copyStruct (void *, const void *, size_t, bool, bool)
    SmallVector<CanQualType,5> Params;
    Params.push_back(Ctx.VoidPtrTy);
    Params.push_back(Ctx.VoidPtrTy);
    Params.push_back(Ctx.LongTy);
    Params.push_back(Ctx.BoolTy);
    Params.push_back(Ctx.BoolTy);
    llvm37::FunctionType *FTy =
        Types.GetFunctionType(Types.arrangeLLVM37FunctionInfo(
            Ctx.VoidTy, false, false, Params, FunctionType::ExtInfo(),
            RequiredArgs::All));
    return CGM.CreateRuntimeFunction(FTy, "objc_copyStruct");
  }
  
  /// This routine declares and returns address of:
  /// void objc_copyCppObjectAtomic(
  ///         void *dest, const void *src, 
  ///         void (*copyHelper) (void *dest, const void *source));
  llvm37::Constant *getCppAtomicObjectFunction() {
    CodeGen::CodeGenTypes &Types = CGM.getTypes();
    ASTContext &Ctx = CGM.getContext();
    /// void objc_copyCppObjectAtomic(void *dest, const void *src, void *helper);
    SmallVector<CanQualType,3> Params;
    Params.push_back(Ctx.VoidPtrTy);
    Params.push_back(Ctx.VoidPtrTy);
    Params.push_back(Ctx.VoidPtrTy);
    llvm37::FunctionType *FTy =
      Types.GetFunctionType(Types.arrangeLLVM37FunctionInfo(Ctx.VoidTy, false, false,
                                                          Params,
                                                          FunctionType::ExtInfo(),
                                                          RequiredArgs::All));
    return CGM.CreateRuntimeFunction(FTy, "objc_copyCppObjectAtomic");
  }
  
  llvm37::Constant *getEnumerationMutationFn() {
    CodeGen::CodeGenTypes &Types = CGM.getTypes();
    ASTContext &Ctx = CGM.getContext();
    // void objc_enumerationMutation (id)
    SmallVector<CanQualType,1> Params;
    Params.push_back(Ctx.getCanonicalParamType(Ctx.getObjCIdType()));
    llvm37::FunctionType *FTy =
        Types.GetFunctionType(Types.arrangeLLVM37FunctionInfo(
            Ctx.VoidTy, false, false, Params, FunctionType::ExtInfo(),
            RequiredArgs::All));
    return CGM.CreateRuntimeFunction(FTy, "objc_enumerationMutation");
  }

  /// GcReadWeakFn -- LLVM37 objc_read_weak (id *src) function.
  llvm37::Constant *getGcReadWeakFn() {
    // id objc_read_weak (id *)
    llvm37::Type *args[] = { ObjectPtrTy->getPointerTo() };
    llvm37::FunctionType *FTy =
      llvm37::FunctionType::get(ObjectPtrTy, args, false);
    return CGM.CreateRuntimeFunction(FTy, "objc_read_weak");
  }

  /// GcAssignWeakFn -- LLVM37 objc_assign_weak function.
  llvm37::Constant *getGcAssignWeakFn() {
    // id objc_assign_weak (id, id *)
    llvm37::Type *args[] = { ObjectPtrTy, ObjectPtrTy->getPointerTo() };
    llvm37::FunctionType *FTy =
      llvm37::FunctionType::get(ObjectPtrTy, args, false);
    return CGM.CreateRuntimeFunction(FTy, "objc_assign_weak");
  }

  /// GcAssignGlobalFn -- LLVM37 objc_assign_global function.
  llvm37::Constant *getGcAssignGlobalFn() {
    // id objc_assign_global(id, id *)
    llvm37::Type *args[] = { ObjectPtrTy, ObjectPtrTy->getPointerTo() };
    llvm37::FunctionType *FTy =
      llvm37::FunctionType::get(ObjectPtrTy, args, false);
    return CGM.CreateRuntimeFunction(FTy, "objc_assign_global");
  }

  /// GcAssignThreadLocalFn -- LLVM37 objc_assign_threadlocal function.
  llvm37::Constant *getGcAssignThreadLocalFn() {
    // id objc_assign_threadlocal(id src, id * dest)
    llvm37::Type *args[] = { ObjectPtrTy, ObjectPtrTy->getPointerTo() };
    llvm37::FunctionType *FTy =
      llvm37::FunctionType::get(ObjectPtrTy, args, false);
    return CGM.CreateRuntimeFunction(FTy, "objc_assign_threadlocal");
  }
  
  /// GcAssignIvarFn -- LLVM37 objc_assign_ivar function.
  llvm37::Constant *getGcAssignIvarFn() {
    // id objc_assign_ivar(id, id *, ptrdiff_t)
    llvm37::Type *args[] = { ObjectPtrTy, ObjectPtrTy->getPointerTo(),
                           CGM.PtrDiffTy };
    llvm37::FunctionType *FTy =
      llvm37::FunctionType::get(ObjectPtrTy, args, false);
    return CGM.CreateRuntimeFunction(FTy, "objc_assign_ivar");
  }

  /// GcMemmoveCollectableFn -- LLVM37 objc_memmove_collectable function.
  llvm37::Constant *GcMemmoveCollectableFn() {
    // void *objc_memmove_collectable(void *dst, const void *src, size_t size)
    llvm37::Type *args[] = { Int8PtrTy, Int8PtrTy, LongTy };
    llvm37::FunctionType *FTy = llvm37::FunctionType::get(Int8PtrTy, args, false);
    return CGM.CreateRuntimeFunction(FTy, "objc_memmove_collectable");
  }

  /// GcAssignStrongCastFn -- LLVM37 objc_assign_strongCast function.
  llvm37::Constant *getGcAssignStrongCastFn() {
    // id objc_assign_strongCast(id, id *)
    llvm37::Type *args[] = { ObjectPtrTy, ObjectPtrTy->getPointerTo() };
    llvm37::FunctionType *FTy =
      llvm37::FunctionType::get(ObjectPtrTy, args, false);
    return CGM.CreateRuntimeFunction(FTy, "objc_assign_strongCast");
  }

  /// ExceptionThrowFn - LLVM37 objc_exception_throw function.
  llvm37::Constant *getExceptionThrowFn() {
    // void objc_exception_throw(id)
    llvm37::Type *args[] = { ObjectPtrTy };
    llvm37::FunctionType *FTy =
      llvm37::FunctionType::get(CGM.VoidTy, args, false);
    return CGM.CreateRuntimeFunction(FTy, "objc_exception_throw");
  }

  /// ExceptionRethrowFn - LLVM37 objc_exception_rethrow function.
  llvm37::Constant *getExceptionRethrowFn() {
    // void objc_exception_rethrow(void)
    llvm37::FunctionType *FTy = llvm37::FunctionType::get(CGM.VoidTy, false);
    return CGM.CreateRuntimeFunction(FTy, "objc_exception_rethrow");
  }
  
  /// SyncEnterFn - LLVM37 object_sync_enter function.
  llvm37::Constant *getSyncEnterFn() {
    // int objc_sync_enter (id)
    llvm37::Type *args[] = { ObjectPtrTy };
    llvm37::FunctionType *FTy =
      llvm37::FunctionType::get(CGM.IntTy, args, false);
    return CGM.CreateRuntimeFunction(FTy, "objc_sync_enter");
  }

  /// SyncExitFn - LLVM37 object_sync_exit function.
  llvm37::Constant *getSyncExitFn() {
    // int objc_sync_exit (id)
    llvm37::Type *args[] = { ObjectPtrTy };
    llvm37::FunctionType *FTy =
      llvm37::FunctionType::get(CGM.IntTy, args, false);
    return CGM.CreateRuntimeFunction(FTy, "objc_sync_exit");
  }

  llvm37::Constant *getSendFn(bool IsSuper) const {
    return IsSuper ? getMessageSendSuperFn() : getMessageSendFn();
  }

  llvm37::Constant *getSendFn2(bool IsSuper) const {
    return IsSuper ? getMessageSendSuperFn2() : getMessageSendFn();
  }

  llvm37::Constant *getSendStretFn(bool IsSuper) const {
    return IsSuper ? getMessageSendSuperStretFn() : getMessageSendStretFn();
  }

  llvm37::Constant *getSendStretFn2(bool IsSuper) const {
    return IsSuper ? getMessageSendSuperStretFn2() : getMessageSendStretFn();
  }

  llvm37::Constant *getSendFpretFn(bool IsSuper) const {
    return IsSuper ? getMessageSendSuperFpretFn() : getMessageSendFpretFn();
  }

  llvm37::Constant *getSendFpretFn2(bool IsSuper) const {
    return IsSuper ? getMessageSendSuperFpretFn2() : getMessageSendFpretFn();
  }

  llvm37::Constant *getSendFp2retFn(bool IsSuper) const {
    return IsSuper ? getMessageSendSuperFn() : getMessageSendFp2retFn();
  }

  llvm37::Constant *getSendFp2RetFn2(bool IsSuper) const {
    return IsSuper ? getMessageSendSuperFn2() : getMessageSendFp2retFn();
  }

  ObjCCommonTypesHelper(CodeGen::CodeGenModule &cgm);
};

/// ObjCTypesHelper - Helper class that encapsulates lazy
/// construction of varies types used during ObjC generation.
class ObjCTypesHelper : public ObjCCommonTypesHelper {
public:
  /// SymtabTy - LLVM37 type for struct objc_symtab.
  llvm37::StructType *SymtabTy;
  /// SymtabPtrTy - LLVM37 type for struct objc_symtab *.
  llvm37::Type *SymtabPtrTy;
  /// ModuleTy - LLVM37 type for struct objc_module.
  llvm37::StructType *ModuleTy;

  /// ProtocolTy - LLVM37 type for struct objc_protocol.
  llvm37::StructType *ProtocolTy;
  /// ProtocolPtrTy - LLVM37 type for struct objc_protocol *.
  llvm37::Type *ProtocolPtrTy;
  /// ProtocolExtensionTy - LLVM37 type for struct
  /// objc_protocol_extension.
  llvm37::StructType *ProtocolExtensionTy;
  /// ProtocolExtensionTy - LLVM37 type for struct
  /// objc_protocol_extension *.
  llvm37::Type *ProtocolExtensionPtrTy;
  /// MethodDescriptionTy - LLVM37 type for struct
  /// objc_method_description.
  llvm37::StructType *MethodDescriptionTy;
  /// MethodDescriptionListTy - LLVM37 type for struct
  /// objc_method_description_list.
  llvm37::StructType *MethodDescriptionListTy;
  /// MethodDescriptionListPtrTy - LLVM37 type for struct
  /// objc_method_description_list *.
  llvm37::Type *MethodDescriptionListPtrTy;
  /// ProtocolListTy - LLVM37 type for struct objc_property_list.
  llvm37::StructType *ProtocolListTy;
  /// ProtocolListPtrTy - LLVM37 type for struct objc_property_list*.
  llvm37::Type *ProtocolListPtrTy;
  /// CategoryTy - LLVM37 type for struct objc_category.
  llvm37::StructType *CategoryTy;
  /// ClassTy - LLVM37 type for struct objc_class.
  llvm37::StructType *ClassTy;
  /// ClassPtrTy - LLVM37 type for struct objc_class *.
  llvm37::Type *ClassPtrTy;
  /// ClassExtensionTy - LLVM37 type for struct objc_class_ext.
  llvm37::StructType *ClassExtensionTy;
  /// ClassExtensionPtrTy - LLVM37 type for struct objc_class_ext *.
  llvm37::Type *ClassExtensionPtrTy;
  // IvarTy - LLVM37 type for struct objc_ivar.
  llvm37::StructType *IvarTy;
  /// IvarListTy - LLVM37 type for struct objc_ivar_list.
  llvm37::Type *IvarListTy;
  /// IvarListPtrTy - LLVM37 type for struct objc_ivar_list *.
  llvm37::Type *IvarListPtrTy;
  /// MethodListTy - LLVM37 type for struct objc_method_list.
  llvm37::Type *MethodListTy;
  /// MethodListPtrTy - LLVM37 type for struct objc_method_list *.
  llvm37::Type *MethodListPtrTy;

  /// ExceptionDataTy - LLVM37 type for struct _objc_exception_data.
  llvm37::Type *ExceptionDataTy;
  
  /// ExceptionTryEnterFn - LLVM37 objc_exception_try_enter function.
  llvm37::Constant *getExceptionTryEnterFn() {
    llvm37::Type *params[] = { ExceptionDataTy->getPointerTo() };
    return CGM.CreateRuntimeFunction(
      llvm37::FunctionType::get(CGM.VoidTy, params, false),
      "objc_exception_try_enter");
  }

  /// ExceptionTryExitFn - LLVM37 objc_exception_try_exit function.
  llvm37::Constant *getExceptionTryExitFn() {
    llvm37::Type *params[] = { ExceptionDataTy->getPointerTo() };
    return CGM.CreateRuntimeFunction(
      llvm37::FunctionType::get(CGM.VoidTy, params, false),
      "objc_exception_try_exit");
  }

  /// ExceptionExtractFn - LLVM37 objc_exception_extract function.
  llvm37::Constant *getExceptionExtractFn() {
    llvm37::Type *params[] = { ExceptionDataTy->getPointerTo() };
    return CGM.CreateRuntimeFunction(llvm37::FunctionType::get(ObjectPtrTy,
                                                             params, false),
                                     "objc_exception_extract");
  }

  /// ExceptionMatchFn - LLVM37 objc_exception_match function.
  llvm37::Constant *getExceptionMatchFn() {
    llvm37::Type *params[] = { ClassPtrTy, ObjectPtrTy };
    return CGM.CreateRuntimeFunction(
      llvm37::FunctionType::get(CGM.Int32Ty, params, false),
      "objc_exception_match");

  }

  /// SetJmpFn - LLVM37 _setjmp function.
  llvm37::Constant *getSetJmpFn() {
    // This is specifically the prototype for x86.
    llvm37::Type *params[] = { CGM.Int32Ty->getPointerTo() };
    return
      CGM.CreateRuntimeFunction(llvm37::FunctionType::get(CGM.Int32Ty,
                                                        params, false),
                                "_setjmp",
                                llvm37::AttributeSet::get(CGM.getLLVM37Context(),
                                              llvm37::AttributeSet::FunctionIndex,
                                                 llvm37::Attribute::NonLazyBind));
  }

public:
  ObjCTypesHelper(CodeGen::CodeGenModule &cgm);
};

/// ObjCNonFragileABITypesHelper - will have all types needed by objective-c's
/// modern abi
class ObjCNonFragileABITypesHelper : public ObjCCommonTypesHelper {
public:

  // MethodListnfABITy - LLVM37 for struct _method_list_t
  llvm37::StructType *MethodListnfABITy;

  // MethodListnfABIPtrTy - LLVM37 for struct _method_list_t*
  llvm37::Type *MethodListnfABIPtrTy;

  // ProtocolnfABITy = LLVM37 for struct _protocol_t
  llvm37::StructType *ProtocolnfABITy;

  // ProtocolnfABIPtrTy = LLVM37 for struct _protocol_t*
  llvm37::Type *ProtocolnfABIPtrTy;

  // ProtocolListnfABITy - LLVM37 for struct _objc_protocol_list
  llvm37::StructType *ProtocolListnfABITy;

  // ProtocolListnfABIPtrTy - LLVM37 for struct _objc_protocol_list*
  llvm37::Type *ProtocolListnfABIPtrTy;

  // ClassnfABITy - LLVM37 for struct _class_t
  llvm37::StructType *ClassnfABITy;

  // ClassnfABIPtrTy - LLVM37 for struct _class_t*
  llvm37::Type *ClassnfABIPtrTy;

  // IvarnfABITy - LLVM37 for struct _ivar_t
  llvm37::StructType *IvarnfABITy;

  // IvarListnfABITy - LLVM37 for struct _ivar_list_t
  llvm37::StructType *IvarListnfABITy;

  // IvarListnfABIPtrTy = LLVM37 for struct _ivar_list_t*
  llvm37::Type *IvarListnfABIPtrTy;

  // ClassRonfABITy - LLVM37 for struct _class_ro_t
  llvm37::StructType *ClassRonfABITy;

  // ImpnfABITy - LLVM37 for id (*)(id, SEL, ...)
  llvm37::Type *ImpnfABITy;

  // CategorynfABITy - LLVM37 for struct _category_t
  llvm37::StructType *CategorynfABITy;

  // New types for nonfragile abi messaging.

  // MessageRefTy - LLVM37 for:
  // struct _message_ref_t {
  //   IMP messenger;
  //   SEL name;
  // };
  llvm37::StructType *MessageRefTy;
  // MessageRefCTy - clang type for struct _message_ref_t
  QualType MessageRefCTy;

  // MessageRefPtrTy - LLVM37 for struct _message_ref_t*
  llvm37::Type *MessageRefPtrTy;
  // MessageRefCPtrTy - clang type for struct _message_ref_t*
  QualType MessageRefCPtrTy;

  // MessengerTy - Type of the messenger (shown as IMP above)
  llvm37::FunctionType *MessengerTy;

  // SuperMessageRefTy - LLVM37 for:
  // struct _super_message_ref_t {
  //   SUPER_IMP messenger;
  //   SEL name;
  // };
  llvm37::StructType *SuperMessageRefTy;

  // SuperMessageRefPtrTy - LLVM37 for struct _super_message_ref_t*
  llvm37::Type *SuperMessageRefPtrTy;

  llvm37::Constant *getMessageSendFixupFn() {
    // id objc_msgSend_fixup(id, struct message_ref_t*, ...)
    llvm37::Type *params[] = { ObjectPtrTy, MessageRefPtrTy };
    return CGM.CreateRuntimeFunction(llvm37::FunctionType::get(ObjectPtrTy,
                                                             params, true),
                                     "objc_msgSend_fixup");
  }

  llvm37::Constant *getMessageSendFpretFixupFn() {
    // id objc_msgSend_fpret_fixup(id, struct message_ref_t*, ...)
    llvm37::Type *params[] = { ObjectPtrTy, MessageRefPtrTy };
    return CGM.CreateRuntimeFunction(llvm37::FunctionType::get(ObjectPtrTy,
                                                             params, true),
                                     "objc_msgSend_fpret_fixup");
  }

  llvm37::Constant *getMessageSendStretFixupFn() {
    // id objc_msgSend_stret_fixup(id, struct message_ref_t*, ...)
    llvm37::Type *params[] = { ObjectPtrTy, MessageRefPtrTy };
    return CGM.CreateRuntimeFunction(llvm37::FunctionType::get(ObjectPtrTy,
                                                             params, true),
                                     "objc_msgSend_stret_fixup");
  }

  llvm37::Constant *getMessageSendSuper2FixupFn() {
    // id objc_msgSendSuper2_fixup (struct objc_super *,
    //                              struct _super_message_ref_t*, ...)
    llvm37::Type *params[] = { SuperPtrTy, SuperMessageRefPtrTy };
    return  CGM.CreateRuntimeFunction(llvm37::FunctionType::get(ObjectPtrTy,
                                                              params, true),
                                      "objc_msgSendSuper2_fixup");
  }

  llvm37::Constant *getMessageSendSuper2StretFixupFn() {
    // id objc_msgSendSuper2_stret_fixup(struct objc_super *,
    //                                   struct _super_message_ref_t*, ...)
    llvm37::Type *params[] = { SuperPtrTy, SuperMessageRefPtrTy };
    return  CGM.CreateRuntimeFunction(llvm37::FunctionType::get(ObjectPtrTy,
                                                              params, true),
                                      "objc_msgSendSuper2_stret_fixup");
  }

  llvm37::Constant *getObjCEndCatchFn() {
    return CGM.CreateRuntimeFunction(llvm37::FunctionType::get(CGM.VoidTy, false),
                                     "objc_end_catch");

  }

  llvm37::Constant *getObjCBeginCatchFn() {
    llvm37::Type *params[] = { Int8PtrTy };
    return CGM.CreateRuntimeFunction(llvm37::FunctionType::get(Int8PtrTy,
                                                             params, false),
                                     "objc_begin_catch");
  }

  llvm37::StructType *EHTypeTy;
  llvm37::Type *EHTypePtrTy;
  
  ObjCNonFragileABITypesHelper(CodeGen::CodeGenModule &cgm);
};

class CGObjCCommonMac : public CodeGen::CGObjCRuntime {
public:
  // FIXME - accessibility
  class GC_IVAR {
  public:
    unsigned ivar_bytepos;
    unsigned ivar_size;
    GC_IVAR(unsigned bytepos = 0, unsigned size = 0)
      : ivar_bytepos(bytepos), ivar_size(size) {}

    // Allow sorting based on byte pos.
    bool operator<(const GC_IVAR &b) const {
      return ivar_bytepos < b.ivar_bytepos;
    }
  };

  class SKIP_SCAN {
  public:
    unsigned skip;
    unsigned scan;
    SKIP_SCAN(unsigned _skip = 0, unsigned _scan = 0)
      : skip(_skip), scan(_scan) {}
  };

  /// opcode for captured block variables layout 'instructions'.
  /// In the following descriptions, 'I' is the value of the immediate field.
  /// (field following the opcode).
  ///
  enum BLOCK_LAYOUT_OPCODE {
    /// An operator which affects how the following layout should be
    /// interpreted.
    ///   I == 0: Halt interpretation and treat everything else as
    ///           a non-pointer.  Note that this instruction is equal
    ///           to '\0'.
    ///   I != 0: Currently unused.
    BLOCK_LAYOUT_OPERATOR            = 0,
    
    /// The next I+1 bytes do not contain a value of object pointer type.
    /// Note that this can leave the stream unaligned, meaning that
    /// subsequent word-size instructions do not begin at a multiple of
    /// the pointer size.
    BLOCK_LAYOUT_NON_OBJECT_BYTES    = 1,
    
    /// The next I+1 words do not contain a value of object pointer type.
    /// This is simply an optimized version of BLOCK_LAYOUT_BYTES for
    /// when the required skip quantity is a multiple of the pointer size.
    BLOCK_LAYOUT_NON_OBJECT_WORDS    = 2,
    
    /// The next I+1 words are __strong pointers to Objective-C
    /// objects or blocks.
    BLOCK_LAYOUT_STRONG              = 3,
    
    /// The next I+1 words are pointers to __block variables.
    BLOCK_LAYOUT_BYREF               = 4,
    
    /// The next I+1 words are __weak pointers to Objective-C
    /// objects or blocks.
    BLOCK_LAYOUT_WEAK                = 5,
    
    /// The next I+1 words are __unsafe_unretained pointers to
    /// Objective-C objects or blocks.
    BLOCK_LAYOUT_UNRETAINED          = 6
    
    /// The next I+1 words are block or object pointers with some
    /// as-yet-unspecified ownership semantics.  If we add more
    /// flavors of ownership semantics, values will be taken from
    /// this range.
    ///
    /// This is included so that older tools can at least continue
    /// processing the layout past such things.
    //BLOCK_LAYOUT_OWNERSHIP_UNKNOWN = 7..10,
    
    /// All other opcodes are reserved.  Halt interpretation and
    /// treat everything else as opaque.
  };
 
  class RUN_SKIP {
  public:
    enum BLOCK_LAYOUT_OPCODE opcode;
    CharUnits block_var_bytepos;
    CharUnits block_var_size;
    RUN_SKIP(enum BLOCK_LAYOUT_OPCODE Opcode = BLOCK_LAYOUT_OPERATOR,
             CharUnits BytePos = CharUnits::Zero(),
             CharUnits Size = CharUnits::Zero())
    : opcode(Opcode), block_var_bytepos(BytePos),  block_var_size(Size) {}
    
    // Allow sorting based on byte pos.
    bool operator<(const RUN_SKIP &b) const {
      return block_var_bytepos < b.block_var_bytepos;
    }
  };
  
protected:
  llvm37::LLVM37Context &VMContext;
  // FIXME! May not be needing this after all.
  unsigned ObjCABI;

  // gc ivar layout bitmap calculation helper caches.
  SmallVector<GC_IVAR, 16> SkipIvars;
  SmallVector<GC_IVAR, 16> IvarsInfo;
  
  // arc/mrr layout of captured block literal variables.
  SmallVector<RUN_SKIP, 16> RunSkipBlockVars;

  /// LazySymbols - Symbols to generate a lazy reference for. See
  /// DefinedSymbols and FinishModule().
  llvm37::SetVector<IdentifierInfo*> LazySymbols;

  /// DefinedSymbols - External symbols which are defined by this
  /// module. The symbols in this list and LazySymbols are used to add
  /// special linker symbols which ensure that Objective-C modules are
  /// linked properly.
  llvm37::SetVector<IdentifierInfo*> DefinedSymbols;

  /// ClassNames - uniqued class names.
  llvm37::StringMap<llvm37::GlobalVariable*> ClassNames;

  /// MethodVarNames - uniqued method variable names.
  llvm37::DenseMap<Selector, llvm37::GlobalVariable*> MethodVarNames;

  /// DefinedCategoryNames - list of category names in form Class_Category.
  llvm37::SetVector<std::string> DefinedCategoryNames;

  /// MethodVarTypes - uniqued method type signatures. We have to use
  /// a StringMap here because have no other unique reference.
  llvm37::StringMap<llvm37::GlobalVariable*> MethodVarTypes;

  /// MethodDefinitions - map of methods which have been defined in
  /// this translation unit.
  llvm37::DenseMap<const ObjCMethodDecl*, llvm37::Function*> MethodDefinitions;

  /// PropertyNames - uniqued method variable names.
  llvm37::DenseMap<IdentifierInfo*, llvm37::GlobalVariable*> PropertyNames;

  /// ClassReferences - uniqued class references.
  llvm37::DenseMap<IdentifierInfo*, llvm37::GlobalVariable*> ClassReferences;

  /// SelectorReferences - uniqued selector references.
  llvm37::DenseMap<Selector, llvm37::GlobalVariable*> SelectorReferences;

  /// Protocols - Protocols for which an objc_protocol structure has
  /// been emitted. Forward declarations are handled by creating an
  /// empty structure whose initializer is filled in when/if defined.
  llvm37::DenseMap<IdentifierInfo*, llvm37::GlobalVariable*> Protocols;

  /// DefinedProtocols - Protocols which have actually been
  /// defined. We should not need this, see FIXME in GenerateProtocol.
  llvm37::DenseSet<IdentifierInfo*> DefinedProtocols;

  /// DefinedClasses - List of defined classes.
  SmallVector<llvm37::GlobalValue*, 16> DefinedClasses;
  
  /// ImplementedClasses - List of @implemented classes.
  SmallVector<const ObjCInterfaceDecl*, 16> ImplementedClasses;

  /// DefinedNonLazyClasses - List of defined "non-lazy" classes.
  SmallVector<llvm37::GlobalValue*, 16> DefinedNonLazyClasses;

  /// DefinedCategories - List of defined categories.
  SmallVector<llvm37::GlobalValue*, 16> DefinedCategories;

  /// DefinedNonLazyCategories - List of defined "non-lazy" categories.
  SmallVector<llvm37::GlobalValue*, 16> DefinedNonLazyCategories;

  /// GetNameForMethod - Return a name for the given method.
  /// \param[out] NameOut - The return value.
  void GetNameForMethod(const ObjCMethodDecl *OMD,
                        const ObjCContainerDecl *CD,
                        SmallVectorImpl<char> &NameOut);

  /// GetMethodVarName - Return a unique constant for the given
  /// selector's name. The return value has type char *.
  llvm37::Constant *GetMethodVarName(Selector Sel);
  llvm37::Constant *GetMethodVarName(IdentifierInfo *Ident);

  /// GetMethodVarType - Return a unique constant for the given
  /// method's type encoding string. The return value has type char *.

  // FIXME: This is a horrible name.
  llvm37::Constant *GetMethodVarType(const ObjCMethodDecl *D,
                                   bool Extended = false);
  llvm37::Constant *GetMethodVarType(const FieldDecl *D);

  /// GetPropertyName - Return a unique constant for the given
  /// name. The return value has type char *.
  llvm37::Constant *GetPropertyName(IdentifierInfo *Ident);

  // FIXME: This can be dropped once string functions are unified.
  llvm37::Constant *GetPropertyTypeString(const ObjCPropertyDecl *PD,
                                        const Decl *Container);

  /// GetClassName - Return a unique constant for the given selector's
  /// runtime name (which may change via use of objc_runtime_name attribute on
  /// class or protocol definition. The return value has type char *.
  llvm37::Constant *GetClassName(StringRef RuntimeName);

  llvm37::Function *GetMethodDefinition(const ObjCMethodDecl *MD);

  /// BuildIvarLayout - Builds ivar layout bitmap for the class
  /// implementation for the __strong or __weak case.
  ///
  llvm37::Constant *BuildIvarLayout(const ObjCImplementationDecl *OI,
                                  bool ForStrongLayout);
  
  llvm37::Constant *BuildIvarLayoutBitmap(std::string &BitMap);

  void BuildAggrIvarRecordLayout(const RecordType *RT,
                                 unsigned int BytePos, bool ForStrongLayout,
                                 bool &HasUnion);
  void BuildAggrIvarLayout(const ObjCImplementationDecl *OI,
                           const llvm37::StructLayout *Layout,
                           const RecordDecl *RD,
                           ArrayRef<const FieldDecl*> RecFields,
                           unsigned int BytePos, bool ForStrongLayout,
                           bool &HasUnion);
  
  Qualifiers::ObjCLifetime getBlockCaptureLifetime(QualType QT, bool ByrefLayout);
  
  void UpdateRunSkipBlockVars(bool IsByref,
                              Qualifiers::ObjCLifetime LifeTime,
                              CharUnits FieldOffset,
                              CharUnits FieldSize);
  
  void BuildRCBlockVarRecordLayout(const RecordType *RT,
                                   CharUnits BytePos, bool &HasUnion,
                                   bool ByrefLayout=false);
  
  void BuildRCRecordLayout(const llvm37::StructLayout *RecLayout,
                           const RecordDecl *RD,
                           ArrayRef<const FieldDecl*> RecFields,
                           CharUnits BytePos, bool &HasUnion,
                           bool ByrefLayout);
  
  uint64_t InlineLayoutInstruction(SmallVectorImpl<unsigned char> &Layout);
  
  llvm37::Constant *getBitmapBlockLayout(bool ComputeByrefLayout);
  

  /// GetIvarLayoutName - Returns a unique constant for the given
  /// ivar layout bitmap.
  llvm37::Constant *GetIvarLayoutName(IdentifierInfo *Ident,
                                    const ObjCCommonTypesHelper &ObjCTypes);

  /// EmitPropertyList - Emit the given property list. The return
  /// value has type PropertyListPtrTy.
  llvm37::Constant *EmitPropertyList(Twine Name,
                                   const Decl *Container,
                                   const ObjCContainerDecl *OCD,
                                   const ObjCCommonTypesHelper &ObjCTypes);

  /// EmitProtocolMethodTypes - Generate the array of extended method type 
  /// strings. The return value has type Int8PtrPtrTy.
  llvm37::Constant *EmitProtocolMethodTypes(Twine Name, 
                                          ArrayRef<llvm37::Constant*> MethodTypes,
                                       const ObjCCommonTypesHelper &ObjCTypes);

  /// PushProtocolProperties - Push protocol's property on the input stack.
  void PushProtocolProperties(
    llvm37::SmallPtrSet<const IdentifierInfo*, 16> &PropertySet,
    SmallVectorImpl<llvm37::Constant*> &Properties,
    const Decl *Container,
    const ObjCProtocolDecl *Proto,
    const ObjCCommonTypesHelper &ObjCTypes);

  /// GetProtocolRef - Return a reference to the internal protocol
  /// description, creating an empty one if it has not been
  /// defined. The return value has type ProtocolPtrTy.
  llvm37::Constant *GetProtocolRef(const ObjCProtocolDecl *PD);

  /// CreateMetadataVar - Create a global variable with internal
  /// linkage for use by the Objective-C runtime.
  ///
  /// This is a convenience wrapper which not only creates the
  /// variable, but also sets the section and alignment and adds the
  /// global to the "llvm.used" list.
  ///
  /// \param Name - The variable name.
  /// \param Init - The variable initializer; this is also used to
  /// define the type of the variable.
  /// \param Section - The section the variable should go into, or empty.
  /// \param Align - The alignment for the variable, or 0.
  /// \param AddToUsed - Whether the variable should be added to
  /// "llvm.used".
  llvm37::GlobalVariable *CreateMetadataVar(Twine Name, llvm37::Constant *Init,
                                          StringRef Section, unsigned Align,
                                          bool AddToUsed);

  CodeGen::RValue EmitMessageSend(CodeGen::CodeGenFunction &CGF,
                                  ReturnValueSlot Return,
                                  QualType ResultType,
                                  llvm37::Value *Sel,
                                  llvm37::Value *Arg0,
                                  QualType Arg0Ty,
                                  bool IsSuper,
                                  const CallArgList &CallArgs,
                                  const ObjCMethodDecl *OMD,
                                  const ObjCCommonTypesHelper &ObjCTypes);

  /// EmitImageInfo - Emit the image info marker used to encode some module
  /// level information.
  void EmitImageInfo();

public:
  CGObjCCommonMac(CodeGen::CodeGenModule &cgm) :
    CGObjCRuntime(cgm), VMContext(cgm.getLLVM37Context()) { }

  llvm37::Constant *GenerateConstantString(const StringLiteral *SL) override;

  llvm37::Function *GenerateMethod(const ObjCMethodDecl *OMD,
                                 const ObjCContainerDecl *CD=nullptr) override;

  void GenerateProtocol(const ObjCProtocolDecl *PD) override;

  /// GetOrEmitProtocol - Get the protocol object for the given
  /// declaration, emitting it if necessary. The return value has type
  /// ProtocolPtrTy.
  virtual llvm37::Constant *GetOrEmitProtocol(const ObjCProtocolDecl *PD)=0;

  /// GetOrEmitProtocolRef - Get a forward reference to the protocol
  /// object for the given declaration, emitting it if needed. These
  /// forward references will be filled in with empty bodies if no
  /// definition is seen. The return value has type ProtocolPtrTy.
  virtual llvm37::Constant *GetOrEmitProtocolRef(const ObjCProtocolDecl *PD)=0;
  llvm37::Constant *BuildGCBlockLayout(CodeGen::CodeGenModule &CGM,
                                     const CGBlockInfo &blockInfo) override;
  llvm37::Constant *BuildRCBlockLayout(CodeGen::CodeGenModule &CGM,
                                     const CGBlockInfo &blockInfo) override;

  llvm37::Constant *BuildByrefLayout(CodeGen::CodeGenModule &CGM,
                                   QualType T) override;
};

class CGObjCMac : public CGObjCCommonMac {
private:
  ObjCTypesHelper ObjCTypes;

  /// EmitModuleInfo - Another marker encoding module level
  /// information.
  void EmitModuleInfo();

  /// EmitModuleSymols - Emit module symbols, the list of defined
  /// classes and categories. The result has type SymtabPtrTy.
  llvm37::Constant *EmitModuleSymbols();

  /// FinishModule - Write out global data structures at the end of
  /// processing a translation unit.
  void FinishModule();

  /// EmitClassExtension - Generate the class extension structure used
  /// to store the weak ivar layout and properties. The return value
  /// has type ClassExtensionPtrTy.
  llvm37::Constant *EmitClassExtension(const ObjCImplementationDecl *ID);

  /// EmitClassRef - Return a Value*, of type ObjCTypes.ClassPtrTy,
  /// for the given class.
  llvm37::Value *EmitClassRef(CodeGenFunction &CGF,
                            const ObjCInterfaceDecl *ID);
  
  llvm37::Value *EmitClassRefFromId(CodeGenFunction &CGF,
                                  IdentifierInfo *II);

  llvm37::Value *EmitNSAutoreleasePoolClassRef(CodeGenFunction &CGF) override;

  /// EmitSuperClassRef - Emits reference to class's main metadata class.
  llvm37::Value *EmitSuperClassRef(const ObjCInterfaceDecl *ID);

  /// EmitIvarList - Emit the ivar list for the given
  /// implementation. If ForClass is true the list of class ivars
  /// (i.e. metaclass ivars) is emitted, otherwise the list of
  /// interface ivars will be emitted. The return value has type
  /// IvarListPtrTy.
  llvm37::Constant *EmitIvarList(const ObjCImplementationDecl *ID,
                               bool ForClass);

  /// EmitMetaClass - Emit a forward reference to the class structure
  /// for the metaclass of the given interface. The return value has
  /// type ClassPtrTy.
  llvm37::Constant *EmitMetaClassRef(const ObjCInterfaceDecl *ID);

  /// EmitMetaClass - Emit a class structure for the metaclass of the
  /// given implementation. The return value has type ClassPtrTy.
  llvm37::Constant *EmitMetaClass(const ObjCImplementationDecl *ID,
                                llvm37::Constant *Protocols,
                                ArrayRef<llvm37::Constant*> Methods);

  llvm37::Constant *GetMethodConstant(const ObjCMethodDecl *MD);

  llvm37::Constant *GetMethodDescriptionConstant(const ObjCMethodDecl *MD);

  /// EmitMethodList - Emit the method list for the given
  /// implementation. The return value has type MethodListPtrTy.
  llvm37::Constant *EmitMethodList(Twine Name,
                                 const char *Section,
                                 ArrayRef<llvm37::Constant*> Methods);

  /// EmitMethodDescList - Emit a method description list for a list of
  /// method declarations.
  ///  - TypeName: The name for the type containing the methods.
  ///  - IsProtocol: True iff these methods are for a protocol.
  ///  - ClassMethds: True iff these are class methods.
  ///  - Required: When true, only "required" methods are
  ///    listed. Similarly, when false only "optional" methods are
  ///    listed. For classes this should always be true.
  ///  - begin, end: The method list to output.
  ///
  /// The return value has type MethodDescriptionListPtrTy.
  llvm37::Constant *EmitMethodDescList(Twine Name,
                                     const char *Section,
                                     ArrayRef<llvm37::Constant*> Methods);

  /// GetOrEmitProtocol - Get the protocol object for the given
  /// declaration, emitting it if necessary. The return value has type
  /// ProtocolPtrTy.
  llvm37::Constant *GetOrEmitProtocol(const ObjCProtocolDecl *PD) override;

  /// GetOrEmitProtocolRef - Get a forward reference to the protocol
  /// object for the given declaration, emitting it if needed. These
  /// forward references will be filled in with empty bodies if no
  /// definition is seen. The return value has type ProtocolPtrTy.
  llvm37::Constant *GetOrEmitProtocolRef(const ObjCProtocolDecl *PD) override;

  /// EmitProtocolExtension - Generate the protocol extension
  /// structure used to store optional instance and class methods, and
  /// protocol properties. The return value has type
  /// ProtocolExtensionPtrTy.
  llvm37::Constant *
  EmitProtocolExtension(const ObjCProtocolDecl *PD,
                        ArrayRef<llvm37::Constant*> OptInstanceMethods,
                        ArrayRef<llvm37::Constant*> OptClassMethods,
                        ArrayRef<llvm37::Constant*> MethodTypesExt);

  /// EmitProtocolList - Generate the list of referenced
  /// protocols. The return value has type ProtocolListPtrTy.
  llvm37::Constant *EmitProtocolList(Twine Name,
                                   ObjCProtocolDecl::protocol_iterator begin,
                                   ObjCProtocolDecl::protocol_iterator end);

  /// EmitSelector - Return a Value*, of type ObjCTypes.SelectorPtrTy,
  /// for the given selector.
  llvm37::Value *EmitSelector(CodeGenFunction &CGF, Selector Sel, 
                            bool lval=false);

public:
  CGObjCMac(CodeGen::CodeGenModule &cgm);

  llvm37::Function *ModuleInitFunction() override;

  CodeGen::RValue GenerateMessageSend(CodeGen::CodeGenFunction &CGF,
                                      ReturnValueSlot Return,
                                      QualType ResultType,
                                      Selector Sel, llvm37::Value *Receiver,
                                      const CallArgList &CallArgs,
                                      const ObjCInterfaceDecl *Class,
                                      const ObjCMethodDecl *Method) override;

  CodeGen::RValue
  GenerateMessageSendSuper(CodeGen::CodeGenFunction &CGF,
                           ReturnValueSlot Return, QualType ResultType,
                           Selector Sel, const ObjCInterfaceDecl *Class,
                           bool isCategoryImpl, llvm37::Value *Receiver,
                           bool IsClassMessage, const CallArgList &CallArgs,
                           const ObjCMethodDecl *Method) override;

  llvm37::Value *GetClass(CodeGenFunction &CGF,
                        const ObjCInterfaceDecl *ID) override;

  llvm37::Value *GetSelector(CodeGenFunction &CGF, Selector Sel,
                           bool lval = false) override;

  /// The NeXT/Apple runtimes do not support typed selectors; just emit an
  /// untyped one.
  llvm37::Value *GetSelector(CodeGenFunction &CGF,
                           const ObjCMethodDecl *Method) override;

  llvm37::Constant *GetEHType(QualType T) override;

  void GenerateCategory(const ObjCCategoryImplDecl *CMD) override;

  void GenerateClass(const ObjCImplementationDecl *ClassDecl) override;

  void RegisterAlias(const ObjCCompatibleAliasDecl *OAD) override {}

  llvm37::Value *GenerateProtocolRef(CodeGenFunction &CGF,
                                   const ObjCProtocolDecl *PD) override;

  llvm37::Constant *GetPropertyGetFunction() override;
  llvm37::Constant *GetPropertySetFunction() override;
  llvm37::Constant *GetOptimizedPropertySetFunction(bool atomic,
                                                  bool copy) override;
  llvm37::Constant *GetGetStructFunction() override;
  llvm37::Constant *GetSetStructFunction() override;
  llvm37::Constant *GetCppAtomicObjectGetFunction() override;
  llvm37::Constant *GetCppAtomicObjectSetFunction() override;
  llvm37::Constant *EnumerationMutationFunction() override;

  void EmitTryStmt(CodeGen::CodeGenFunction &CGF,
                   const ObjCAtTryStmt &S) override;
  void EmitSynchronizedStmt(CodeGen::CodeGenFunction &CGF,
                            const ObjCAtSynchronizedStmt &S) override;
  void EmitTryOrSynchronizedStmt(CodeGen::CodeGenFunction &CGF, const Stmt &S);
  void EmitThrowStmt(CodeGen::CodeGenFunction &CGF, const ObjCAtThrowStmt &S,
                     bool ClearInsertionPoint=true) override;
  llvm37::Value * EmitObjCWeakRead(CodeGen::CodeGenFunction &CGF,
                                 llvm37::Value *AddrWeakObj) override;
  void EmitObjCWeakAssign(CodeGen::CodeGenFunction &CGF,
                          llvm37::Value *src, llvm37::Value *dst) override;
  void EmitObjCGlobalAssign(CodeGen::CodeGenFunction &CGF,
                            llvm37::Value *src, llvm37::Value *dest,
                            bool threadlocal = false) override;
  void EmitObjCIvarAssign(CodeGen::CodeGenFunction &CGF,
                          llvm37::Value *src, llvm37::Value *dest,
                          llvm37::Value *ivarOffset) override;
  void EmitObjCStrongCastAssign(CodeGen::CodeGenFunction &CGF,
                                llvm37::Value *src, llvm37::Value *dest) override;
  void EmitGCMemmoveCollectable(CodeGen::CodeGenFunction &CGF,
                                llvm37::Value *dest, llvm37::Value *src,
                                llvm37::Value *size) override;

  LValue EmitObjCValueForIvar(CodeGen::CodeGenFunction &CGF, QualType ObjectTy,
                              llvm37::Value *BaseValue, const ObjCIvarDecl *Ivar,
                              unsigned CVRQualifiers) override;
  llvm37::Value *EmitIvarOffset(CodeGen::CodeGenFunction &CGF,
                              const ObjCInterfaceDecl *Interface,
                              const ObjCIvarDecl *Ivar) override;

  /// GetClassGlobal - Return the global variable for the Objective-C
  /// class of the given name.
  llvm37::GlobalVariable *GetClassGlobal(const std::string &Name,
                                       bool Weak = false) override {
    llvm37_unreachable("CGObjCMac::GetClassGlobal");
  }
};

class CGObjCNonFragileABIMac : public CGObjCCommonMac {
private:
  ObjCNonFragileABITypesHelper ObjCTypes;
  llvm37::GlobalVariable* ObjCEmptyCacheVar;
  llvm37::GlobalVariable* ObjCEmptyVtableVar;

  /// SuperClassReferences - uniqued super class references.
  llvm37::DenseMap<IdentifierInfo*, llvm37::GlobalVariable*> SuperClassReferences;

  /// MetaClassReferences - uniqued meta class references.
  llvm37::DenseMap<IdentifierInfo*, llvm37::GlobalVariable*> MetaClassReferences;

  /// EHTypeReferences - uniqued class ehtype references.
  llvm37::DenseMap<IdentifierInfo*, llvm37::GlobalVariable*> EHTypeReferences;

  /// VTableDispatchMethods - List of methods for which we generate
  /// vtable-based message dispatch.
  llvm37::DenseSet<Selector> VTableDispatchMethods;

  /// DefinedMetaClasses - List of defined meta-classes.
  std::vector<llvm37::GlobalValue*> DefinedMetaClasses;
  
  /// isVTableDispatchedSelector - Returns true if SEL is a
  /// vtable-based selector.
  bool isVTableDispatchedSelector(Selector Sel);

  /// FinishNonFragileABIModule - Write out global data structures at the end of
  /// processing a translation unit.
  void FinishNonFragileABIModule();

  /// AddModuleClassList - Add the given list of class pointers to the
  /// module with the provided symbol and section names.
  void AddModuleClassList(ArrayRef<llvm37::GlobalValue*> Container,
                          const char *SymbolName,
                          const char *SectionName);

  llvm37::GlobalVariable * BuildClassRoTInitializer(unsigned flags,
                                              unsigned InstanceStart,
                                              unsigned InstanceSize,
                                              const ObjCImplementationDecl *ID);
  llvm37::GlobalVariable * BuildClassMetaData(const std::string &ClassName,
                                            llvm37::Constant *IsAGV,
                                            llvm37::Constant *SuperClassGV,
                                            llvm37::Constant *ClassRoGV,
                                            bool HiddenVisibility,
                                            bool Weak);

  llvm37::Constant *GetMethodConstant(const ObjCMethodDecl *MD);

  llvm37::Constant *GetMethodDescriptionConstant(const ObjCMethodDecl *MD);

  /// EmitMethodList - Emit the method list for the given
  /// implementation. The return value has type MethodListnfABITy.
  llvm37::Constant *EmitMethodList(Twine Name,
                                 const char *Section,
                                 ArrayRef<llvm37::Constant*> Methods);
  /// EmitIvarList - Emit the ivar list for the given
  /// implementation. If ForClass is true the list of class ivars
  /// (i.e. metaclass ivars) is emitted, otherwise the list of
  /// interface ivars will be emitted. The return value has type
  /// IvarListnfABIPtrTy.
  llvm37::Constant *EmitIvarList(const ObjCImplementationDecl *ID);

  llvm37::Constant *EmitIvarOffsetVar(const ObjCInterfaceDecl *ID,
                                    const ObjCIvarDecl *Ivar,
                                    unsigned long int offset);

  /// GetOrEmitProtocol - Get the protocol object for the given
  /// declaration, emitting it if necessary. The return value has type
  /// ProtocolPtrTy.
  llvm37::Constant *GetOrEmitProtocol(const ObjCProtocolDecl *PD) override;

  /// GetOrEmitProtocolRef - Get a forward reference to the protocol
  /// object for the given declaration, emitting it if needed. These
  /// forward references will be filled in with empty bodies if no
  /// definition is seen. The return value has type ProtocolPtrTy.
  llvm37::Constant *GetOrEmitProtocolRef(const ObjCProtocolDecl *PD) override;

  /// EmitProtocolList - Generate the list of referenced
  /// protocols. The return value has type ProtocolListPtrTy.
  llvm37::Constant *EmitProtocolList(Twine Name,
                                   ObjCProtocolDecl::protocol_iterator begin,
                                   ObjCProtocolDecl::protocol_iterator end);

  CodeGen::RValue EmitVTableMessageSend(CodeGen::CodeGenFunction &CGF,
                                        ReturnValueSlot Return,
                                        QualType ResultType,
                                        Selector Sel,
                                        llvm37::Value *Receiver,
                                        QualType Arg0Ty,
                                        bool IsSuper,
                                        const CallArgList &CallArgs,
                                        const ObjCMethodDecl *Method);
  
  /// GetClassGlobal - Return the global variable for the Objective-C
  /// class of the given name.
  llvm37::GlobalVariable *GetClassGlobal(const std::string &Name,
                                       bool Weak = false) override;

  /// EmitClassRef - Return a Value*, of type ObjCTypes.ClassPtrTy,
  /// for the given class reference.
  llvm37::Value *EmitClassRef(CodeGenFunction &CGF,
                            const ObjCInterfaceDecl *ID);
  
  llvm37::Value *EmitClassRefFromId(CodeGenFunction &CGF,
                                  IdentifierInfo *II, bool Weak,
                                  const ObjCInterfaceDecl *ID);

  llvm37::Value *EmitNSAutoreleasePoolClassRef(CodeGenFunction &CGF) override;

  /// EmitSuperClassRef - Return a Value*, of type ObjCTypes.ClassPtrTy,
  /// for the given super class reference.
  llvm37::Value *EmitSuperClassRef(CodeGenFunction &CGF,
                                 const ObjCInterfaceDecl *ID);

  /// EmitMetaClassRef - Return a Value * of the address of _class_t
  /// meta-data
  llvm37::Value *EmitMetaClassRef(CodeGenFunction &CGF,
                                const ObjCInterfaceDecl *ID, bool Weak);

  /// ObjCIvarOffsetVariable - Returns the ivar offset variable for
  /// the given ivar.
  ///
  llvm37::GlobalVariable * ObjCIvarOffsetVariable(
    const ObjCInterfaceDecl *ID,
    const ObjCIvarDecl *Ivar);

  /// EmitSelector - Return a Value*, of type ObjCTypes.SelectorPtrTy,
  /// for the given selector.
  llvm37::Value *EmitSelector(CodeGenFunction &CGF, Selector Sel, 
                            bool lval=false);

  /// GetInterfaceEHType - Get the cached ehtype for the given Objective-C
  /// interface. The return value has type EHTypePtrTy.
  llvm37::Constant *GetInterfaceEHType(const ObjCInterfaceDecl *ID,
                                  bool ForDefinition);

  const char *getMetaclassSymbolPrefix() const {
    return "OBJC_METACLASS_$_";
  }

  const char *getClassSymbolPrefix() const {
    return "OBJC_CLASS_$_";
  }

  void GetClassSizeInfo(const ObjCImplementationDecl *OID,
                        uint32_t &InstanceStart,
                        uint32_t &InstanceSize);

  // Shamelessly stolen from Analysis/CFRefCount.cpp
  Selector GetNullarySelector(const char* name) const {
    IdentifierInfo* II = &CGM.getContext().Idents.get(name);
    return CGM.getContext().Selectors.getSelector(0, &II);
  }

  Selector GetUnarySelector(const char* name) const {
    IdentifierInfo* II = &CGM.getContext().Idents.get(name);
    return CGM.getContext().Selectors.getSelector(1, &II);
  }

  /// ImplementationIsNonLazy - Check whether the given category or
  /// class implementation is "non-lazy".
  bool ImplementationIsNonLazy(const ObjCImplDecl *OD) const;

  bool IsIvarOffsetKnownIdempotent(const CodeGen::CodeGenFunction &CGF,
                                   const ObjCIvarDecl *IV) {
    // Annotate the load as an invariant load iff inside an instance method
    // and ivar belongs to instance method's class and one of its super class.
    // This check is needed because the ivar offset is a lazily
    // initialised value that may depend on objc_msgSend to perform a fixup on
    // the first message dispatch.
    //
    // An additional opportunity to mark the load as invariant arises when the
    // base of the ivar access is a parameter to an Objective C method.
    // However, because the parameters are not available in the current
    // interface, we cannot perform this check.
    if (const ObjCMethodDecl *MD =
          dyn_cast_or_null<ObjCMethodDecl>(CGF.CurFuncDecl))
      if (MD->isInstanceMethod())
        if (const ObjCInterfaceDecl *ID = MD->getClassInterface())
          return IV->getContainingInterface()->isSuperClassOf(ID);
    return false;
  }

public:
  CGObjCNonFragileABIMac(CodeGen::CodeGenModule &cgm);
  // FIXME. All stubs for now!
  llvm37::Function *ModuleInitFunction() override;

  CodeGen::RValue GenerateMessageSend(CodeGen::CodeGenFunction &CGF,
                                      ReturnValueSlot Return,
                                      QualType ResultType, Selector Sel,
                                      llvm37::Value *Receiver,
                                      const CallArgList &CallArgs,
                                      const ObjCInterfaceDecl *Class,
                                      const ObjCMethodDecl *Method) override;

  CodeGen::RValue
  GenerateMessageSendSuper(CodeGen::CodeGenFunction &CGF,
                           ReturnValueSlot Return, QualType ResultType,
                           Selector Sel, const ObjCInterfaceDecl *Class,
                           bool isCategoryImpl, llvm37::Value *Receiver,
                           bool IsClassMessage, const CallArgList &CallArgs,
                           const ObjCMethodDecl *Method) override;

  llvm37::Value *GetClass(CodeGenFunction &CGF,
                        const ObjCInterfaceDecl *ID) override;

  llvm37::Value *GetSelector(CodeGenFunction &CGF, Selector Sel,
                           bool lvalue = false) override
    { return EmitSelector(CGF, Sel, lvalue); }

  /// The NeXT/Apple runtimes do not support typed selectors; just emit an
  /// untyped one.
  llvm37::Value *GetSelector(CodeGenFunction &CGF,
                           const ObjCMethodDecl *Method) override
    { return EmitSelector(CGF, Method->getSelector()); }

  void GenerateCategory(const ObjCCategoryImplDecl *CMD) override;

  void GenerateClass(const ObjCImplementationDecl *ClassDecl) override;

  void RegisterAlias(const ObjCCompatibleAliasDecl *OAD) override {}

  llvm37::Value *GenerateProtocolRef(CodeGenFunction &CGF,
                                   const ObjCProtocolDecl *PD) override;

  llvm37::Constant *GetEHType(QualType T) override;

  llvm37::Constant *GetPropertyGetFunction() override {
    return ObjCTypes.getGetPropertyFn();
  }
  llvm37::Constant *GetPropertySetFunction() override {
    return ObjCTypes.getSetPropertyFn();
  }

  llvm37::Constant *GetOptimizedPropertySetFunction(bool atomic,
                                                  bool copy) override {
    return ObjCTypes.getOptimizedSetPropertyFn(atomic, copy);
  }

  llvm37::Constant *GetSetStructFunction() override {
    return ObjCTypes.getCopyStructFn();
  }
  llvm37::Constant *GetGetStructFunction() override {
    return ObjCTypes.getCopyStructFn();
  }
  llvm37::Constant *GetCppAtomicObjectSetFunction() override {
    return ObjCTypes.getCppAtomicObjectFunction();
  }
  llvm37::Constant *GetCppAtomicObjectGetFunction() override {
    return ObjCTypes.getCppAtomicObjectFunction();
  }

  llvm37::Constant *EnumerationMutationFunction() override {
    return ObjCTypes.getEnumerationMutationFn();
  }

  void EmitTryStmt(CodeGen::CodeGenFunction &CGF,
                   const ObjCAtTryStmt &S) override;
  void EmitSynchronizedStmt(CodeGen::CodeGenFunction &CGF,
                            const ObjCAtSynchronizedStmt &S) override;
  void EmitThrowStmt(CodeGen::CodeGenFunction &CGF, const ObjCAtThrowStmt &S,
                     bool ClearInsertionPoint=true) override;
  llvm37::Value * EmitObjCWeakRead(CodeGen::CodeGenFunction &CGF,
                                 llvm37::Value *AddrWeakObj) override;
  void EmitObjCWeakAssign(CodeGen::CodeGenFunction &CGF,
                          llvm37::Value *src, llvm37::Value *dst) override;
  void EmitObjCGlobalAssign(CodeGen::CodeGenFunction &CGF,
                            llvm37::Value *src, llvm37::Value *dest,
                            bool threadlocal = false) override;
  void EmitObjCIvarAssign(CodeGen::CodeGenFunction &CGF,
                          llvm37::Value *src, llvm37::Value *dest,
                          llvm37::Value *ivarOffset) override;
  void EmitObjCStrongCastAssign(CodeGen::CodeGenFunction &CGF,
                                llvm37::Value *src, llvm37::Value *dest) override;
  void EmitGCMemmoveCollectable(CodeGen::CodeGenFunction &CGF,
                                llvm37::Value *dest, llvm37::Value *src,
                                llvm37::Value *size) override;
  LValue EmitObjCValueForIvar(CodeGen::CodeGenFunction &CGF, QualType ObjectTy,
                              llvm37::Value *BaseValue, const ObjCIvarDecl *Ivar,
                              unsigned CVRQualifiers) override;
  llvm37::Value *EmitIvarOffset(CodeGen::CodeGenFunction &CGF,
                              const ObjCInterfaceDecl *Interface,
                              const ObjCIvarDecl *Ivar) override;
};

/// A helper class for performing the null-initialization of a return
/// value.
struct NullReturnState {
  llvm37::BasicBlock *NullBB;
  NullReturnState() : NullBB(nullptr) {}

  /// Perform a null-check of the given receiver.
  void init(CodeGenFunction &CGF, llvm37::Value *receiver) {
    // Make blocks for the null-receiver and call edges.
    NullBB = CGF.createBasicBlock("msgSend.null-receiver");
    llvm37::BasicBlock *callBB = CGF.createBasicBlock("msgSend.call");

    // Check for a null receiver and, if there is one, jump to the
    // null-receiver block.  There's no point in trying to avoid it:
    // we're always going to put *something* there, because otherwise
    // we shouldn't have done this null-check in the first place.
    llvm37::Value *isNull = CGF.Builder.CreateIsNull(receiver);
    CGF.Builder.CreateCondBr(isNull, NullBB, callBB);

    // Otherwise, start performing the call.
    CGF.EmitBlock(callBB);
  }

  /// Complete the null-return operation.  It is valid to call this
  /// regardless of whether 'init' has been called.
  RValue complete(CodeGenFunction &CGF, RValue result, QualType resultType,
                  const CallArgList &CallArgs,
                  const ObjCMethodDecl *Method) {
    // If we never had to do a null-check, just use the raw result.
    if (!NullBB) return result;

    // The continuation block.  This will be left null if we don't have an
    // IP, which can happen if the method we're calling is marked noreturn.
    llvm37::BasicBlock *contBB = nullptr;

    // Finish the call path.
    llvm37::BasicBlock *callBB = CGF.Builder.GetInsertBlock();
    if (callBB) {
      contBB = CGF.createBasicBlock("msgSend.cont");
      CGF.Builder.CreateBr(contBB);
    }

    // Okay, start emitting the null-receiver block.
    CGF.EmitBlock(NullBB);
    
    // Release any consumed arguments we've got.
    if (Method) {
      CallArgList::const_iterator I = CallArgs.begin();
      for (ObjCMethodDecl::param_const_iterator i = Method->param_begin(),
           e = Method->param_end(); i != e; ++i, ++I) {
        const ParmVarDecl *ParamDecl = (*i);
        if (ParamDecl->hasAttr<NSConsumedAttr>()) {
          RValue RV = I->RV;
          assert(RV.isScalar() && 
                 "NullReturnState::complete - arg not on object");
          CGF.EmitARCRelease(RV.getScalarVal(), ARCImpreciseLifetime);
        }
      }
    }

    // The phi code below assumes that we haven't needed any control flow yet.
    assert(CGF.Builder.GetInsertBlock() == NullBB);

    // If we've got a void return, just jump to the continuation block.
    if (result.isScalar() && resultType->isVoidType()) {
      // No jumps required if the message-send was noreturn.
      if (contBB) CGF.EmitBlock(contBB);
      return result;
    }

    // If we've got a scalar return, build a phi.
    if (result.isScalar()) {
      // Derive the null-initialization value.
      llvm37::Constant *null = CGF.CGM.EmitNullConstant(resultType);

      // If no join is necessary, just flow out.
      if (!contBB) return RValue::get(null);

      // Otherwise, build a phi.
      CGF.EmitBlock(contBB);
      llvm37::PHINode *phi = CGF.Builder.CreatePHI(null->getType(), 2);
      phi->addIncoming(result.getScalarVal(), callBB);
      phi->addIncoming(null, NullBB);
      return RValue::get(phi);
    }

    // If we've got an aggregate return, null the buffer out.
    // FIXME: maybe we should be doing things differently for all the
    // cases where the ABI has us returning (1) non-agg values in
    // memory or (2) agg values in registers.
    if (result.isAggregate()) {
      assert(result.isAggregate() && "null init of non-aggregate result?");
      CGF.EmitNullInitialization(result.getAggregateAddr(), resultType);
      if (contBB) CGF.EmitBlock(contBB);
      return result;
    }

    // Complex types.
    CGF.EmitBlock(contBB);
    CodeGenFunction::ComplexPairTy callResult = result.getComplexVal();

    // Find the scalar type and its zero value.
    llvm37::Type *scalarTy = callResult.first->getType();
    llvm37::Constant *scalarZero = llvm37::Constant::getNullValue(scalarTy);

    // Build phis for both coordinates.
    llvm37::PHINode *real = CGF.Builder.CreatePHI(scalarTy, 2);
    real->addIncoming(callResult.first, callBB);
    real->addIncoming(scalarZero, NullBB);
    llvm37::PHINode *imag = CGF.Builder.CreatePHI(scalarTy, 2);
    imag->addIncoming(callResult.second, callBB);
    imag->addIncoming(scalarZero, NullBB);
    return RValue::getComplex(real, imag);
  }
};

} // end anonymous namespace

/* *** Helper Functions *** */

/// getConstantGEP() - Help routine to construct simple GEPs.
static llvm37::Constant *getConstantGEP(llvm37::LLVM37Context &VMContext,
                                      llvm37::GlobalVariable *C, unsigned idx0,
                                      unsigned idx1) {
  llvm37::Value *Idxs[] = {
    llvm37::ConstantInt::get(llvm37::Type::getInt32Ty(VMContext), idx0),
    llvm37::ConstantInt::get(llvm37::Type::getInt32Ty(VMContext), idx1)
  };
  return llvm37::ConstantExpr::getGetElementPtr(C->getValueType(), C, Idxs);
}

/// hasObjCExceptionAttribute - Return true if this class or any super
/// class has the __objc_exception__ attribute.
static bool hasObjCExceptionAttribute(ASTContext &Context,
                                      const ObjCInterfaceDecl *OID) {
  if (OID->hasAttr<ObjCExceptionAttr>())
    return true;
  if (const ObjCInterfaceDecl *Super = OID->getSuperClass())
    return hasObjCExceptionAttribute(Context, Super);
  return false;
}

/* *** CGObjCMac Public Interface *** */

CGObjCMac::CGObjCMac(CodeGen::CodeGenModule &cgm) : CGObjCCommonMac(cgm),
                                                    ObjCTypes(cgm) {
  ObjCABI = 1;
  EmitImageInfo();
}

/// GetClass - Return a reference to the class for the given interface
/// decl.
llvm37::Value *CGObjCMac::GetClass(CodeGenFunction &CGF,
                                 const ObjCInterfaceDecl *ID) {
  return EmitClassRef(CGF, ID);
}

/// GetSelector - Return the pointer to the unique'd string for this selector.
llvm37::Value *CGObjCMac::GetSelector(CodeGenFunction &CGF, Selector Sel, 
                                    bool lval) {
  return EmitSelector(CGF, Sel, lval);
}
llvm37::Value *CGObjCMac::GetSelector(CodeGenFunction &CGF, const ObjCMethodDecl
                                    *Method) {
  return EmitSelector(CGF, Method->getSelector());
}

llvm37::Constant *CGObjCMac::GetEHType(QualType T) {
  if (T->isObjCIdType() ||
      T->isObjCQualifiedIdType()) {
    return CGM.GetAddrOfRTTIDescriptor(
              CGM.getContext().getObjCIdRedefinitionType(), /*ForEH=*/true);
  }
  if (T->isObjCClassType() ||
      T->isObjCQualifiedClassType()) {
    return CGM.GetAddrOfRTTIDescriptor(
             CGM.getContext().getObjCClassRedefinitionType(), /*ForEH=*/true);
  }
  if (T->isObjCObjectPointerType())
    return CGM.GetAddrOfRTTIDescriptor(T,  /*ForEH=*/true);
  
  llvm37_unreachable("asking for catch type for ObjC type in fragile runtime");
}

/// Generate a constant CFString object.
/*
  struct __builtin_CFString {
  const int *isa; // point to __CFConstantStringClassReference
  int flags;
  const char *str;
  long length;
  };
*/

/// or Generate a constant NSString object.
/*
   struct __builtin_NSString {
     const int *isa; // point to __NSConstantStringClassReference
     const char *str;
     unsigned int length;
   };
*/

llvm37::Constant *CGObjCCommonMac::GenerateConstantString(
  const StringLiteral *SL) {
  return (CGM.getLangOpts().NoConstantCFStrings == 0 ? 
          CGM.GetAddrOfConstantCFString(SL) :
          CGM.GetAddrOfConstantString(SL));
}

enum {
  kCFTaggedObjectID_Integer = (1 << 1) + 1
};

/// Generates a message send where the super is the receiver.  This is
/// a message send to self with special delivery semantics indicating
/// which class's method should be called.
CodeGen::RValue
CGObjCMac::GenerateMessageSendSuper(CodeGen::CodeGenFunction &CGF,
                                    ReturnValueSlot Return,
                                    QualType ResultType,
                                    Selector Sel,
                                    const ObjCInterfaceDecl *Class,
                                    bool isCategoryImpl,
                                    llvm37::Value *Receiver,
                                    bool IsClassMessage,
                                    const CodeGen::CallArgList &CallArgs,
                                    const ObjCMethodDecl *Method) {
  // Create and init a super structure; this is a (receiver, class)
  // pair we will pass to objc_msgSendSuper.
  llvm37::Value *ObjCSuper =
    CGF.CreateTempAlloca(ObjCTypes.SuperTy, "objc_super");
  llvm37::Value *ReceiverAsObject =
    CGF.Builder.CreateBitCast(Receiver, ObjCTypes.ObjectPtrTy);
  CGF.Builder.CreateStore(
      ReceiverAsObject,
      CGF.Builder.CreateStructGEP(ObjCTypes.SuperTy, ObjCSuper, 0));

  // If this is a class message the metaclass is passed as the target.
  llvm37::Value *Target;
  if (IsClassMessage) {
    if (isCategoryImpl) {
      // Message sent to 'super' in a class method defined in a category
      // implementation requires an odd treatment.
      // If we are in a class method, we must retrieve the
      // _metaclass_ for the current class, pointed at by
      // the class's "isa" pointer.  The following assumes that
      // isa" is the first ivar in a class (which it must be).
      Target = EmitClassRef(CGF, Class->getSuperClass());
      Target = CGF.Builder.CreateStructGEP(ObjCTypes.ClassTy, Target, 0);
      Target = CGF.Builder.CreateLoad(Target);
    } else {
      llvm37::Constant *MetaClassPtr = EmitMetaClassRef(Class);
      llvm37::Value *SuperPtr =
          CGF.Builder.CreateStructGEP(ObjCTypes.ClassTy, MetaClassPtr, 1);
      llvm37::Value *Super = CGF.Builder.CreateLoad(SuperPtr);
      Target = Super;
    }
  } else if (isCategoryImpl)
    Target = EmitClassRef(CGF, Class->getSuperClass());
  else {
    llvm37::Value *ClassPtr = EmitSuperClassRef(Class);
    ClassPtr = CGF.Builder.CreateStructGEP(ObjCTypes.ClassTy, ClassPtr, 1);
    Target = CGF.Builder.CreateLoad(ClassPtr);
  }
  // FIXME: We shouldn't need to do this cast, rectify the ASTContext and
  // ObjCTypes types.
  llvm37::Type *ClassTy =
    CGM.getTypes().ConvertType(CGF.getContext().getObjCClassType());
  Target = CGF.Builder.CreateBitCast(Target, ClassTy);
  CGF.Builder.CreateStore(
      Target, CGF.Builder.CreateStructGEP(ObjCTypes.SuperTy, ObjCSuper, 1));
  return EmitMessageSend(CGF, Return, ResultType,
                         EmitSelector(CGF, Sel),
                         ObjCSuper, ObjCTypes.SuperPtrCTy,
                         true, CallArgs, Method, ObjCTypes);
}

/// Generate code for a message send expression.
CodeGen::RValue CGObjCMac::GenerateMessageSend(CodeGen::CodeGenFunction &CGF,
                                               ReturnValueSlot Return,
                                               QualType ResultType,
                                               Selector Sel,
                                               llvm37::Value *Receiver,
                                               const CallArgList &CallArgs,
                                               const ObjCInterfaceDecl *Class,
                                               const ObjCMethodDecl *Method) {
  return EmitMessageSend(CGF, Return, ResultType,
                         EmitSelector(CGF, Sel),
                         Receiver, CGF.getContext().getObjCIdType(),
                         false, CallArgs, Method, ObjCTypes);
}

CodeGen::RValue
CGObjCCommonMac::EmitMessageSend(CodeGen::CodeGenFunction &CGF,
                                 ReturnValueSlot Return,
                                 QualType ResultType,
                                 llvm37::Value *Sel,
                                 llvm37::Value *Arg0,
                                 QualType Arg0Ty,
                                 bool IsSuper,
                                 const CallArgList &CallArgs,
                                 const ObjCMethodDecl *Method,
                                 const ObjCCommonTypesHelper &ObjCTypes) {
  CallArgList ActualArgs;
  if (!IsSuper)
    Arg0 = CGF.Builder.CreateBitCast(Arg0, ObjCTypes.ObjectPtrTy);
  ActualArgs.add(RValue::get(Arg0), Arg0Ty);
  ActualArgs.add(RValue::get(Sel), CGF.getContext().getObjCSelType());
  ActualArgs.addFrom(CallArgs);

  // If we're calling a method, use the formal signature.
  MessageSendInfo MSI = getMessageSendInfo(Method, ResultType, ActualArgs);

  if (Method)
    assert(CGM.getContext().getCanonicalType(Method->getReturnType()) ==
               CGM.getContext().getCanonicalType(ResultType) &&
           "Result type mismatch!");

  NullReturnState nullReturn;

  llvm37::Constant *Fn = nullptr;
  if (CGM.ReturnSlotInterferesWithArgs(MSI.CallInfo)) {
    if (!IsSuper) nullReturn.init(CGF, Arg0);
    Fn = (ObjCABI == 2) ?  ObjCTypes.getSendStretFn2(IsSuper)
      : ObjCTypes.getSendStretFn(IsSuper);
  } else if (CGM.ReturnTypeUsesFPRet(ResultType)) {
    Fn = (ObjCABI == 2) ? ObjCTypes.getSendFpretFn2(IsSuper)
      : ObjCTypes.getSendFpretFn(IsSuper);
  } else if (CGM.ReturnTypeUsesFP2Ret(ResultType)) {
    Fn = (ObjCABI == 2) ? ObjCTypes.getSendFp2RetFn2(IsSuper)
      : ObjCTypes.getSendFp2retFn(IsSuper);
  } else {
    // arm64 uses objc_msgSend for stret methods and yet null receiver check
    // must be made for it.
    if (!IsSuper && CGM.ReturnTypeUsesSRet(MSI.CallInfo))
      nullReturn.init(CGF, Arg0);
    Fn = (ObjCABI == 2) ? ObjCTypes.getSendFn2(IsSuper)
      : ObjCTypes.getSendFn(IsSuper);
  }
  
  bool requiresnullCheck = false;
  if (CGM.getLangOpts().ObjCAutoRefCount && Method)
    for (const auto *ParamDecl : Method->params()) {
      if (ParamDecl->hasAttr<NSConsumedAttr>()) {
        if (!nullReturn.NullBB)
          nullReturn.init(CGF, Arg0);
        requiresnullCheck = true;
        break;
      }
    }
  
  Fn = llvm37::ConstantExpr::getBitCast(Fn, MSI.MessengerType);
  RValue rvalue = CGF.EmitCall(MSI.CallInfo, Fn, Return, ActualArgs);
  return nullReturn.complete(CGF, rvalue, ResultType, CallArgs,
                             requiresnullCheck ? Method : nullptr);
}

static Qualifiers::GC GetGCAttrTypeForType(ASTContext &Ctx, QualType FQT) {
  if (FQT.isObjCGCStrong())
    return Qualifiers::Strong;
  
  if (FQT.isObjCGCWeak() || FQT.getObjCLifetime() == Qualifiers::OCL_Weak)
    return Qualifiers::Weak;
  
  // check for __unsafe_unretained
  if (FQT.getObjCLifetime() == Qualifiers::OCL_ExplicitNone)
    return Qualifiers::GCNone;
  
  if (FQT->isObjCObjectPointerType() || FQT->isBlockPointerType())
    return Qualifiers::Strong;
  
  if (const PointerType *PT = FQT->getAs<PointerType>())
    return GetGCAttrTypeForType(Ctx, PT->getPointeeType());
  
  return Qualifiers::GCNone;
}

llvm37::Constant *CGObjCCommonMac::BuildGCBlockLayout(CodeGenModule &CGM,
                                                const CGBlockInfo &blockInfo) {
  
  llvm37::Constant *nullPtr = llvm37::Constant::getNullValue(CGM.Int8PtrTy);
  if (CGM.getLangOpts().getGC() == LangOptions::NonGC &&
      !CGM.getLangOpts().ObjCAutoRefCount)
    return nullPtr;

  bool hasUnion = false;
  SkipIvars.clear();
  IvarsInfo.clear();
  unsigned WordSizeInBits = CGM.getTarget().getPointerWidth(0);
  unsigned ByteSizeInBits = CGM.getTarget().getCharWidth();
  
  // __isa is the first field in block descriptor and must assume by runtime's
  // convention that it is GC'able.
  IvarsInfo.push_back(GC_IVAR(0, 1));

  const BlockDecl *blockDecl = blockInfo.getBlockDecl();

  // Calculate the basic layout of the block structure.
  const llvm37::StructLayout *layout =
    CGM.getDataLayout().getStructLayout(blockInfo.StructureType);

  // Ignore the optional 'this' capture: C++ objects are not assumed
  // to be GC'ed.

  // Walk the captured variables.
  for (const auto &CI : blockDecl->captures()) {
    const VarDecl *variable = CI.getVariable();
    QualType type = variable->getType();

    const CGBlockInfo::Capture &capture = blockInfo.getCapture(variable);

    // Ignore constant captures.
    if (capture.isConstant()) continue;

    uint64_t fieldOffset = layout->getElementOffset(capture.getIndex());

    // __block variables are passed by their descriptor address.
    if (CI.isByRef()) {
      IvarsInfo.push_back(GC_IVAR(fieldOffset, /*size in words*/ 1));
      continue;
    }

    assert(!type->isArrayType() && "array variable should not be caught");
    if (const RecordType *record = type->getAs<RecordType>()) {
      BuildAggrIvarRecordLayout(record, fieldOffset, true, hasUnion);
      continue;
    }
      
    Qualifiers::GC GCAttr = GetGCAttrTypeForType(CGM.getContext(), type);
    unsigned fieldSize = CGM.getContext().getTypeSize(type);

    if (GCAttr == Qualifiers::Strong)
      IvarsInfo.push_back(GC_IVAR(fieldOffset,
                                  fieldSize / WordSizeInBits));
    else if (GCAttr == Qualifiers::GCNone || GCAttr == Qualifiers::Weak)
      SkipIvars.push_back(GC_IVAR(fieldOffset,
                                  fieldSize / ByteSizeInBits));
  }
  
  if (IvarsInfo.empty())
    return nullPtr;

  // Sort on byte position; captures might not be allocated in order,
  // and unions can do funny things.
  llvm37::array_pod_sort(IvarsInfo.begin(), IvarsInfo.end());
  llvm37::array_pod_sort(SkipIvars.begin(), SkipIvars.end());
  
  std::string BitMap;
  llvm37::Constant *C = BuildIvarLayoutBitmap(BitMap);
  if (CGM.getLangOpts().ObjCGCBitmapPrint) {
    printf("\n block variable layout for block: ");
    const unsigned char *s = (const unsigned char*)BitMap.c_str();
    for (unsigned i = 0, e = BitMap.size(); i < e; i++)
      if (!(s[i] & 0xf0))
        printf("0x0%x%s", s[i], s[i] != 0 ? ", " : "");
      else
        printf("0x%x%s",  s[i], s[i] != 0 ? ", " : "");
    printf("\n");
  }
  
  return C;
}

/// getBlockCaptureLifetime - This routine returns life time of the captured
/// block variable for the purpose of block layout meta-data generation. FQT is
/// the type of the variable captured in the block.
Qualifiers::ObjCLifetime CGObjCCommonMac::getBlockCaptureLifetime(QualType FQT,
                                                                  bool ByrefLayout) {
  if (CGM.getLangOpts().ObjCAutoRefCount)
    return FQT.getObjCLifetime();
  
  // MRR.
  if (FQT->isObjCObjectPointerType() || FQT->isBlockPointerType())
    return ByrefLayout ? Qualifiers::OCL_ExplicitNone : Qualifiers::OCL_Strong;
  
  return Qualifiers::OCL_None;
}

void CGObjCCommonMac::UpdateRunSkipBlockVars(bool IsByref,
                                             Qualifiers::ObjCLifetime LifeTime,
                                             CharUnits FieldOffset,
                                             CharUnits FieldSize) {
  // __block variables are passed by their descriptor address.
  if (IsByref)
    RunSkipBlockVars.push_back(RUN_SKIP(BLOCK_LAYOUT_BYREF, FieldOffset,
                                        FieldSize));
  else if (LifeTime == Qualifiers::OCL_Strong)
    RunSkipBlockVars.push_back(RUN_SKIP(BLOCK_LAYOUT_STRONG, FieldOffset,
                                        FieldSize));
  else if (LifeTime == Qualifiers::OCL_Weak)
    RunSkipBlockVars.push_back(RUN_SKIP(BLOCK_LAYOUT_WEAK, FieldOffset,
                                        FieldSize));
  else if (LifeTime == Qualifiers::OCL_ExplicitNone)
    RunSkipBlockVars.push_back(RUN_SKIP(BLOCK_LAYOUT_UNRETAINED, FieldOffset,
                                        FieldSize));
  else
    RunSkipBlockVars.push_back(RUN_SKIP(BLOCK_LAYOUT_NON_OBJECT_BYTES,
                                        FieldOffset,
                                        FieldSize));
}

void CGObjCCommonMac::BuildRCRecordLayout(const llvm37::StructLayout *RecLayout,
                                          const RecordDecl *RD,
                                          ArrayRef<const FieldDecl*> RecFields,
                                          CharUnits BytePos, bool &HasUnion,
                                          bool ByrefLayout) {
  bool IsUnion = (RD && RD->isUnion());
  CharUnits MaxUnionSize = CharUnits::Zero();
  const FieldDecl *MaxField = nullptr;
  const FieldDecl *LastFieldBitfieldOrUnnamed = nullptr;
  CharUnits MaxFieldOffset = CharUnits::Zero();
  CharUnits LastBitfieldOrUnnamedOffset = CharUnits::Zero();
  
  if (RecFields.empty())
    return;
  unsigned ByteSizeInBits = CGM.getTarget().getCharWidth();
  
  for (unsigned i = 0, e = RecFields.size(); i != e; ++i) {
    const FieldDecl *Field = RecFields[i];
    // Note that 'i' here is actually the field index inside RD of Field,
    // although this dependency is hidden.
    const ASTRecordLayout &RL = CGM.getContext().getASTRecordLayout(RD);
    CharUnits FieldOffset =
      CGM.getContext().toCharUnitsFromBits(RL.getFieldOffset(i));
    
    // Skip over unnamed or bitfields
    if (!Field->getIdentifier() || Field->isBitField()) {
      LastFieldBitfieldOrUnnamed = Field;
      LastBitfieldOrUnnamedOffset = FieldOffset;
      continue;
    }

    LastFieldBitfieldOrUnnamed = nullptr;
    QualType FQT = Field->getType();
    if (FQT->isRecordType() || FQT->isUnionType()) {
      if (FQT->isUnionType())
        HasUnion = true;
      
      BuildRCBlockVarRecordLayout(FQT->getAs<RecordType>(),
                                  BytePos + FieldOffset, HasUnion);
      continue;
    }
    
    if (const ArrayType *Array = CGM.getContext().getAsArrayType(FQT)) {
      const ConstantArrayType *CArray =
        dyn_cast_or_null<ConstantArrayType>(Array);
      uint64_t ElCount = CArray->getSize().getZExtValue();
      assert(CArray && "only array with known element size is supported");
      FQT = CArray->getElementType();
      while (const ArrayType *Array = CGM.getContext().getAsArrayType(FQT)) {
        const ConstantArrayType *CArray =
          dyn_cast_or_null<ConstantArrayType>(Array);
        ElCount *= CArray->getSize().getZExtValue();
        FQT = CArray->getElementType();
      }
      if (FQT->isRecordType() && ElCount) {
        int OldIndex = RunSkipBlockVars.size() - 1;
        const RecordType *RT = FQT->getAs<RecordType>();
        BuildRCBlockVarRecordLayout(RT, BytePos + FieldOffset,
                                    HasUnion);
        
        // Replicate layout information for each array element. Note that
        // one element is already done.
        uint64_t ElIx = 1;
        for (int FirstIndex = RunSkipBlockVars.size() - 1 ;ElIx < ElCount; ElIx++) {
          CharUnits Size = CGM.getContext().getTypeSizeInChars(RT);
          for (int i = OldIndex+1; i <= FirstIndex; ++i)
            RunSkipBlockVars.push_back(
              RUN_SKIP(RunSkipBlockVars[i].opcode,
              RunSkipBlockVars[i].block_var_bytepos + Size*ElIx,
              RunSkipBlockVars[i].block_var_size));
        }
        continue;
      }
    }
    CharUnits FieldSize = CGM.getContext().getTypeSizeInChars(Field->getType());
    if (IsUnion) {
      CharUnits UnionIvarSize = FieldSize;
      if (UnionIvarSize > MaxUnionSize) {
        MaxUnionSize = UnionIvarSize;
        MaxField = Field;
        MaxFieldOffset = FieldOffset;
      }
    } else {
      UpdateRunSkipBlockVars(false,
                             getBlockCaptureLifetime(FQT, ByrefLayout),
                             BytePos + FieldOffset,
                             FieldSize);
    }
  }
  
  if (LastFieldBitfieldOrUnnamed) {
    if (LastFieldBitfieldOrUnnamed->isBitField()) {
      // Last field was a bitfield. Must update the info.
      uint64_t BitFieldSize
        = LastFieldBitfieldOrUnnamed->getBitWidthValue(CGM.getContext());
      unsigned UnsSize = (BitFieldSize / ByteSizeInBits) +
                        ((BitFieldSize % ByteSizeInBits) != 0);
      CharUnits Size = CharUnits::fromQuantity(UnsSize);
      Size += LastBitfieldOrUnnamedOffset;
      UpdateRunSkipBlockVars(false,
                             getBlockCaptureLifetime(LastFieldBitfieldOrUnnamed->getType(),
                                                     ByrefLayout),
                             BytePos + LastBitfieldOrUnnamedOffset,
                             Size);
    } else {
      assert(!LastFieldBitfieldOrUnnamed->getIdentifier() &&"Expected unnamed");
      // Last field was unnamed. Must update skip info.
      CharUnits FieldSize
        = CGM.getContext().getTypeSizeInChars(LastFieldBitfieldOrUnnamed->getType());
      UpdateRunSkipBlockVars(false,
                             getBlockCaptureLifetime(LastFieldBitfieldOrUnnamed->getType(),
                                                     ByrefLayout),
                             BytePos + LastBitfieldOrUnnamedOffset,
                             FieldSize);
    }
  }
  
  if (MaxField)
    UpdateRunSkipBlockVars(false,
                           getBlockCaptureLifetime(MaxField->getType(), ByrefLayout),
                           BytePos + MaxFieldOffset,
                           MaxUnionSize);
}

void CGObjCCommonMac::BuildRCBlockVarRecordLayout(const RecordType *RT,
                                                  CharUnits BytePos,
                                                  bool &HasUnion,
                                                  bool ByrefLayout) {
  const RecordDecl *RD = RT->getDecl();
  SmallVector<const FieldDecl*, 16> Fields(RD->fields());
  llvm37::Type *Ty = CGM.getTypes().ConvertType(QualType(RT, 0));
  const llvm37::StructLayout *RecLayout =
    CGM.getDataLayout().getStructLayout(cast<llvm37::StructType>(Ty));
  
  BuildRCRecordLayout(RecLayout, RD, Fields, BytePos, HasUnion, ByrefLayout);
}

/// InlineLayoutInstruction - This routine produce an inline instruction for the
/// block variable layout if it can. If not, it returns 0. Rules are as follow:
/// If ((uintptr_t) layout) < (1 << 12), the layout is inline. In the 64bit world,
/// an inline layout of value 0x0000000000000xyz is interpreted as follows:
/// x captured object pointers of BLOCK_LAYOUT_STRONG. Followed by
/// y captured object of BLOCK_LAYOUT_BYREF. Followed by
/// z captured object of BLOCK_LAYOUT_WEAK. If any of the above is missing, zero
/// replaces it. For example, 0x00000x00 means x BLOCK_LAYOUT_STRONG and no
/// BLOCK_LAYOUT_BYREF and no BLOCK_LAYOUT_WEAK objects are captured.
uint64_t CGObjCCommonMac::InlineLayoutInstruction(
                                    SmallVectorImpl<unsigned char> &Layout) {
  uint64_t Result = 0;
  if (Layout.size() <= 3) {
    unsigned size = Layout.size();
    unsigned strong_word_count = 0, byref_word_count=0, weak_word_count=0;
    unsigned char inst;
    enum BLOCK_LAYOUT_OPCODE opcode ;
    switch (size) {
      case 3:
        inst = Layout[0];
        opcode = (enum BLOCK_LAYOUT_OPCODE) (inst >> 4);
        if (opcode == BLOCK_LAYOUT_STRONG)
          strong_word_count = (inst & 0xF)+1;
        else
          return 0;
        inst = Layout[1];
        opcode = (enum BLOCK_LAYOUT_OPCODE) (inst >> 4);
        if (opcode == BLOCK_LAYOUT_BYREF)
          byref_word_count = (inst & 0xF)+1;
        else
          return 0;
        inst = Layout[2];
        opcode = (enum BLOCK_LAYOUT_OPCODE) (inst >> 4);
        if (opcode == BLOCK_LAYOUT_WEAK)
          weak_word_count = (inst & 0xF)+1;
        else
          return 0;
        break;
        
      case 2:
        inst = Layout[0];
        opcode = (enum BLOCK_LAYOUT_OPCODE) (inst >> 4);
        if (opcode == BLOCK_LAYOUT_STRONG) {
          strong_word_count = (inst & 0xF)+1;
          inst = Layout[1];
          opcode = (enum BLOCK_LAYOUT_OPCODE) (inst >> 4);
          if (opcode == BLOCK_LAYOUT_BYREF)
            byref_word_count = (inst & 0xF)+1;
          else if (opcode == BLOCK_LAYOUT_WEAK)
            weak_word_count = (inst & 0xF)+1;
          else
            return 0;
        }
        else if (opcode == BLOCK_LAYOUT_BYREF) {
          byref_word_count = (inst & 0xF)+1;
          inst = Layout[1];
          opcode = (enum BLOCK_LAYOUT_OPCODE) (inst >> 4);
          if (opcode == BLOCK_LAYOUT_WEAK)
            weak_word_count = (inst & 0xF)+1;
          else
            return 0;
        }
        else
          return 0;
        break;
        
      case 1:
        inst = Layout[0];
        opcode = (enum BLOCK_LAYOUT_OPCODE) (inst >> 4);
        if (opcode == BLOCK_LAYOUT_STRONG)
          strong_word_count = (inst & 0xF)+1;
        else if (opcode == BLOCK_LAYOUT_BYREF)
          byref_word_count = (inst & 0xF)+1;
        else if (opcode == BLOCK_LAYOUT_WEAK)
          weak_word_count = (inst & 0xF)+1;
        else
          return 0;
        break;
        
      default:
        return 0;
    }
    
    // Cannot inline when any of the word counts is 15. Because this is one less
    // than the actual work count (so 15 means 16 actual word counts),
    // and we can only display 0 thru 15 word counts.
    if (strong_word_count == 16 || byref_word_count == 16 || weak_word_count == 16)
      return 0;
    
    unsigned count =
      (strong_word_count != 0) + (byref_word_count != 0) + (weak_word_count != 0);
    
    if (size == count) {
      if (strong_word_count)
        Result = strong_word_count;
      Result <<= 4;
      if (byref_word_count)
        Result += byref_word_count;
      Result <<= 4;
      if (weak_word_count)
        Result += weak_word_count;
    }
  }
  return Result;
}

llvm37::Constant *CGObjCCommonMac::getBitmapBlockLayout(bool ComputeByrefLayout) {
  llvm37::Constant *nullPtr = llvm37::Constant::getNullValue(CGM.Int8PtrTy);
  if (RunSkipBlockVars.empty())
    return nullPtr;
  unsigned WordSizeInBits = CGM.getTarget().getPointerWidth(0);
  unsigned ByteSizeInBits = CGM.getTarget().getCharWidth();
  unsigned WordSizeInBytes = WordSizeInBits/ByteSizeInBits;
  
  // Sort on byte position; captures might not be allocated in order,
  // and unions can do funny things.
  llvm37::array_pod_sort(RunSkipBlockVars.begin(), RunSkipBlockVars.end());
  SmallVector<unsigned char, 16> Layout;
  
  unsigned size = RunSkipBlockVars.size();
  for (unsigned i = 0; i < size; i++) {
    enum BLOCK_LAYOUT_OPCODE opcode = RunSkipBlockVars[i].opcode;
    CharUnits start_byte_pos = RunSkipBlockVars[i].block_var_bytepos;
    CharUnits end_byte_pos = start_byte_pos;
    unsigned j = i+1;
    while (j < size) {
      if (opcode == RunSkipBlockVars[j].opcode) {
        end_byte_pos = RunSkipBlockVars[j++].block_var_bytepos;
        i++;
      }
      else
        break;
    }
    CharUnits size_in_bytes =
    end_byte_pos - start_byte_pos + RunSkipBlockVars[j-1].block_var_size;
    if (j < size) {
      CharUnits gap =
      RunSkipBlockVars[j].block_var_bytepos -
      RunSkipBlockVars[j-1].block_var_bytepos - RunSkipBlockVars[j-1].block_var_size;
      size_in_bytes += gap;
    }
    CharUnits residue_in_bytes = CharUnits::Zero();
    if (opcode == BLOCK_LAYOUT_NON_OBJECT_BYTES) {
      residue_in_bytes = size_in_bytes % WordSizeInBytes;
      size_in_bytes -= residue_in_bytes;
      opcode = BLOCK_LAYOUT_NON_OBJECT_WORDS;
    }
    
    unsigned size_in_words = size_in_bytes.getQuantity() / WordSizeInBytes;
    while (size_in_words >= 16) {
      // Note that value in imm. is one less that the actual
      // value. So, 0xf means 16 words follow!
      unsigned char inst = (opcode << 4) | 0xf;
      Layout.push_back(inst);
      size_in_words -= 16;
    }
    if (size_in_words > 0) {
      // Note that value in imm. is one less that the actual
      // value. So, we subtract 1 away!
      unsigned char inst = (opcode << 4) | (size_in_words-1);
      Layout.push_back(inst);
    }
    if (residue_in_bytes > CharUnits::Zero()) {
      unsigned char inst =
      (BLOCK_LAYOUT_NON_OBJECT_BYTES << 4) | (residue_in_bytes.getQuantity()-1);
      Layout.push_back(inst);
    }
  }
  
  int e = Layout.size()-1;
  while (e >= 0) {
    unsigned char inst = Layout[e--];
    enum BLOCK_LAYOUT_OPCODE opcode = (enum BLOCK_LAYOUT_OPCODE) (inst >> 4);
    if (opcode == BLOCK_LAYOUT_NON_OBJECT_BYTES || opcode == BLOCK_LAYOUT_NON_OBJECT_WORDS)
      Layout.pop_back();
    else
      break;
  }
  
  uint64_t Result = InlineLayoutInstruction(Layout);
  if (Result != 0) {
    // Block variable layout instruction has been inlined.
    if (CGM.getLangOpts().ObjCGCBitmapPrint) {
      if (ComputeByrefLayout)
        printf("\n Inline instruction for BYREF variable layout: ");
      else
        printf("\n Inline instruction for block variable layout: ");
      printf("0x0%" PRIx64 "\n", Result);
    }
    if (WordSizeInBytes == 8) {
      const llvm37::APInt Instruction(64, Result);
      return llvm37::Constant::getIntegerValue(CGM.Int64Ty, Instruction);
    }
    else {
      const llvm37::APInt Instruction(32, Result);
      return llvm37::Constant::getIntegerValue(CGM.Int32Ty, Instruction);
    }
  }
  
  unsigned char inst = (BLOCK_LAYOUT_OPERATOR << 4) | 0;
  Layout.push_back(inst);
  std::string BitMap;
  for (unsigned i = 0, e = Layout.size(); i != e; i++)
    BitMap += Layout[i];
  
  if (CGM.getLangOpts().ObjCGCBitmapPrint) {
    if (ComputeByrefLayout)
      printf("\n BYREF variable layout: ");
    else
      printf("\n block variable layout: ");
    for (unsigned i = 0, e = BitMap.size(); i != e; i++) {
      unsigned char inst = BitMap[i];
      enum BLOCK_LAYOUT_OPCODE opcode = (enum BLOCK_LAYOUT_OPCODE) (inst >> 4);
      unsigned delta = 1;
      switch (opcode) {
        case BLOCK_LAYOUT_OPERATOR:
          printf("BL_OPERATOR:");
          delta = 0;
          break;
        case BLOCK_LAYOUT_NON_OBJECT_BYTES:
          printf("BL_NON_OBJECT_BYTES:");
          break;
        case BLOCK_LAYOUT_NON_OBJECT_WORDS:
          printf("BL_NON_OBJECT_WORD:");
          break;
        case BLOCK_LAYOUT_STRONG:
          printf("BL_STRONG:");
          break;
        case BLOCK_LAYOUT_BYREF:
          printf("BL_BYREF:");
          break;
        case BLOCK_LAYOUT_WEAK:
          printf("BL_WEAK:");
          break;
        case BLOCK_LAYOUT_UNRETAINED:
          printf("BL_UNRETAINED:");
          break;
      }
      // Actual value of word count is one more that what is in the imm.
      // field of the instruction
      printf("%d", (inst & 0xf) + delta);
      if (i < e-1)
        printf(", ");
      else
        printf("\n");
    }
  }

  llvm37::GlobalVariable *Entry = CreateMetadataVar(
      "OBJC_CLASS_NAME_",
      llvm37::ConstantDataArray::getString(VMContext, BitMap, false),
      "__TEXT,__objc_classname,cstring_literals", 1, true);
  return getConstantGEP(VMContext, Entry, 0, 0);
}

llvm37::Constant *CGObjCCommonMac::BuildRCBlockLayout(CodeGenModule &CGM,
                                                    const CGBlockInfo &blockInfo) {
  assert(CGM.getLangOpts().getGC() == LangOptions::NonGC);
  
  RunSkipBlockVars.clear();
  bool hasUnion = false;
  
  unsigned WordSizeInBits = CGM.getTarget().getPointerWidth(0);
  unsigned ByteSizeInBits = CGM.getTarget().getCharWidth();
  unsigned WordSizeInBytes = WordSizeInBits/ByteSizeInBits;
  
  const BlockDecl *blockDecl = blockInfo.getBlockDecl();
  
  // Calculate the basic layout of the block structure.
  const llvm37::StructLayout *layout =
  CGM.getDataLayout().getStructLayout(blockInfo.StructureType);
  
  // Ignore the optional 'this' capture: C++ objects are not assumed
  // to be GC'ed.
  if (blockInfo.BlockHeaderForcedGapSize != CharUnits::Zero())
    UpdateRunSkipBlockVars(false, Qualifiers::OCL_None,
                           blockInfo.BlockHeaderForcedGapOffset,
                           blockInfo.BlockHeaderForcedGapSize);
  // Walk the captured variables.
  for (const auto &CI : blockDecl->captures()) {
    const VarDecl *variable = CI.getVariable();
    QualType type = variable->getType();
    
    const CGBlockInfo::Capture &capture = blockInfo.getCapture(variable);
    
    // Ignore constant captures.
    if (capture.isConstant()) continue;
    
    CharUnits fieldOffset =
       CharUnits::fromQuantity(layout->getElementOffset(capture.getIndex()));
    
    assert(!type->isArrayType() && "array variable should not be caught");
    if (!CI.isByRef())
      if (const RecordType *record = type->getAs<RecordType>()) {
        BuildRCBlockVarRecordLayout(record, fieldOffset, hasUnion);
        continue;
      }
    CharUnits fieldSize;
    if (CI.isByRef())
      fieldSize = CharUnits::fromQuantity(WordSizeInBytes);
    else
      fieldSize = CGM.getContext().getTypeSizeInChars(type);
    UpdateRunSkipBlockVars(CI.isByRef(), getBlockCaptureLifetime(type, false),
                           fieldOffset, fieldSize);
  }
  return getBitmapBlockLayout(false);
}


llvm37::Constant *CGObjCCommonMac::BuildByrefLayout(CodeGen::CodeGenModule &CGM,
                                                  QualType T) {
  assert(CGM.getLangOpts().getGC() == LangOptions::NonGC);
  assert(!T->isArrayType() && "__block array variable should not be caught");
  CharUnits fieldOffset;
  RunSkipBlockVars.clear();
  bool hasUnion = false;
  if (const RecordType *record = T->getAs<RecordType>()) {
    BuildRCBlockVarRecordLayout(record, fieldOffset, hasUnion, true /*ByrefLayout */);
    llvm37::Constant *Result = getBitmapBlockLayout(true);
    return Result;
  }
  llvm37::Constant *nullPtr = llvm37::Constant::getNullValue(CGM.Int8PtrTy);
  return nullPtr;
}

llvm37::Value *CGObjCMac::GenerateProtocolRef(CodeGenFunction &CGF,
                                            const ObjCProtocolDecl *PD) {
  // FIXME: I don't understand why gcc generates this, or where it is
  // resolved. Investigate. Its also wasteful to look this up over and over.
  LazySymbols.insert(&CGM.getContext().Idents.get("Protocol"));

  return llvm37::ConstantExpr::getBitCast(GetProtocolRef(PD),
                                        ObjCTypes.getExternalProtocolPtrTy());
}

void CGObjCCommonMac::GenerateProtocol(const ObjCProtocolDecl *PD) {
  // FIXME: We shouldn't need this, the protocol decl should contain enough
  // information to tell us whether this was a declaration or a definition.
  DefinedProtocols.insert(PD->getIdentifier());

  // If we have generated a forward reference to this protocol, emit
  // it now. Otherwise do nothing, the protocol objects are lazily
  // emitted.
  if (Protocols.count(PD->getIdentifier()))
    GetOrEmitProtocol(PD);
}

llvm37::Constant *CGObjCCommonMac::GetProtocolRef(const ObjCProtocolDecl *PD) {
  if (DefinedProtocols.count(PD->getIdentifier()))
    return GetOrEmitProtocol(PD);
  
  return GetOrEmitProtocolRef(PD);
}

/*
// Objective-C 1.0 extensions
struct _objc_protocol {
struct _objc_protocol_extension *isa;
char *protocol_name;
struct _objc_protocol_list *protocol_list;
struct _objc__method_prototype_list *instance_methods;
struct _objc__method_prototype_list *class_methods
};

See EmitProtocolExtension().
*/
llvm37::Constant *CGObjCMac::GetOrEmitProtocol(const ObjCProtocolDecl *PD) {
  llvm37::GlobalVariable *Entry = Protocols[PD->getIdentifier()];

  // Early exit if a defining object has already been generated.
  if (Entry && Entry->hasInitializer())
    return Entry;

  // Use the protocol definition, if there is one.
  if (const ObjCProtocolDecl *Def = PD->getDefinition())
    PD = Def;

  // FIXME: I don't understand why gcc generates this, or where it is
  // resolved. Investigate. Its also wasteful to look this up over and over.
  LazySymbols.insert(&CGM.getContext().Idents.get("Protocol"));

  // Construct method lists.
  std::vector<llvm37::Constant*> InstanceMethods, ClassMethods;
  std::vector<llvm37::Constant*> OptInstanceMethods, OptClassMethods;
  std::vector<llvm37::Constant*> MethodTypesExt, OptMethodTypesExt;
  for (const auto *MD : PD->instance_methods()) {
    llvm37::Constant *C = GetMethodDescriptionConstant(MD);
    if (!C)
      return GetOrEmitProtocolRef(PD);
    
    if (MD->getImplementationControl() == ObjCMethodDecl::Optional) {
      OptInstanceMethods.push_back(C);
      OptMethodTypesExt.push_back(GetMethodVarType(MD, true));
    } else {
      InstanceMethods.push_back(C);
      MethodTypesExt.push_back(GetMethodVarType(MD, true));
    }
  }

  for (const auto *MD : PD->class_methods()) {
    llvm37::Constant *C = GetMethodDescriptionConstant(MD);
    if (!C)
      return GetOrEmitProtocolRef(PD);

    if (MD->getImplementationControl() == ObjCMethodDecl::Optional) {
      OptClassMethods.push_back(C);
      OptMethodTypesExt.push_back(GetMethodVarType(MD, true));
    } else {
      ClassMethods.push_back(C);
      MethodTypesExt.push_back(GetMethodVarType(MD, true));
    }
  }

  MethodTypesExt.insert(MethodTypesExt.end(),
                        OptMethodTypesExt.begin(), OptMethodTypesExt.end());

  llvm37::Constant *Values[] = {
      EmitProtocolExtension(PD, OptInstanceMethods, OptClassMethods,
                            MethodTypesExt),
      GetClassName(PD->getObjCRuntimeNameAsString()),
      EmitProtocolList("OBJC_PROTOCOL_REFS_" + PD->getName(),
                       PD->protocol_begin(), PD->protocol_end()),
      EmitMethodDescList("OBJC_PROTOCOL_INSTANCE_METHODS_" + PD->getName(),
                         "__OBJC,__cat_inst_meth,regular,no_dead_strip",
                         InstanceMethods),
      EmitMethodDescList("OBJC_PROTOCOL_CLASS_METHODS_" + PD->getName(),
                         "__OBJC,__cat_cls_meth,regular,no_dead_strip",
                         ClassMethods)};
  llvm37::Constant *Init = llvm37::ConstantStruct::get(ObjCTypes.ProtocolTy,
                                                   Values);

  if (Entry) {
    // Already created, update the initializer.
    assert(Entry->hasPrivateLinkage());
    Entry->setInitializer(Init);
  } else {
    Entry = new llvm37::GlobalVariable(CGM.getModule(), ObjCTypes.ProtocolTy,
                                     false, llvm37::GlobalValue::PrivateLinkage,
                                     Init, "OBJC_PROTOCOL_" + PD->getName());
    Entry->setSection("__OBJC,__protocol,regular,no_dead_strip");
    // FIXME: Is this necessary? Why only for protocol?
    Entry->setAlignment(4);

    Protocols[PD->getIdentifier()] = Entry;
  }
  CGM.addCompilerUsedGlobal(Entry);

  return Entry;
}

llvm37::Constant *CGObjCMac::GetOrEmitProtocolRef(const ObjCProtocolDecl *PD) {
  llvm37::GlobalVariable *&Entry = Protocols[PD->getIdentifier()];

  if (!Entry) {
    // We use the initializer as a marker of whether this is a forward
    // reference or not. At module finalization we add the empty
    // contents for protocols which were referenced but never defined.
    Entry = new llvm37::GlobalVariable(CGM.getModule(), ObjCTypes.ProtocolTy,
                                     false, llvm37::GlobalValue::PrivateLinkage,
                                     nullptr, "OBJC_PROTOCOL_" + PD->getName());
    Entry->setSection("__OBJC,__protocol,regular,no_dead_strip");
    // FIXME: Is this necessary? Why only for protocol?
    Entry->setAlignment(4);
  }

  return Entry;
}

/*
  struct _objc_protocol_extension {
  uint32_t size;
  struct objc_method_description_list *optional_instance_methods;
  struct objc_method_description_list *optional_class_methods;
  struct objc_property_list *instance_properties;
  const char ** extendedMethodTypes;
  };
*/
llvm37::Constant *
CGObjCMac::EmitProtocolExtension(const ObjCProtocolDecl *PD,
                                 ArrayRef<llvm37::Constant*> OptInstanceMethods,
                                 ArrayRef<llvm37::Constant*> OptClassMethods,
                                 ArrayRef<llvm37::Constant*> MethodTypesExt) {
  uint64_t Size =
    CGM.getDataLayout().getTypeAllocSize(ObjCTypes.ProtocolExtensionTy);
  llvm37::Constant *Values[] = {
      llvm37::ConstantInt::get(ObjCTypes.IntTy, Size),
      EmitMethodDescList("OBJC_PROTOCOL_INSTANCE_METHODS_OPT_" + PD->getName(),
                         "__OBJC,__cat_inst_meth,regular,no_dead_strip",
                         OptInstanceMethods),
      EmitMethodDescList("OBJC_PROTOCOL_CLASS_METHODS_OPT_" + PD->getName(),
                         "__OBJC,__cat_cls_meth,regular,no_dead_strip",
                         OptClassMethods),
      EmitPropertyList("OBJC_$_PROP_PROTO_LIST_" + PD->getName(), nullptr, PD,
                       ObjCTypes),
      EmitProtocolMethodTypes("OBJC_PROTOCOL_METHOD_TYPES_" + PD->getName(),
                              MethodTypesExt, ObjCTypes)};

  // Return null if no extension bits are used.
  if (Values[1]->isNullValue() && Values[2]->isNullValue() &&
      Values[3]->isNullValue() && Values[4]->isNullValue())
    return llvm37::Constant::getNullValue(ObjCTypes.ProtocolExtensionPtrTy);

  llvm37::Constant *Init =
    llvm37::ConstantStruct::get(ObjCTypes.ProtocolExtensionTy, Values);

  // No special section, but goes in llvm37.used
  return CreateMetadataVar("\01l_OBJC_PROTOCOLEXT_" + PD->getName(), Init,
                           StringRef(), 0, true);
}

/*
  struct objc_protocol_list {
    struct objc_protocol_list *next;
    long count;
    Protocol *list[];
  };
*/
llvm37::Constant *
CGObjCMac::EmitProtocolList(Twine Name,
                            ObjCProtocolDecl::protocol_iterator begin,
                            ObjCProtocolDecl::protocol_iterator end) {
  SmallVector<llvm37::Constant *, 16> ProtocolRefs;

  for (; begin != end; ++begin)
    ProtocolRefs.push_back(GetProtocolRef(*begin));

  // Just return null for empty protocol lists
  if (ProtocolRefs.empty())
    return llvm37::Constant::getNullValue(ObjCTypes.ProtocolListPtrTy);

  // This list is null terminated.
  ProtocolRefs.push_back(llvm37::Constant::getNullValue(ObjCTypes.ProtocolPtrTy));

  llvm37::Constant *Values[3];
  // This field is only used by the runtime.
  Values[0] = llvm37::Constant::getNullValue(ObjCTypes.ProtocolListPtrTy);
  Values[1] = llvm37::ConstantInt::get(ObjCTypes.LongTy,
                                     ProtocolRefs.size() - 1);
  Values[2] =
    llvm37::ConstantArray::get(llvm37::ArrayType::get(ObjCTypes.ProtocolPtrTy,
                                                  ProtocolRefs.size()),
                             ProtocolRefs);

  llvm37::Constant *Init = llvm37::ConstantStruct::getAnon(Values);
  llvm37::GlobalVariable *GV =
    CreateMetadataVar(Name, Init, "__OBJC,__cat_cls_meth,regular,no_dead_strip",
                      4, false);
  return llvm37::ConstantExpr::getBitCast(GV, ObjCTypes.ProtocolListPtrTy);
}

void CGObjCCommonMac::
PushProtocolProperties(llvm37::SmallPtrSet<const IdentifierInfo*,16> &PropertySet,
                       SmallVectorImpl<llvm37::Constant *> &Properties,
                       const Decl *Container,
                       const ObjCProtocolDecl *Proto,
                       const ObjCCommonTypesHelper &ObjCTypes) {
  for (const auto *P : Proto->protocols()) 
    PushProtocolProperties(PropertySet, Properties, Container, P, ObjCTypes);
  for (const auto *PD : Proto->properties()) {
    if (!PropertySet.insert(PD->getIdentifier()).second)
      continue;
    llvm37::Constant *Prop[] = {
      GetPropertyName(PD->getIdentifier()),
      GetPropertyTypeString(PD, Container)
    };
    Properties.push_back(llvm37::ConstantStruct::get(ObjCTypes.PropertyTy, Prop));
  }
}

/*
  struct _objc_property {
    const char * const name;
    const char * const attributes;
  };

  struct _objc_property_list {
    uint32_t entsize; // sizeof (struct _objc_property)
    uint32_t prop_count;
    struct _objc_property[prop_count];
  };
*/
llvm37::Constant *CGObjCCommonMac::EmitPropertyList(Twine Name,
                                       const Decl *Container,
                                       const ObjCContainerDecl *OCD,
                                       const ObjCCommonTypesHelper &ObjCTypes) {
  SmallVector<llvm37::Constant *, 16> Properties;
  llvm37::SmallPtrSet<const IdentifierInfo*, 16> PropertySet;
  for (const auto *PD : OCD->properties()) {
    PropertySet.insert(PD->getIdentifier());
    llvm37::Constant *Prop[] = {
      GetPropertyName(PD->getIdentifier()),
      GetPropertyTypeString(PD, Container)
    };
    Properties.push_back(llvm37::ConstantStruct::get(ObjCTypes.PropertyTy,
                                                   Prop));
  }
  if (const ObjCInterfaceDecl *OID = dyn_cast<ObjCInterfaceDecl>(OCD)) {
    for (const auto *P : OID->all_referenced_protocols())
      PushProtocolProperties(PropertySet, Properties, Container, P, ObjCTypes);
  }
  else if (const ObjCCategoryDecl *CD = dyn_cast<ObjCCategoryDecl>(OCD)) {
    for (const auto *P : CD->protocols())
      PushProtocolProperties(PropertySet, Properties, Container, P, ObjCTypes);
  }

  // Return null for empty list.
  if (Properties.empty())
    return llvm37::Constant::getNullValue(ObjCTypes.PropertyListPtrTy);

  unsigned PropertySize =
    CGM.getDataLayout().getTypeAllocSize(ObjCTypes.PropertyTy);
  llvm37::Constant *Values[3];
  Values[0] = llvm37::ConstantInt::get(ObjCTypes.IntTy, PropertySize);
  Values[1] = llvm37::ConstantInt::get(ObjCTypes.IntTy, Properties.size());
  llvm37::ArrayType *AT = llvm37::ArrayType::get(ObjCTypes.PropertyTy,
                                             Properties.size());
  Values[2] = llvm37::ConstantArray::get(AT, Properties);
  llvm37::Constant *Init = llvm37::ConstantStruct::getAnon(Values);

  llvm37::GlobalVariable *GV =
    CreateMetadataVar(Name, Init,
                      (ObjCABI == 2) ? "__DATA, __objc_const" :
                      "__OBJC,__property,regular,no_dead_strip",
                      (ObjCABI == 2) ? 8 : 4,
                      true);
  return llvm37::ConstantExpr::getBitCast(GV, ObjCTypes.PropertyListPtrTy);
}

llvm37::Constant *
CGObjCCommonMac::EmitProtocolMethodTypes(Twine Name,
                                         ArrayRef<llvm37::Constant*> MethodTypes,
                                         const ObjCCommonTypesHelper &ObjCTypes) {
  // Return null for empty list.
  if (MethodTypes.empty())
    return llvm37::Constant::getNullValue(ObjCTypes.Int8PtrPtrTy);

  llvm37::ArrayType *AT = llvm37::ArrayType::get(ObjCTypes.Int8PtrTy,
                                             MethodTypes.size());
  llvm37::Constant *Init = llvm37::ConstantArray::get(AT, MethodTypes);

  llvm37::GlobalVariable *GV = CreateMetadataVar(
      Name, Init, (ObjCABI == 2) ? "__DATA, __objc_const" : StringRef(),
      (ObjCABI == 2) ? 8 : 4, true);
  return llvm37::ConstantExpr::getBitCast(GV, ObjCTypes.Int8PtrPtrTy);
}

/*
  struct objc_method_description_list {
  int count;
  struct objc_method_description list[];
  };
*/
llvm37::Constant *
CGObjCMac::GetMethodDescriptionConstant(const ObjCMethodDecl *MD) {
  llvm37::Constant *Desc[] = {
    llvm37::ConstantExpr::getBitCast(GetMethodVarName(MD->getSelector()),
                                   ObjCTypes.SelectorPtrTy),
    GetMethodVarType(MD)
  };
  if (!Desc[1])
    return nullptr;

  return llvm37::ConstantStruct::get(ObjCTypes.MethodDescriptionTy,
                                   Desc);
}

llvm37::Constant *
CGObjCMac::EmitMethodDescList(Twine Name, const char *Section,
                              ArrayRef<llvm37::Constant*> Methods) {
  // Return null for empty list.
  if (Methods.empty())
    return llvm37::Constant::getNullValue(ObjCTypes.MethodDescriptionListPtrTy);

  llvm37::Constant *Values[2];
  Values[0] = llvm37::ConstantInt::get(ObjCTypes.IntTy, Methods.size());
  llvm37::ArrayType *AT = llvm37::ArrayType::get(ObjCTypes.MethodDescriptionTy,
                                             Methods.size());
  Values[1] = llvm37::ConstantArray::get(AT, Methods);
  llvm37::Constant *Init = llvm37::ConstantStruct::getAnon(Values);

  llvm37::GlobalVariable *GV = CreateMetadataVar(Name, Init, Section, 4, true);
  return llvm37::ConstantExpr::getBitCast(GV,
                                        ObjCTypes.MethodDescriptionListPtrTy);
}

/*
  struct _objc_category {
  char *category_name;
  char *class_name;
  struct _objc_method_list *instance_methods;
  struct _objc_method_list *class_methods;
  struct _objc_protocol_list *protocols;
  uint32_t size; // <rdar://4585769>
  struct _objc_property_list *instance_properties;
  };
*/
void CGObjCMac::GenerateCategory(const ObjCCategoryImplDecl *OCD) {
  unsigned Size = CGM.getDataLayout().getTypeAllocSize(ObjCTypes.CategoryTy);

  // FIXME: This is poor design, the OCD should have a pointer to the category
  // decl. Additionally, note that Category can be null for the @implementation
  // w/o an @interface case. Sema should just create one for us as it does for
  // @implementation so everyone else can live life under a clear blue sky.
  const ObjCInterfaceDecl *Interface = OCD->getClassInterface();
  const ObjCCategoryDecl *Category =
    Interface->FindCategoryDeclaration(OCD->getIdentifier());

  SmallString<256> ExtName;
  llvm37::raw_svector_ostream(ExtName) << Interface->getName() << '_'
                                     << OCD->getName();

  SmallVector<llvm37::Constant *, 16> InstanceMethods, ClassMethods;
  for (const auto *I : OCD->instance_methods())
    // Instance methods should always be defined.
    InstanceMethods.push_back(GetMethodConstant(I));

  for (const auto *I : OCD->class_methods())
    // Class methods should always be defined.
    ClassMethods.push_back(GetMethodConstant(I));

  llvm37::Constant *Values[7];
  Values[0] = GetClassName(OCD->getName());
  Values[1] = GetClassName(Interface->getObjCRuntimeNameAsString());
  LazySymbols.insert(Interface->getIdentifier());
  Values[2] = EmitMethodList("OBJC_CATEGORY_INSTANCE_METHODS_" + ExtName.str(),
                             "__OBJC,__cat_inst_meth,regular,no_dead_strip",
                             InstanceMethods);
  Values[3] = EmitMethodList("OBJC_CATEGORY_CLASS_METHODS_" + ExtName.str(),
                             "__OBJC,__cat_cls_meth,regular,no_dead_strip",
                             ClassMethods);
  if (Category) {
    Values[4] =
        EmitProtocolList("OBJC_CATEGORY_PROTOCOLS_" + ExtName.str(),
                         Category->protocol_begin(), Category->protocol_end());
  } else {
    Values[4] = llvm37::Constant::getNullValue(ObjCTypes.ProtocolListPtrTy);
  }
  Values[5] = llvm37::ConstantInt::get(ObjCTypes.IntTy, Size);

  // If there is no category @interface then there can be no properties.
  if (Category) {
    Values[6] = EmitPropertyList("\01l_OBJC_$_PROP_LIST_" + ExtName.str(),
                                 OCD, Category, ObjCTypes);
  } else {
    Values[6] = llvm37::Constant::getNullValue(ObjCTypes.PropertyListPtrTy);
  }

  llvm37::Constant *Init = llvm37::ConstantStruct::get(ObjCTypes.CategoryTy,
                                                   Values);

  llvm37::GlobalVariable *GV =
      CreateMetadataVar("OBJC_CATEGORY_" + ExtName.str(), Init,
                        "__OBJC,__category,regular,no_dead_strip", 4, true);
  DefinedCategories.push_back(GV);
  DefinedCategoryNames.insert(ExtName.str());
  // method definition entries must be clear for next implementation.
  MethodDefinitions.clear();
}

enum FragileClassFlags {
  FragileABI_Class_Factory                 = 0x00001,
  FragileABI_Class_Meta                    = 0x00002,
  FragileABI_Class_HasCXXStructors         = 0x02000,
  FragileABI_Class_Hidden                  = 0x20000
};

enum NonFragileClassFlags {
  /// Is a meta-class.
  NonFragileABI_Class_Meta                 = 0x00001,

  /// Is a root class.
  NonFragileABI_Class_Root                 = 0x00002,

  /// Has a C++ constructor and destructor.
  NonFragileABI_Class_HasCXXStructors      = 0x00004,

  /// Has hidden visibility.
  NonFragileABI_Class_Hidden               = 0x00010,

  /// Has the exception attribute.
  NonFragileABI_Class_Exception            = 0x00020,

  /// (Obsolete) ARC-specific: this class has a .release_ivars method
  NonFragileABI_Class_HasIvarReleaser      = 0x00040,

  /// Class implementation was compiled under ARC.
  NonFragileABI_Class_CompiledByARC        = 0x00080,

  /// Class has non-trivial destructors, but zero-initialization is okay.
  NonFragileABI_Class_HasCXXDestructorOnly = 0x00100
};

/*
  struct _objc_class {
  Class isa;
  Class super_class;
  const char *name;
  long version;
  long info;
  long instance_size;
  struct _objc_ivar_list *ivars;
  struct _objc_method_list *methods;
  struct _objc_cache *cache;
  struct _objc_protocol_list *protocols;
  // Objective-C 1.0 extensions (<rdr://4585769>)
  const char *ivar_layout;
  struct _objc_class_ext *ext;
  };

  See EmitClassExtension();
*/
void CGObjCMac::GenerateClass(const ObjCImplementationDecl *ID) {
  DefinedSymbols.insert(ID->getIdentifier());

  std::string ClassName = ID->getNameAsString();
  // FIXME: Gross
  ObjCInterfaceDecl *Interface =
    const_cast<ObjCInterfaceDecl*>(ID->getClassInterface());
  llvm37::Constant *Protocols =
      EmitProtocolList("OBJC_CLASS_PROTOCOLS_" + ID->getName(),
                       Interface->all_referenced_protocol_begin(),
                       Interface->all_referenced_protocol_end());
  unsigned Flags = FragileABI_Class_Factory;
  if (ID->hasNonZeroConstructors() || ID->hasDestructors())
    Flags |= FragileABI_Class_HasCXXStructors;
  unsigned Size =
    CGM.getContext().getASTObjCImplementationLayout(ID).getSize().getQuantity();

  // FIXME: Set CXX-structors flag.
  if (ID->getClassInterface()->getVisibility() == HiddenVisibility)
    Flags |= FragileABI_Class_Hidden;

  SmallVector<llvm37::Constant *, 16> InstanceMethods, ClassMethods;
  for (const auto *I : ID->instance_methods())
    // Instance methods should always be defined.
    InstanceMethods.push_back(GetMethodConstant(I));

  for (const auto *I : ID->class_methods())
    // Class methods should always be defined.
    ClassMethods.push_back(GetMethodConstant(I));

  for (const auto *PID : ID->property_impls()) {
    if (PID->getPropertyImplementation() == ObjCPropertyImplDecl::Synthesize) {
      ObjCPropertyDecl *PD = PID->getPropertyDecl();

      if (ObjCMethodDecl *MD = PD->getGetterMethodDecl())
        if (llvm37::Constant *C = GetMethodConstant(MD))
          InstanceMethods.push_back(C);
      if (ObjCMethodDecl *MD = PD->getSetterMethodDecl())
        if (llvm37::Constant *C = GetMethodConstant(MD))
          InstanceMethods.push_back(C);
    }
  }

  llvm37::Constant *Values[12];
  Values[ 0] = EmitMetaClass(ID, Protocols, ClassMethods);
  if (ObjCInterfaceDecl *Super = Interface->getSuperClass()) {
    // Record a reference to the super class.
    LazySymbols.insert(Super->getIdentifier());

    Values[ 1] =
      llvm37::ConstantExpr::getBitCast(GetClassName(Super->getObjCRuntimeNameAsString()),
                                     ObjCTypes.ClassPtrTy);
  } else {
    Values[ 1] = llvm37::Constant::getNullValue(ObjCTypes.ClassPtrTy);
  }
  Values[ 2] = GetClassName(ID->getObjCRuntimeNameAsString());
  // Version is always 0.
  Values[ 3] = llvm37::ConstantInt::get(ObjCTypes.LongTy, 0);
  Values[ 4] = llvm37::ConstantInt::get(ObjCTypes.LongTy, Flags);
  Values[ 5] = llvm37::ConstantInt::get(ObjCTypes.LongTy, Size);
  Values[ 6] = EmitIvarList(ID, false);
  Values[7] = EmitMethodList("OBJC_INSTANCE_METHODS_" + ID->getName(),
                             "__OBJC,__inst_meth,regular,no_dead_strip",
                             InstanceMethods);
  // cache is always NULL.
  Values[ 8] = llvm37::Constant::getNullValue(ObjCTypes.CachePtrTy);
  Values[ 9] = Protocols;
  Values[10] = BuildIvarLayout(ID, true);
  Values[11] = EmitClassExtension(ID);
  llvm37::Constant *Init = llvm37::ConstantStruct::get(ObjCTypes.ClassTy,
                                                   Values);
  std::string Name("OBJC_CLASS_");
  Name += ClassName;
  const char *Section = "__OBJC,__class,regular,no_dead_strip";
  // Check for a forward reference.
  llvm37::GlobalVariable *GV = CGM.getModule().getGlobalVariable(Name, true);
  if (GV) {
    assert(GV->getType()->getElementType() == ObjCTypes.ClassTy &&
           "Forward metaclass reference has incorrect type.");
    GV->setInitializer(Init);
    GV->setSection(Section);
    GV->setAlignment(4);
    CGM.addCompilerUsedGlobal(GV);
  } else
    GV = CreateMetadataVar(Name, Init, Section, 4, true);
  DefinedClasses.push_back(GV);
  ImplementedClasses.push_back(Interface);
  // method definition entries must be clear for next implementation.
  MethodDefinitions.clear();
}

llvm37::Constant *CGObjCMac::EmitMetaClass(const ObjCImplementationDecl *ID,
                                         llvm37::Constant *Protocols,
                                         ArrayRef<llvm37::Constant*> Methods) {
  unsigned Flags = FragileABI_Class_Meta;
  unsigned Size = CGM.getDataLayout().getTypeAllocSize(ObjCTypes.ClassTy);

  if (ID->getClassInterface()->getVisibility() == HiddenVisibility)
    Flags |= FragileABI_Class_Hidden;

  llvm37::Constant *Values[12];
  // The isa for the metaclass is the root of the hierarchy.
  const ObjCInterfaceDecl *Root = ID->getClassInterface();
  while (const ObjCInterfaceDecl *Super = Root->getSuperClass())
    Root = Super;
  Values[ 0] =
    llvm37::ConstantExpr::getBitCast(GetClassName(Root->getObjCRuntimeNameAsString()),
                                   ObjCTypes.ClassPtrTy);
  // The super class for the metaclass is emitted as the name of the
  // super class. The runtime fixes this up to point to the
  // *metaclass* for the super class.
  if (ObjCInterfaceDecl *Super = ID->getClassInterface()->getSuperClass()) {
    Values[ 1] =
      llvm37::ConstantExpr::getBitCast(GetClassName(Super->getObjCRuntimeNameAsString()),
                                     ObjCTypes.ClassPtrTy);
  } else {
    Values[ 1] = llvm37::Constant::getNullValue(ObjCTypes.ClassPtrTy);
  }
  Values[ 2] = GetClassName(ID->getObjCRuntimeNameAsString());
  // Version is always 0.
  Values[ 3] = llvm37::ConstantInt::get(ObjCTypes.LongTy, 0);
  Values[ 4] = llvm37::ConstantInt::get(ObjCTypes.LongTy, Flags);
  Values[ 5] = llvm37::ConstantInt::get(ObjCTypes.LongTy, Size);
  Values[ 6] = EmitIvarList(ID, true);
  Values[7] =
      EmitMethodList("OBJC_CLASS_METHODS_" + ID->getNameAsString(),
                     "__OBJC,__cls_meth,regular,no_dead_strip", Methods);
  // cache is always NULL.
  Values[ 8] = llvm37::Constant::getNullValue(ObjCTypes.CachePtrTy);
  Values[ 9] = Protocols;
  // ivar_layout for metaclass is always NULL.
  Values[10] = llvm37::Constant::getNullValue(ObjCTypes.Int8PtrTy);
  // The class extension is always unused for metaclasses.
  Values[11] = llvm37::Constant::getNullValue(ObjCTypes.ClassExtensionPtrTy);
  llvm37::Constant *Init = llvm37::ConstantStruct::get(ObjCTypes.ClassTy,
                                                   Values);

  std::string Name("OBJC_METACLASS_");
  Name += ID->getName();

  // Check for a forward reference.
  llvm37::GlobalVariable *GV = CGM.getModule().getGlobalVariable(Name, true);
  if (GV) {
    assert(GV->getType()->getElementType() == ObjCTypes.ClassTy &&
           "Forward metaclass reference has incorrect type.");
    GV->setInitializer(Init);
  } else {
    GV = new llvm37::GlobalVariable(CGM.getModule(), ObjCTypes.ClassTy, false,
                                  llvm37::GlobalValue::PrivateLinkage,
                                  Init, Name);
  }
  GV->setSection("__OBJC,__meta_class,regular,no_dead_strip");
  GV->setAlignment(4);
  CGM.addCompilerUsedGlobal(GV);

  return GV;
}

llvm37::Constant *CGObjCMac::EmitMetaClassRef(const ObjCInterfaceDecl *ID) {
  std::string Name = "OBJC_METACLASS_" + ID->getNameAsString();

  // FIXME: Should we look these up somewhere other than the module. Its a bit
  // silly since we only generate these while processing an implementation, so
  // exactly one pointer would work if know when we entered/exitted an
  // implementation block.

  // Check for an existing forward reference.
  // Previously, metaclass with internal linkage may have been defined.
  // pass 'true' as 2nd argument so it is returned.
  llvm37::GlobalVariable *GV = CGM.getModule().getGlobalVariable(Name, true);
  if (!GV)
    GV = new llvm37::GlobalVariable(CGM.getModule(), ObjCTypes.ClassTy, false,
                                  llvm37::GlobalValue::PrivateLinkage, nullptr,
                                  Name);

  assert(GV->getType()->getElementType() == ObjCTypes.ClassTy &&
         "Forward metaclass reference has incorrect type.");
  return GV;
}

llvm37::Value *CGObjCMac::EmitSuperClassRef(const ObjCInterfaceDecl *ID) {
  std::string Name = "OBJC_CLASS_" + ID->getNameAsString();
  llvm37::GlobalVariable *GV = CGM.getModule().getGlobalVariable(Name, true);

  if (!GV)
    GV = new llvm37::GlobalVariable(CGM.getModule(), ObjCTypes.ClassTy, false,
                                  llvm37::GlobalValue::PrivateLinkage, nullptr,
                                  Name);

  assert(GV->getType()->getElementType() == ObjCTypes.ClassTy &&
         "Forward class metadata reference has incorrect type.");
  return GV;
}

/*
  struct objc_class_ext {
  uint32_t size;
  const char *weak_ivar_layout;
  struct _objc_property_list *properties;
  };
*/
llvm37::Constant *
CGObjCMac::EmitClassExtension(const ObjCImplementationDecl *ID) {
  uint64_t Size =
    CGM.getDataLayout().getTypeAllocSize(ObjCTypes.ClassExtensionTy);

  llvm37::Constant *Values[3];
  Values[0] = llvm37::ConstantInt::get(ObjCTypes.IntTy, Size);
  Values[1] = BuildIvarLayout(ID, false);
  Values[2] = EmitPropertyList("\01l_OBJC_$_PROP_LIST_" + ID->getName(),
                               ID, ID->getClassInterface(), ObjCTypes);

  // Return null if no extension bits are used.
  if (Values[1]->isNullValue() && Values[2]->isNullValue())
    return llvm37::Constant::getNullValue(ObjCTypes.ClassExtensionPtrTy);

  llvm37::Constant *Init =
    llvm37::ConstantStruct::get(ObjCTypes.ClassExtensionTy, Values);
  return CreateMetadataVar("OBJC_CLASSEXT_" + ID->getName(), Init,
                           "__OBJC,__class_ext,regular,no_dead_strip", 4, true);
}

/*
  struct objc_ivar {
    char *ivar_name;
    char *ivar_type;
    int ivar_offset;
  };

  struct objc_ivar_list {
    int ivar_count;
    struct objc_ivar list[count];
  };
*/
llvm37::Constant *CGObjCMac::EmitIvarList(const ObjCImplementationDecl *ID,
                                        bool ForClass) {
  std::vector<llvm37::Constant*> Ivars;

  // When emitting the root class GCC emits ivar entries for the
  // actual class structure. It is not clear if we need to follow this
  // behavior; for now lets try and get away with not doing it. If so,
  // the cleanest solution would be to make up an ObjCInterfaceDecl
  // for the class.
  if (ForClass)
    return llvm37::Constant::getNullValue(ObjCTypes.IvarListPtrTy);

  const ObjCInterfaceDecl *OID = ID->getClassInterface();

  for (const ObjCIvarDecl *IVD = OID->all_declared_ivar_begin(); 
       IVD; IVD = IVD->getNextIvar()) {
    // Ignore unnamed bit-fields.
    if (!IVD->getDeclName())
      continue;
    llvm37::Constant *Ivar[] = {
      GetMethodVarName(IVD->getIdentifier()),
      GetMethodVarType(IVD),
      llvm37::ConstantInt::get(ObjCTypes.IntTy,
                             ComputeIvarBaseOffset(CGM, OID, IVD))
    };
    Ivars.push_back(llvm37::ConstantStruct::get(ObjCTypes.IvarTy, Ivar));
  }

  // Return null for empty list.
  if (Ivars.empty())
    return llvm37::Constant::getNullValue(ObjCTypes.IvarListPtrTy);

  llvm37::Constant *Values[2];
  Values[0] = llvm37::ConstantInt::get(ObjCTypes.IntTy, Ivars.size());
  llvm37::ArrayType *AT = llvm37::ArrayType::get(ObjCTypes.IvarTy,
                                             Ivars.size());
  Values[1] = llvm37::ConstantArray::get(AT, Ivars);
  llvm37::Constant *Init = llvm37::ConstantStruct::getAnon(Values);

  llvm37::GlobalVariable *GV;
  if (ForClass)
    GV =
        CreateMetadataVar("OBJC_CLASS_VARIABLES_" + ID->getName(), Init,
                          "__OBJC,__class_vars,regular,no_dead_strip", 4, true);
  else
    GV = CreateMetadataVar("OBJC_INSTANCE_VARIABLES_" + ID->getName(), Init,
                           "__OBJC,__instance_vars,regular,no_dead_strip", 4,
                           true);
  return llvm37::ConstantExpr::getBitCast(GV, ObjCTypes.IvarListPtrTy);
}

/*
  struct objc_method {
  SEL method_name;
  char *method_types;
  void *method;
  };

  struct objc_method_list {
  struct objc_method_list *obsolete;
  int count;
  struct objc_method methods_list[count];
  };
*/

/// GetMethodConstant - Return a struct objc_method constant for the
/// given method if it has been defined. The result is null if the
/// method has not been defined. The return value has type MethodPtrTy.
llvm37::Constant *CGObjCMac::GetMethodConstant(const ObjCMethodDecl *MD) {
  llvm37::Function *Fn = GetMethodDefinition(MD);
  if (!Fn)
    return nullptr;

  llvm37::Constant *Method[] = {
    llvm37::ConstantExpr::getBitCast(GetMethodVarName(MD->getSelector()),
                                   ObjCTypes.SelectorPtrTy),
    GetMethodVarType(MD),
    llvm37::ConstantExpr::getBitCast(Fn, ObjCTypes.Int8PtrTy)
  };
  return llvm37::ConstantStruct::get(ObjCTypes.MethodTy, Method);
}

llvm37::Constant *CGObjCMac::EmitMethodList(Twine Name,
                                          const char *Section,
                                          ArrayRef<llvm37::Constant*> Methods) {
  // Return null for empty list.
  if (Methods.empty())
    return llvm37::Constant::getNullValue(ObjCTypes.MethodListPtrTy);

  llvm37::Constant *Values[3];
  Values[0] = llvm37::Constant::getNullValue(ObjCTypes.Int8PtrTy);
  Values[1] = llvm37::ConstantInt::get(ObjCTypes.IntTy, Methods.size());
  llvm37::ArrayType *AT = llvm37::ArrayType::get(ObjCTypes.MethodTy,
                                             Methods.size());
  Values[2] = llvm37::ConstantArray::get(AT, Methods);
  llvm37::Constant *Init = llvm37::ConstantStruct::getAnon(Values);

  llvm37::GlobalVariable *GV = CreateMetadataVar(Name, Init, Section, 4, true);
  return llvm37::ConstantExpr::getBitCast(GV, ObjCTypes.MethodListPtrTy);
}

llvm37::Function *CGObjCCommonMac::GenerateMethod(const ObjCMethodDecl *OMD,
                                                const ObjCContainerDecl *CD) {
  SmallString<256> Name;
  GetNameForMethod(OMD, CD, Name);

  CodeGenTypes &Types = CGM.getTypes();
  llvm37::FunctionType *MethodTy =
    Types.GetFunctionType(Types.arrangeObjCMethodDeclaration(OMD));
  llvm37::Function *Method =
    llvm37::Function::Create(MethodTy,
                           llvm37::GlobalValue::InternalLinkage,
                           Name.str(),
                           &CGM.getModule());
  MethodDefinitions.insert(std::make_pair(OMD, Method));

  return Method;
}

llvm37::GlobalVariable *CGObjCCommonMac::CreateMetadataVar(Twine Name,
                                                         llvm37::Constant *Init,
                                                         StringRef Section,
                                                         unsigned Align,
                                                         bool AddToUsed) {
  llvm37::Type *Ty = Init->getType();
  llvm37::GlobalVariable *GV =
    new llvm37::GlobalVariable(CGM.getModule(), Ty, false,
                             llvm37::GlobalValue::PrivateLinkage, Init, Name);
  if (!Section.empty())
    GV->setSection(Section);
  if (Align)
    GV->setAlignment(Align);
  if (AddToUsed)
    CGM.addCompilerUsedGlobal(GV);
  return GV;
}

llvm37::Function *CGObjCMac::ModuleInitFunction() {
  // Abuse this interface function as a place to finalize.
  FinishModule();
  return nullptr;
}

llvm37::Constant *CGObjCMac::GetPropertyGetFunction() {
  return ObjCTypes.getGetPropertyFn();
}

llvm37::Constant *CGObjCMac::GetPropertySetFunction() {
  return ObjCTypes.getSetPropertyFn();
}

llvm37::Constant *CGObjCMac::GetOptimizedPropertySetFunction(bool atomic, 
                                                           bool copy) {
  return ObjCTypes.getOptimizedSetPropertyFn(atomic, copy);
}

llvm37::Constant *CGObjCMac::GetGetStructFunction() {
  return ObjCTypes.getCopyStructFn();
}
llvm37::Constant *CGObjCMac::GetSetStructFunction() {
  return ObjCTypes.getCopyStructFn();
}

llvm37::Constant *CGObjCMac::GetCppAtomicObjectGetFunction() {
  return ObjCTypes.getCppAtomicObjectFunction();
}
llvm37::Constant *CGObjCMac::GetCppAtomicObjectSetFunction() {
  return ObjCTypes.getCppAtomicObjectFunction();
}

llvm37::Constant *CGObjCMac::EnumerationMutationFunction() {
  return ObjCTypes.getEnumerationMutationFn();
}

void CGObjCMac::EmitTryStmt(CodeGenFunction &CGF, const ObjCAtTryStmt &S) {
  return EmitTryOrSynchronizedStmt(CGF, S);
}

void CGObjCMac::EmitSynchronizedStmt(CodeGenFunction &CGF,
                                     const ObjCAtSynchronizedStmt &S) {
  return EmitTryOrSynchronizedStmt(CGF, S);
}

namespace {
  struct PerformFragileFinally : EHScopeStack::Cleanup {
    const Stmt &S;
    llvm37::Value *SyncArgSlot;
    llvm37::Value *CallTryExitVar;
    llvm37::Value *ExceptionData;
    ObjCTypesHelper &ObjCTypes;
    PerformFragileFinally(const Stmt *S,
                          llvm37::Value *SyncArgSlot,
                          llvm37::Value *CallTryExitVar,
                          llvm37::Value *ExceptionData,
                          ObjCTypesHelper *ObjCTypes)
      : S(*S), SyncArgSlot(SyncArgSlot), CallTryExitVar(CallTryExitVar),
        ExceptionData(ExceptionData), ObjCTypes(*ObjCTypes) {}

    void Emit(CodeGenFunction &CGF, Flags flags) override {
      // Check whether we need to call objc_exception_try_exit.
      // In optimized code, this branch will always be folded.
      llvm37::BasicBlock *FinallyCallExit =
        CGF.createBasicBlock("finally.call_exit");
      llvm37::BasicBlock *FinallyNoCallExit =
        CGF.createBasicBlock("finally.no_call_exit");
      CGF.Builder.CreateCondBr(CGF.Builder.CreateLoad(CallTryExitVar),
                               FinallyCallExit, FinallyNoCallExit);

      CGF.EmitBlock(FinallyCallExit);
      CGF.EmitNounwindRuntimeCall(ObjCTypes.getExceptionTryExitFn(),
                                  ExceptionData);

      CGF.EmitBlock(FinallyNoCallExit);

      if (isa<ObjCAtTryStmt>(S)) {
        if (const ObjCAtFinallyStmt* FinallyStmt =
              cast<ObjCAtTryStmt>(S).getFinallyStmt()) {
          // Don't try to do the @finally if this is an EH cleanup.
          if (flags.isForEHCleanup()) return;

          // Save the current cleanup destination in case there's
          // control flow inside the finally statement.
          llvm37::Value *CurCleanupDest =
            CGF.Builder.CreateLoad(CGF.getNormalCleanupDestSlot());

          CGF.EmitStmt(FinallyStmt->getFinallyBody());

          if (CGF.HaveInsertPoint()) {
            CGF.Builder.CreateStore(CurCleanupDest,
                                    CGF.getNormalCleanupDestSlot());
          } else {
            // Currently, the end of the cleanup must always exist.
            CGF.EnsureInsertPoint();
          }
        }
      } else {
        // Emit objc_sync_exit(expr); as finally's sole statement for
        // @synchronized.
        llvm37::Value *SyncArg = CGF.Builder.CreateLoad(SyncArgSlot);
        CGF.EmitNounwindRuntimeCall(ObjCTypes.getSyncExitFn(), SyncArg);
      }
    }
  };

  class FragileHazards {
    CodeGenFunction &CGF;
    SmallVector<llvm37::Value*, 20> Locals;
    llvm37::DenseSet<llvm37::BasicBlock*> BlocksBeforeTry;

    llvm37::InlineAsm *ReadHazard;
    llvm37::InlineAsm *WriteHazard;

    llvm37::FunctionType *GetAsmFnType();

    void collectLocals();
    void emitReadHazard(CGBuilderTy &Builder);

  public:
    FragileHazards(CodeGenFunction &CGF);

    void emitWriteHazard();
    void emitHazardsInNewBlocks();
  };
}

/// Create the fragile-ABI read and write hazards based on the current
/// state of the function, which is presumed to be immediately prior
/// to a @try block.  These hazards are used to maintain correct
/// semantics in the face of optimization and the fragile ABI's
/// cavalier use of setjmp/longjmp.
FragileHazards::FragileHazards(CodeGenFunction &CGF) : CGF(CGF) {
  collectLocals();

  if (Locals.empty()) return;

  // Collect all the blocks in the function.
  for (llvm37::Function::iterator
         I = CGF.CurFn->begin(), E = CGF.CurFn->end(); I != E; ++I)
    BlocksBeforeTry.insert(&*I);

  llvm37::FunctionType *AsmFnTy = GetAsmFnType();

  // Create a read hazard for the allocas.  This inhibits dead-store
  // optimizations and forces the values to memory.  This hazard is
  // inserted before any 'throwing' calls in the protected scope to
  // reflect the possibility that the variables might be read from the
  // catch block if the call throws.
  {
    std::string Constraint;
    for (unsigned I = 0, E = Locals.size(); I != E; ++I) {
      if (I) Constraint += ',';
      Constraint += "*m";
    }

    ReadHazard = llvm37::InlineAsm::get(AsmFnTy, "", Constraint, true, false);
  }

  // Create a write hazard for the allocas.  This inhibits folding
  // loads across the hazard.  This hazard is inserted at the
  // beginning of the catch path to reflect the possibility that the
  // variables might have been written within the protected scope.
  {
    std::string Constraint;
    for (unsigned I = 0, E = Locals.size(); I != E; ++I) {
      if (I) Constraint += ',';
      Constraint += "=*m";
    }

    WriteHazard = llvm37::InlineAsm::get(AsmFnTy, "", Constraint, true, false);
  }
}

/// Emit a write hazard at the current location.
void FragileHazards::emitWriteHazard() {
  if (Locals.empty()) return;

  CGF.EmitNounwindRuntimeCall(WriteHazard, Locals);
}

void FragileHazards::emitReadHazard(CGBuilderTy &Builder) {
  assert(!Locals.empty());
  llvm37::CallInst *call = Builder.CreateCall(ReadHazard, Locals);
  call->setDoesNotThrow();
  call->setCallingConv(CGF.getRuntimeCC());
}

/// Emit read hazards in all the protected blocks, i.e. all the blocks
/// which have been inserted since the beginning of the try.
void FragileHazards::emitHazardsInNewBlocks() {
  if (Locals.empty()) return;

  CGBuilderTy Builder(CGF.getLLVM37Context());

  // Iterate through all blocks, skipping those prior to the try.
  for (llvm37::Function::iterator
         FI = CGF.CurFn->begin(), FE = CGF.CurFn->end(); FI != FE; ++FI) {
    llvm37::BasicBlock &BB = *FI;
    if (BlocksBeforeTry.count(&BB)) continue;

    // Walk through all the calls in the block.
    for (llvm37::BasicBlock::iterator
           BI = BB.begin(), BE = BB.end(); BI != BE; ++BI) {
      llvm37::Instruction &I = *BI;

      // Ignore instructions that aren't non-intrinsic calls.
      // These are the only calls that can possibly call longjmp.
      if (!isa<llvm37::CallInst>(I) && !isa<llvm37::InvokeInst>(I)) continue;
      if (isa<llvm37::IntrinsicInst>(I))
        continue;

      // Ignore call sites marked nounwind.  This may be questionable,
      // since 'nounwind' doesn't necessarily mean 'does not call longjmp'.
      llvm37::CallSite CS(&I);
      if (CS.doesNotThrow()) continue;

      // Insert a read hazard before the call.  This will ensure that
      // any writes to the locals are performed before making the
      // call.  If the call throws, then this is sufficient to
      // guarantee correctness as long as it doesn't also write to any
      // locals.
      Builder.SetInsertPoint(&BB, BI);
      emitReadHazard(Builder);
    }
  }
}

static void addIfPresent(llvm37::DenseSet<llvm37::Value*> &S, llvm37::Value *V) {
  if (V) S.insert(V);
}

void FragileHazards::collectLocals() {
  // Compute a set of allocas to ignore.
  llvm37::DenseSet<llvm37::Value*> AllocasToIgnore;
  addIfPresent(AllocasToIgnore, CGF.ReturnValue);
  addIfPresent(AllocasToIgnore, CGF.NormalCleanupDest);

  // Collect all the allocas currently in the function.  This is
  // probably way too aggressive.
  llvm37::BasicBlock &Entry = CGF.CurFn->getEntryBlock();
  for (llvm37::BasicBlock::iterator
         I = Entry.begin(), E = Entry.end(); I != E; ++I)
    if (isa<llvm37::AllocaInst>(*I) && !AllocasToIgnore.count(&*I))
      Locals.push_back(&*I);
}

llvm37::FunctionType *FragileHazards::GetAsmFnType() {
  SmallVector<llvm37::Type *, 16> tys(Locals.size());
  for (unsigned i = 0, e = Locals.size(); i != e; ++i)
    tys[i] = Locals[i]->getType();
  return llvm37::FunctionType::get(CGF.VoidTy, tys, false);
}

/*

  Objective-C setjmp-longjmp (sjlj) Exception Handling
  --

  A catch buffer is a setjmp buffer plus:
    - a pointer to the exception that was caught
    - a pointer to the previous exception data buffer
    - two pointers of reserved storage
  Therefore catch buffers form a stack, with a pointer to the top
  of the stack kept in thread-local storage.

  objc_exception_try_enter pushes a catch buffer onto the EH stack.
  objc_exception_try_exit pops the given catch buffer, which is
    required to be the top of the EH stack.
  objc_exception_throw pops the top of the EH stack, writes the
    thrown exception into the appropriate field, and longjmps
    to the setjmp buffer.  It crashes the process (with a printf
    and an abort()) if there are no catch buffers on the stack.
  objc_exception_extract just reads the exception pointer out of the
    catch buffer.

  There's no reason an implementation couldn't use a light-weight
  setjmp here --- something like __builtin_setjmp, but API-compatible
  with the heavyweight setjmp.  This will be more important if we ever
  want to implement correct ObjC/C++ exception interactions for the
  fragile ABI.

  Note that for this use of setjmp/longjmp to be correct, we may need
  to mark some local variables volatile: if a non-volatile local
  variable is modified between the setjmp and the longjmp, it has
  indeterminate value.  For the purposes of LLVM37 IR, it may be
  sufficient to make loads and stores within the @try (to variables
  declared outside the @try) volatile.  This is necessary for
  optimized correctness, but is not currently being done; this is
  being tracked as rdar://problem/8160285

  The basic framework for a @try-catch-finally is as follows:
  {
  objc_exception_data d;
  id _rethrow = null;
  bool _call_try_exit = true;

  objc_exception_try_enter(&d);
  if (!setjmp(d.jmp_buf)) {
  ... try body ...
  } else {
  // exception path
  id _caught = objc_exception_extract(&d);

  // enter new try scope for handlers
  if (!setjmp(d.jmp_buf)) {
  ... match exception and execute catch blocks ...

  // fell off end, rethrow.
  _rethrow = _caught;
  ... jump-through-finally to finally_rethrow ...
  } else {
  // exception in catch block
  _rethrow = objc_exception_extract(&d);
  _call_try_exit = false;
  ... jump-through-finally to finally_rethrow ...
  }
  }
  ... jump-through-finally to finally_end ...

  finally:
  if (_call_try_exit)
  objc_exception_try_exit(&d);

  ... finally block ....
  ... dispatch to finally destination ...

  finally_rethrow:
  objc_exception_throw(_rethrow);

  finally_end:
  }

  This framework differs slightly from the one gcc uses, in that gcc
  uses _rethrow to determine if objc_exception_try_exit should be called
  and if the object should be rethrown. This breaks in the face of
  throwing nil and introduces unnecessary branches.

  We specialize this framework for a few particular circumstances:

  - If there are no catch blocks, then we avoid emitting the second
  exception handling context.

  - If there is a catch-all catch block (i.e. @catch(...) or @catch(id
  e)) we avoid emitting the code to rethrow an uncaught exception.

  - FIXME: If there is no @finally block we can do a few more
  simplifications.

  Rethrows and Jumps-Through-Finally
  --

  '@throw;' is supported by pushing the currently-caught exception
  onto ObjCEHStack while the @catch blocks are emitted.

  Branches through the @finally block are handled with an ordinary
  normal cleanup.  We do not register an EH cleanup; fragile-ABI ObjC
  exceptions are not compatible with C++ exceptions, and this is
  hardly the only place where this will go wrong.

  @synchronized(expr) { stmt; } is emitted as if it were:
    id synch_value = expr;
    objc_sync_enter(synch_value);
    @try { stmt; } @finally { objc_sync_exit(synch_value); }
*/

void CGObjCMac::EmitTryOrSynchronizedStmt(CodeGen::CodeGenFunction &CGF,
                                          const Stmt &S) {
  bool isTry = isa<ObjCAtTryStmt>(S);

  // A destination for the fall-through edges of the catch handlers to
  // jump to.
  CodeGenFunction::JumpDest FinallyEnd =
    CGF.getJumpDestInCurrentScope("finally.end");

  // A destination for the rethrow edge of the catch handlers to jump
  // to.
  CodeGenFunction::JumpDest FinallyRethrow =
    CGF.getJumpDestInCurrentScope("finally.rethrow");

  // For @synchronized, call objc_sync_enter(sync.expr). The
  // evaluation of the expression must occur before we enter the
  // @synchronized.  We can't avoid a temp here because we need the
  // value to be preserved.  If the backend ever does liveness
  // correctly after setjmp, this will be unnecessary.
  llvm37::Value *SyncArgSlot = nullptr;
  if (!isTry) {
    llvm37::Value *SyncArg =
      CGF.EmitScalarExpr(cast<ObjCAtSynchronizedStmt>(S).getSynchExpr());
    SyncArg = CGF.Builder.CreateBitCast(SyncArg, ObjCTypes.ObjectPtrTy);
    CGF.EmitNounwindRuntimeCall(ObjCTypes.getSyncEnterFn(), SyncArg);

    SyncArgSlot = CGF.CreateTempAlloca(SyncArg->getType(), "sync.arg");
    CGF.Builder.CreateStore(SyncArg, SyncArgSlot);
  }

  // Allocate memory for the setjmp buffer.  This needs to be kept
  // live throughout the try and catch blocks.
  llvm37::Value *ExceptionData = CGF.CreateTempAlloca(ObjCTypes.ExceptionDataTy,
                                                    "exceptiondata.ptr");

  // Create the fragile hazards.  Note that this will not capture any
  // of the allocas required for exception processing, but will
  // capture the current basic block (which extends all the way to the
  // setjmp call) as "before the @try".
  FragileHazards Hazards(CGF);

  // Create a flag indicating whether the cleanup needs to call
  // objc_exception_try_exit.  This is true except when
  //   - no catches match and we're branching through the cleanup
  //     just to rethrow the exception, or
  //   - a catch matched and we're falling out of the catch handler.
  // The setjmp-safety rule here is that we should always store to this
  // variable in a place that dominates the branch through the cleanup
  // without passing through any setjmps.
  llvm37::Value *CallTryExitVar = CGF.CreateTempAlloca(CGF.Builder.getInt1Ty(),
                                                     "_call_try_exit");

  // A slot containing the exception to rethrow.  Only needed when we
  // have both a @catch and a @finally.
  llvm37::Value *PropagatingExnVar = nullptr;

  // Push a normal cleanup to leave the try scope.
  CGF.EHStack.pushCleanup<PerformFragileFinally>(NormalAndEHCleanup, &S,
                                                 SyncArgSlot,
                                                 CallTryExitVar,
                                                 ExceptionData,
                                                 &ObjCTypes);

  // Enter a try block:
  //  - Call objc_exception_try_enter to push ExceptionData on top of
  //    the EH stack.
  CGF.EmitNounwindRuntimeCall(ObjCTypes.getExceptionTryEnterFn(),
                              ExceptionData);

  //  - Call setjmp on the exception data buffer.
  llvm37::Constant *Zero = llvm37::ConstantInt::get(CGF.Builder.getInt32Ty(), 0);
  llvm37::Value *GEPIndexes[] = { Zero, Zero, Zero };
  llvm37::Value *SetJmpBuffer = CGF.Builder.CreateGEP(
      ObjCTypes.ExceptionDataTy, ExceptionData, GEPIndexes, "setjmp_buffer");
  llvm37::CallInst *SetJmpResult = CGF.EmitNounwindRuntimeCall(
      ObjCTypes.getSetJmpFn(), SetJmpBuffer, "setjmp_result");
  SetJmpResult->setCanReturnTwice();

  // If setjmp returned 0, enter the protected block; otherwise,
  // branch to the handler.
  llvm37::BasicBlock *TryBlock = CGF.createBasicBlock("try");
  llvm37::BasicBlock *TryHandler = CGF.createBasicBlock("try.handler");
  llvm37::Value *DidCatch =
    CGF.Builder.CreateIsNotNull(SetJmpResult, "did_catch_exception");
  CGF.Builder.CreateCondBr(DidCatch, TryHandler, TryBlock);

  // Emit the protected block.
  CGF.EmitBlock(TryBlock);
  CGF.Builder.CreateStore(CGF.Builder.getTrue(), CallTryExitVar);
  CGF.EmitStmt(isTry ? cast<ObjCAtTryStmt>(S).getTryBody()
                     : cast<ObjCAtSynchronizedStmt>(S).getSynchBody());

  CGBuilderTy::InsertPoint TryFallthroughIP = CGF.Builder.saveAndClearIP();

  // Emit the exception handler block.
  CGF.EmitBlock(TryHandler);

  // Don't optimize loads of the in-scope locals across this point.
  Hazards.emitWriteHazard();

  // For a @synchronized (or a @try with no catches), just branch
  // through the cleanup to the rethrow block.
  if (!isTry || !cast<ObjCAtTryStmt>(S).getNumCatchStmts()) {
    // Tell the cleanup not to re-pop the exit.
    CGF.Builder.CreateStore(CGF.Builder.getFalse(), CallTryExitVar);
    CGF.EmitBranchThroughCleanup(FinallyRethrow);

  // Otherwise, we have to match against the caught exceptions.
  } else {
    // Retrieve the exception object.  We may emit multiple blocks but
    // nothing can cross this so the value is already in SSA form.
    llvm37::CallInst *Caught =
      CGF.EmitNounwindRuntimeCall(ObjCTypes.getExceptionExtractFn(),
                                  ExceptionData, "caught");

    // Push the exception to rethrow onto the EH value stack for the
    // benefit of any @throws in the handlers.
    CGF.ObjCEHValueStack.push_back(Caught);

    const ObjCAtTryStmt* AtTryStmt = cast<ObjCAtTryStmt>(&S);

    bool HasFinally = (AtTryStmt->getFinallyStmt() != nullptr);

    llvm37::BasicBlock *CatchBlock = nullptr;
    llvm37::BasicBlock *CatchHandler = nullptr;
    if (HasFinally) {
      // Save the currently-propagating exception before
      // objc_exception_try_enter clears the exception slot.
      PropagatingExnVar = CGF.CreateTempAlloca(Caught->getType(),
                                               "propagating_exception");
      CGF.Builder.CreateStore(Caught, PropagatingExnVar);

      // Enter a new exception try block (in case a @catch block
      // throws an exception).
      CGF.EmitNounwindRuntimeCall(ObjCTypes.getExceptionTryEnterFn(),
                                  ExceptionData);

      llvm37::CallInst *SetJmpResult =
        CGF.EmitNounwindRuntimeCall(ObjCTypes.getSetJmpFn(),
                                    SetJmpBuffer, "setjmp.result");
      SetJmpResult->setCanReturnTwice();

      llvm37::Value *Threw =
        CGF.Builder.CreateIsNotNull(SetJmpResult, "did_catch_exception");

      CatchBlock = CGF.createBasicBlock("catch");
      CatchHandler = CGF.createBasicBlock("catch_for_catch");
      CGF.Builder.CreateCondBr(Threw, CatchHandler, CatchBlock);

      CGF.EmitBlock(CatchBlock);
    }

    CGF.Builder.CreateStore(CGF.Builder.getInt1(HasFinally), CallTryExitVar);

    // Handle catch list. As a special case we check if everything is
    // matched and avoid generating code for falling off the end if
    // so.
    bool AllMatched = false;
    for (unsigned I = 0, N = AtTryStmt->getNumCatchStmts(); I != N; ++I) {
      const ObjCAtCatchStmt *CatchStmt = AtTryStmt->getCatchStmt(I);

      const VarDecl *CatchParam = CatchStmt->getCatchParamDecl();
      const ObjCObjectPointerType *OPT = nullptr;

      // catch(...) always matches.
      if (!CatchParam) {
        AllMatched = true;
      } else {
        OPT = CatchParam->getType()->getAs<ObjCObjectPointerType>();

        // catch(id e) always matches under this ABI, since only
        // ObjC exceptions end up here in the first place.
        // FIXME: For the time being we also match id<X>; this should
        // be rejected by Sema instead.
        if (OPT && (OPT->isObjCIdType() || OPT->isObjCQualifiedIdType()))
          AllMatched = true;
      }

      // If this is a catch-all, we don't need to test anything.
      if (AllMatched) {
        CodeGenFunction::RunCleanupsScope CatchVarCleanups(CGF);

        if (CatchParam) {
          CGF.EmitAutoVarDecl(*CatchParam);
          assert(CGF.HaveInsertPoint() && "DeclStmt destroyed insert point?");

          // These types work out because ConvertType(id) == i8*.
          CGF.Builder.CreateStore(Caught, CGF.GetAddrOfLocalVar(CatchParam));
        }

        CGF.EmitStmt(CatchStmt->getCatchBody());

        // The scope of the catch variable ends right here.
        CatchVarCleanups.ForceCleanup();

        CGF.EmitBranchThroughCleanup(FinallyEnd);
        break;
      }

      assert(OPT && "Unexpected non-object pointer type in @catch");
      const ObjCObjectType *ObjTy = OPT->getObjectType();

      // FIXME: @catch (Class c) ?
      ObjCInterfaceDecl *IDecl = ObjTy->getInterface();
      assert(IDecl && "Catch parameter must have Objective-C type!");

      // Check if the @catch block matches the exception object.
      llvm37::Value *Class = EmitClassRef(CGF, IDecl);

      llvm37::Value *matchArgs[] = { Class, Caught };
      llvm37::CallInst *Match =
        CGF.EmitNounwindRuntimeCall(ObjCTypes.getExceptionMatchFn(),
                                    matchArgs, "match");

      llvm37::BasicBlock *MatchedBlock = CGF.createBasicBlock("match");
      llvm37::BasicBlock *NextCatchBlock = CGF.createBasicBlock("catch.next");

      CGF.Builder.CreateCondBr(CGF.Builder.CreateIsNotNull(Match, "matched"),
                               MatchedBlock, NextCatchBlock);

      // Emit the @catch block.
      CGF.EmitBlock(MatchedBlock);

      // Collect any cleanups for the catch variable.  The scope lasts until
      // the end of the catch body.
      CodeGenFunction::RunCleanupsScope CatchVarCleanups(CGF);

      CGF.EmitAutoVarDecl(*CatchParam);
      assert(CGF.HaveInsertPoint() && "DeclStmt destroyed insert point?");

      // Initialize the catch variable.
      llvm37::Value *Tmp =
        CGF.Builder.CreateBitCast(Caught,
                                  CGF.ConvertType(CatchParam->getType()));
      CGF.Builder.CreateStore(Tmp, CGF.GetAddrOfLocalVar(CatchParam));

      CGF.EmitStmt(CatchStmt->getCatchBody());

      // We're done with the catch variable.
      CatchVarCleanups.ForceCleanup();

      CGF.EmitBranchThroughCleanup(FinallyEnd);

      CGF.EmitBlock(NextCatchBlock);
    }

    CGF.ObjCEHValueStack.pop_back();

    // If nothing wanted anything to do with the caught exception,
    // kill the extract call.
    if (Caught->use_empty())
      Caught->eraseFromParent();

    if (!AllMatched)
      CGF.EmitBranchThroughCleanup(FinallyRethrow);

    if (HasFinally) {
      // Emit the exception handler for the @catch blocks.
      CGF.EmitBlock(CatchHandler);

      // In theory we might now need a write hazard, but actually it's
      // unnecessary because there's no local-accessing code between
      // the try's write hazard and here.
      //Hazards.emitWriteHazard();

      // Extract the new exception and save it to the
      // propagating-exception slot.
      assert(PropagatingExnVar);
      llvm37::CallInst *NewCaught =
        CGF.EmitNounwindRuntimeCall(ObjCTypes.getExceptionExtractFn(),
                                    ExceptionData, "caught");
      CGF.Builder.CreateStore(NewCaught, PropagatingExnVar);

      // Don't pop the catch handler; the throw already did.
      CGF.Builder.CreateStore(CGF.Builder.getFalse(), CallTryExitVar);
      CGF.EmitBranchThroughCleanup(FinallyRethrow);
    }
  }

  // Insert read hazards as required in the new blocks.
  Hazards.emitHazardsInNewBlocks();

  // Pop the cleanup.
  CGF.Builder.restoreIP(TryFallthroughIP);
  if (CGF.HaveInsertPoint())
    CGF.Builder.CreateStore(CGF.Builder.getTrue(), CallTryExitVar);
  CGF.PopCleanupBlock();
  CGF.EmitBlock(FinallyEnd.getBlock(), true);

  // Emit the rethrow block.
  CGBuilderTy::InsertPoint SavedIP = CGF.Builder.saveAndClearIP();
  CGF.EmitBlock(FinallyRethrow.getBlock(), true);
  if (CGF.HaveInsertPoint()) {
    // If we have a propagating-exception variable, check it.
    llvm37::Value *PropagatingExn;
    if (PropagatingExnVar) {
      PropagatingExn = CGF.Builder.CreateLoad(PropagatingExnVar);

    // Otherwise, just look in the buffer for the exception to throw.
    } else {
      llvm37::CallInst *Caught =
        CGF.EmitNounwindRuntimeCall(ObjCTypes.getExceptionExtractFn(),
                                    ExceptionData);
      PropagatingExn = Caught;
    }

    CGF.EmitNounwindRuntimeCall(ObjCTypes.getExceptionThrowFn(),
                                PropagatingExn);
    CGF.Builder.CreateUnreachable();
  }

  CGF.Builder.restoreIP(SavedIP);
}

void CGObjCMac::EmitThrowStmt(CodeGen::CodeGenFunction &CGF,
                              const ObjCAtThrowStmt &S,
                              bool ClearInsertionPoint) {
  llvm37::Value *ExceptionAsObject;

  if (const Expr *ThrowExpr = S.getThrowExpr()) {
    llvm37::Value *Exception = CGF.EmitObjCThrowOperand(ThrowExpr);
    ExceptionAsObject =
      CGF.Builder.CreateBitCast(Exception, ObjCTypes.ObjectPtrTy);
  } else {
    assert((!CGF.ObjCEHValueStack.empty() && CGF.ObjCEHValueStack.back()) &&
           "Unexpected rethrow outside @catch block.");
    ExceptionAsObject = CGF.ObjCEHValueStack.back();
  }

  CGF.EmitRuntimeCall(ObjCTypes.getExceptionThrowFn(), ExceptionAsObject)
    ->setDoesNotReturn();
  CGF.Builder.CreateUnreachable();

  // Clear the insertion point to indicate we are in unreachable code.
  if (ClearInsertionPoint)
    CGF.Builder.ClearInsertionPoint();
}

/// EmitObjCWeakRead - Code gen for loading value of a __weak
/// object: objc_read_weak (id *src)
///
llvm37::Value * CGObjCMac::EmitObjCWeakRead(CodeGen::CodeGenFunction &CGF,
                                          llvm37::Value *AddrWeakObj) {
  llvm37::Type* DestTy =
    cast<llvm37::PointerType>(AddrWeakObj->getType())->getElementType();
  AddrWeakObj = CGF.Builder.CreateBitCast(AddrWeakObj,
                                          ObjCTypes.PtrObjectPtrTy);
  llvm37::Value *read_weak =
    CGF.EmitNounwindRuntimeCall(ObjCTypes.getGcReadWeakFn(),
                                AddrWeakObj, "weakread");
  read_weak = CGF.Builder.CreateBitCast(read_weak, DestTy);
  return read_weak;
}

/// EmitObjCWeakAssign - Code gen for assigning to a __weak object.
/// objc_assign_weak (id src, id *dst)
///
void CGObjCMac::EmitObjCWeakAssign(CodeGen::CodeGenFunction &CGF,
                                   llvm37::Value *src, llvm37::Value *dst) {
  llvm37::Type * SrcTy = src->getType();
  if (!isa<llvm37::PointerType>(SrcTy)) {
    unsigned Size = CGM.getDataLayout().getTypeAllocSize(SrcTy);
    assert(Size <= 8 && "does not support size > 8");
    src = (Size == 4) ? CGF.Builder.CreateBitCast(src, ObjCTypes.IntTy)
      : CGF.Builder.CreateBitCast(src, ObjCTypes.LongLongTy);
    src = CGF.Builder.CreateIntToPtr(src, ObjCTypes.Int8PtrTy);
  }
  src = CGF.Builder.CreateBitCast(src, ObjCTypes.ObjectPtrTy);
  dst = CGF.Builder.CreateBitCast(dst, ObjCTypes.PtrObjectPtrTy);
  llvm37::Value *args[] = { src, dst };
  CGF.EmitNounwindRuntimeCall(ObjCTypes.getGcAssignWeakFn(),
                              args, "weakassign");
  return;
}

/// EmitObjCGlobalAssign - Code gen for assigning to a __strong object.
/// objc_assign_global (id src, id *dst)
///
void CGObjCMac::EmitObjCGlobalAssign(CodeGen::CodeGenFunction &CGF,
                                     llvm37::Value *src, llvm37::Value *dst,
                                     bool threadlocal) {
  llvm37::Type * SrcTy = src->getType();
  if (!isa<llvm37::PointerType>(SrcTy)) {
    unsigned Size = CGM.getDataLayout().getTypeAllocSize(SrcTy);
    assert(Size <= 8 && "does not support size > 8");
    src = (Size == 4) ? CGF.Builder.CreateBitCast(src, ObjCTypes.IntTy)
      : CGF.Builder.CreateBitCast(src, ObjCTypes.LongLongTy);
    src = CGF.Builder.CreateIntToPtr(src, ObjCTypes.Int8PtrTy);
  }
  src = CGF.Builder.CreateBitCast(src, ObjCTypes.ObjectPtrTy);
  dst = CGF.Builder.CreateBitCast(dst, ObjCTypes.PtrObjectPtrTy);
  llvm37::Value *args[] = { src, dst };
  if (!threadlocal)
    CGF.EmitNounwindRuntimeCall(ObjCTypes.getGcAssignGlobalFn(),
                                args, "globalassign");
  else
    CGF.EmitNounwindRuntimeCall(ObjCTypes.getGcAssignThreadLocalFn(),
                                args, "threadlocalassign");
  return;
}

/// EmitObjCIvarAssign - Code gen for assigning to a __strong object.
/// objc_assign_ivar (id src, id *dst, ptrdiff_t ivaroffset)
///
void CGObjCMac::EmitObjCIvarAssign(CodeGen::CodeGenFunction &CGF,
                                   llvm37::Value *src, llvm37::Value *dst,
                                   llvm37::Value *ivarOffset) {
  assert(ivarOffset && "EmitObjCIvarAssign - ivarOffset is NULL");
  llvm37::Type * SrcTy = src->getType();
  if (!isa<llvm37::PointerType>(SrcTy)) {
    unsigned Size = CGM.getDataLayout().getTypeAllocSize(SrcTy);
    assert(Size <= 8 && "does not support size > 8");
    src = (Size == 4) ? CGF.Builder.CreateBitCast(src, ObjCTypes.IntTy)
      : CGF.Builder.CreateBitCast(src, ObjCTypes.LongLongTy);
    src = CGF.Builder.CreateIntToPtr(src, ObjCTypes.Int8PtrTy);
  }
  src = CGF.Builder.CreateBitCast(src, ObjCTypes.ObjectPtrTy);
  dst = CGF.Builder.CreateBitCast(dst, ObjCTypes.PtrObjectPtrTy);
  llvm37::Value *args[] = { src, dst, ivarOffset };
  CGF.EmitNounwindRuntimeCall(ObjCTypes.getGcAssignIvarFn(), args);
  return;
}

/// EmitObjCStrongCastAssign - Code gen for assigning to a __strong cast object.
/// objc_assign_strongCast (id src, id *dst)
///
void CGObjCMac::EmitObjCStrongCastAssign(CodeGen::CodeGenFunction &CGF,
                                         llvm37::Value *src, llvm37::Value *dst) {
  llvm37::Type * SrcTy = src->getType();
  if (!isa<llvm37::PointerType>(SrcTy)) {
    unsigned Size = CGM.getDataLayout().getTypeAllocSize(SrcTy);
    assert(Size <= 8 && "does not support size > 8");
    src = (Size == 4) ? CGF.Builder.CreateBitCast(src, ObjCTypes.IntTy)
      : CGF.Builder.CreateBitCast(src, ObjCTypes.LongLongTy);
    src = CGF.Builder.CreateIntToPtr(src, ObjCTypes.Int8PtrTy);
  }
  src = CGF.Builder.CreateBitCast(src, ObjCTypes.ObjectPtrTy);
  dst = CGF.Builder.CreateBitCast(dst, ObjCTypes.PtrObjectPtrTy);
  llvm37::Value *args[] = { src, dst };
  CGF.EmitNounwindRuntimeCall(ObjCTypes.getGcAssignStrongCastFn(),
                              args, "weakassign");
  return;
}

void CGObjCMac::EmitGCMemmoveCollectable(CodeGen::CodeGenFunction &CGF,
                                         llvm37::Value *DestPtr,
                                         llvm37::Value *SrcPtr,
                                         llvm37::Value *size) {
  SrcPtr = CGF.Builder.CreateBitCast(SrcPtr, ObjCTypes.Int8PtrTy);
  DestPtr = CGF.Builder.CreateBitCast(DestPtr, ObjCTypes.Int8PtrTy);
  llvm37::Value *args[] = { DestPtr, SrcPtr, size };
  CGF.EmitNounwindRuntimeCall(ObjCTypes.GcMemmoveCollectableFn(), args);
}

/// EmitObjCValueForIvar - Code Gen for ivar reference.
///
LValue CGObjCMac::EmitObjCValueForIvar(CodeGen::CodeGenFunction &CGF,
                                       QualType ObjectTy,
                                       llvm37::Value *BaseValue,
                                       const ObjCIvarDecl *Ivar,
                                       unsigned CVRQualifiers) {
  const ObjCInterfaceDecl *ID =
    ObjectTy->getAs<ObjCObjectType>()->getInterface();
  return EmitValueForIvarAtOffset(CGF, ID, BaseValue, Ivar, CVRQualifiers,
                                  EmitIvarOffset(CGF, ID, Ivar));
}

llvm37::Value *CGObjCMac::EmitIvarOffset(CodeGen::CodeGenFunction &CGF,
                                       const ObjCInterfaceDecl *Interface,
                                       const ObjCIvarDecl *Ivar) {
  uint64_t Offset = ComputeIvarBaseOffset(CGM, Interface, Ivar);
  return llvm37::ConstantInt::get(
    CGM.getTypes().ConvertType(CGM.getContext().LongTy),
    Offset);
}

/* *** Private Interface *** */

/// EmitImageInfo - Emit the image info marker used to encode some module
/// level information.
///
/// See: <rdr://4810609&4810587&4810587>
/// struct IMAGE_INFO {
///   unsigned version;
///   unsigned flags;
/// };
enum ImageInfoFlags {
  eImageInfo_FixAndContinue      = (1 << 0), // This flag is no longer set by clang.
  eImageInfo_GarbageCollected    = (1 << 1),
  eImageInfo_GCOnly              = (1 << 2),
  eImageInfo_OptimizedByDyld     = (1 << 3), // This flag is set by the dyld shared cache.

  // A flag indicating that the module has no instances of a @synthesize of a
  // superclass variable. <rdar://problem/6803242>
  eImageInfo_CorrectedSynthesize = (1 << 4), // This flag is no longer set by clang.
  eImageInfo_ImageIsSimulated    = (1 << 5)
};

void CGObjCCommonMac::EmitImageInfo() {
  unsigned version = 0; // Version is unused?
  const char *Section = (ObjCABI == 1) ?
    "__OBJC, __image_info,regular" :
    "__DATA, __objc_imageinfo, regular, no_dead_strip";

  // Generate module-level named metadata to convey this information to the
  // linker and code-gen.
  llvm37::Module &Mod = CGM.getModule();

  // Add the ObjC ABI version to the module flags.
  Mod.addModuleFlag(llvm37::Module::Error, "Objective-C Version", ObjCABI);
  Mod.addModuleFlag(llvm37::Module::Error, "Objective-C Image Info Version",
                    version);
  Mod.addModuleFlag(llvm37::Module::Error, "Objective-C Image Info Section",
                    llvm37::MDString::get(VMContext,Section));

  if (CGM.getLangOpts().getGC() == LangOptions::NonGC) {
    // Non-GC overrides those files which specify GC.
    Mod.addModuleFlag(llvm37::Module::Override,
                      "Objective-C Garbage Collection", (uint32_t)0);
  } else {
    // Add the ObjC garbage collection value.
    Mod.addModuleFlag(llvm37::Module::Error,
                      "Objective-C Garbage Collection",
                      eImageInfo_GarbageCollected);

    if (CGM.getLangOpts().getGC() == LangOptions::GCOnly) {
      // Add the ObjC GC Only value.
      Mod.addModuleFlag(llvm37::Module::Error, "Objective-C GC Only",
                        eImageInfo_GCOnly);

      // Require that GC be specified and set to eImageInfo_GarbageCollected.
      llvm37::Metadata *Ops[2] = {
          llvm37::MDString::get(VMContext, "Objective-C Garbage Collection"),
          llvm37::ConstantAsMetadata::get(llvm37::ConstantInt::get(
              llvm37::Type::getInt32Ty(VMContext), eImageInfo_GarbageCollected))};
      Mod.addModuleFlag(llvm37::Module::Require, "Objective-C GC Only",
                        llvm37::MDNode::get(VMContext, Ops));
    }
  }

  // Indicate whether we're compiling this to run on a simulator.
  const llvm37::Triple &Triple = CGM.getTarget().getTriple();
  if (Triple.isiOS() &&
      (Triple.getArch() == llvm37::Triple::x86 ||
       Triple.getArch() == llvm37::Triple::x86_64))
    Mod.addModuleFlag(llvm37::Module::Error, "Objective-C Is Simulated",
                      eImageInfo_ImageIsSimulated);
}

// struct objc_module {
//   unsigned long version;
//   unsigned long size;
//   const char *name;
//   Symtab symtab;
// };

// FIXME: Get from somewhere
static const int ModuleVersion = 7;

void CGObjCMac::EmitModuleInfo() {
  uint64_t Size = CGM.getDataLayout().getTypeAllocSize(ObjCTypes.ModuleTy);

  llvm37::Constant *Values[] = {
    llvm37::ConstantInt::get(ObjCTypes.LongTy, ModuleVersion),
    llvm37::ConstantInt::get(ObjCTypes.LongTy, Size),
    // This used to be the filename, now it is unused. <rdr://4327263>
    GetClassName(StringRef("")),
    EmitModuleSymbols()
  };
  CreateMetadataVar("OBJC_MODULES",
                    llvm37::ConstantStruct::get(ObjCTypes.ModuleTy, Values),
                    "__OBJC,__module_info,regular,no_dead_strip", 4, true);
}

llvm37::Constant *CGObjCMac::EmitModuleSymbols() {
  unsigned NumClasses = DefinedClasses.size();
  unsigned NumCategories = DefinedCategories.size();

  // Return null if no symbols were defined.
  if (!NumClasses && !NumCategories)
    return llvm37::Constant::getNullValue(ObjCTypes.SymtabPtrTy);

  llvm37::Constant *Values[5];
  Values[0] = llvm37::ConstantInt::get(ObjCTypes.LongTy, 0);
  Values[1] = llvm37::Constant::getNullValue(ObjCTypes.SelectorPtrTy);
  Values[2] = llvm37::ConstantInt::get(ObjCTypes.ShortTy, NumClasses);
  Values[3] = llvm37::ConstantInt::get(ObjCTypes.ShortTy, NumCategories);

  // The runtime expects exactly the list of defined classes followed
  // by the list of defined categories, in a single array.
  SmallVector<llvm37::Constant*, 8> Symbols(NumClasses + NumCategories);
  for (unsigned i=0; i<NumClasses; i++) {
    const ObjCInterfaceDecl *ID = ImplementedClasses[i];
    assert(ID);
    if (ObjCImplementationDecl *IMP = ID->getImplementation())
      // We are implementing a weak imported interface. Give it external linkage
      if (ID->isWeakImported() && !IMP->isWeakImported())
        DefinedClasses[i]->setLinkage(llvm37::GlobalVariable::ExternalLinkage);
    
    Symbols[i] = llvm37::ConstantExpr::getBitCast(DefinedClasses[i],
                                                ObjCTypes.Int8PtrTy);
  }
  for (unsigned i=0; i<NumCategories; i++)
    Symbols[NumClasses + i] =
      llvm37::ConstantExpr::getBitCast(DefinedCategories[i],
                                     ObjCTypes.Int8PtrTy);

  Values[4] =
    llvm37::ConstantArray::get(llvm37::ArrayType::get(ObjCTypes.Int8PtrTy,
                                                  Symbols.size()),
                             Symbols);

  llvm37::Constant *Init = llvm37::ConstantStruct::getAnon(Values);

  llvm37::GlobalVariable *GV = CreateMetadataVar(
      "OBJC_SYMBOLS", Init, "__OBJC,__symbols,regular,no_dead_strip", 4, true);
  return llvm37::ConstantExpr::getBitCast(GV, ObjCTypes.SymtabPtrTy);
}

llvm37::Value *CGObjCMac::EmitClassRefFromId(CodeGenFunction &CGF,
                                           IdentifierInfo *II) {
  LazySymbols.insert(II);
  
  llvm37::GlobalVariable *&Entry = ClassReferences[II];
  
  if (!Entry) {
    llvm37::Constant *Casted =
    llvm37::ConstantExpr::getBitCast(GetClassName(II->getName()),
                                   ObjCTypes.ClassPtrTy);
    Entry = CreateMetadataVar(
        "OBJC_CLASS_REFERENCES_", Casted,
        "__OBJC,__cls_refs,literal_pointers,no_dead_strip", 4, true);
  }
  
  return CGF.Builder.CreateLoad(Entry);
}

llvm37::Value *CGObjCMac::EmitClassRef(CodeGenFunction &CGF,
                                     const ObjCInterfaceDecl *ID) {
  return EmitClassRefFromId(CGF, ID->getIdentifier());
}

llvm37::Value *CGObjCMac::EmitNSAutoreleasePoolClassRef(CodeGenFunction &CGF) {
  IdentifierInfo *II = &CGM.getContext().Idents.get("NSAutoreleasePool");
  return EmitClassRefFromId(CGF, II);
}

llvm37::Value *CGObjCMac::EmitSelector(CodeGenFunction &CGF, Selector Sel,
                                     bool lvalue) {
  llvm37::GlobalVariable *&Entry = SelectorReferences[Sel];

  if (!Entry) {
    llvm37::Constant *Casted =
      llvm37::ConstantExpr::getBitCast(GetMethodVarName(Sel),
                                     ObjCTypes.SelectorPtrTy);
    Entry = CreateMetadataVar(
        "OBJC_SELECTOR_REFERENCES_", Casted,
        "__OBJC,__message_refs,literal_pointers,no_dead_strip", 4, true);
    Entry->setExternallyInitialized(true);
  }

  if (lvalue)
    return Entry;
  return CGF.Builder.CreateLoad(Entry);
}

llvm37::Constant *CGObjCCommonMac::GetClassName(StringRef RuntimeName) {
    llvm37::GlobalVariable *&Entry = ClassNames[RuntimeName];
    if (!Entry)
      Entry = CreateMetadataVar(
          "OBJC_CLASS_NAME_",
          llvm37::ConstantDataArray::getString(VMContext, RuntimeName),
          ((ObjCABI == 2) ? "__TEXT,__objc_classname,cstring_literals"
                          : "__TEXT,__cstring,cstring_literals"),
          1, true);
    return getConstantGEP(VMContext, Entry, 0, 0);
}

llvm37::Function *CGObjCCommonMac::GetMethodDefinition(const ObjCMethodDecl *MD) {
  llvm37::DenseMap<const ObjCMethodDecl*, llvm37::Function*>::iterator
      I = MethodDefinitions.find(MD);
  if (I != MethodDefinitions.end())
    return I->second;

  return nullptr;
}

/// GetIvarLayoutName - Returns a unique constant for the given
/// ivar layout bitmap.
llvm37::Constant *CGObjCCommonMac::GetIvarLayoutName(IdentifierInfo *Ident,
                                       const ObjCCommonTypesHelper &ObjCTypes) {
  return llvm37::Constant::getNullValue(ObjCTypes.Int8PtrTy);
}

void CGObjCCommonMac::BuildAggrIvarRecordLayout(const RecordType *RT,
                                                unsigned int BytePos,
                                                bool ForStrongLayout,
                                                bool &HasUnion) {
  const RecordDecl *RD = RT->getDecl();
  // FIXME - Use iterator.
  SmallVector<const FieldDecl*, 16> Fields(RD->fields());
  llvm37::Type *Ty = CGM.getTypes().ConvertType(QualType(RT, 0));
  const llvm37::StructLayout *RecLayout =
    CGM.getDataLayout().getStructLayout(cast<llvm37::StructType>(Ty));

  BuildAggrIvarLayout(nullptr, RecLayout, RD, Fields, BytePos, ForStrongLayout,
                      HasUnion);
}

void CGObjCCommonMac::BuildAggrIvarLayout(const ObjCImplementationDecl *OI,
                             const llvm37::StructLayout *Layout,
                             const RecordDecl *RD,
                             ArrayRef<const FieldDecl*> RecFields,
                             unsigned int BytePos, bool ForStrongLayout,
                             bool &HasUnion) {
  bool IsUnion = (RD && RD->isUnion());
  uint64_t MaxUnionIvarSize = 0;
  uint64_t MaxSkippedUnionIvarSize = 0;
  const FieldDecl *MaxField = nullptr;
  const FieldDecl *MaxSkippedField = nullptr;
  const FieldDecl *LastFieldBitfieldOrUnnamed = nullptr;
  uint64_t MaxFieldOffset = 0;
  uint64_t MaxSkippedFieldOffset = 0;
  uint64_t LastBitfieldOrUnnamedOffset = 0;
  uint64_t FirstFieldDelta = 0;

  if (RecFields.empty())
    return;
  unsigned WordSizeInBits = CGM.getTarget().getPointerWidth(0);
  unsigned ByteSizeInBits = CGM.getTarget().getCharWidth();
  if (!RD && CGM.getLangOpts().ObjCAutoRefCount) {
    const FieldDecl *FirstField = RecFields[0];
    FirstFieldDelta = 
      ComputeIvarBaseOffset(CGM, OI, cast<ObjCIvarDecl>(FirstField));
  }
  
  for (unsigned i = 0, e = RecFields.size(); i != e; ++i) {
    const FieldDecl *Field = RecFields[i];
    uint64_t FieldOffset;
    if (RD) {
      // Note that 'i' here is actually the field index inside RD of Field,
      // although this dependency is hidden.
      const ASTRecordLayout &RL = CGM.getContext().getASTRecordLayout(RD);
      FieldOffset = (RL.getFieldOffset(i) / ByteSizeInBits) - FirstFieldDelta;
    } else
      FieldOffset = 
        ComputeIvarBaseOffset(CGM, OI, cast<ObjCIvarDecl>(Field)) - FirstFieldDelta;

    // Skip over unnamed or bitfields
    if (!Field->getIdentifier() || Field->isBitField()) {
      LastFieldBitfieldOrUnnamed = Field;
      LastBitfieldOrUnnamedOffset = FieldOffset;
      continue;
    }

    LastFieldBitfieldOrUnnamed = nullptr;
    QualType FQT = Field->getType();
    if (FQT->isRecordType() || FQT->isUnionType()) {
      if (FQT->isUnionType())
        HasUnion = true;

      BuildAggrIvarRecordLayout(FQT->getAs<RecordType>(),
                                BytePos + FieldOffset,
                                ForStrongLayout, HasUnion);
      continue;
    }

    if (const ArrayType *Array = CGM.getContext().getAsArrayType(FQT)) {
      const ConstantArrayType *CArray =
        dyn_cast_or_null<ConstantArrayType>(Array);
      uint64_t ElCount = CArray->getSize().getZExtValue();
      assert(CArray && "only array with known element size is supported");
      FQT = CArray->getElementType();
      while (const ArrayType *Array = CGM.getContext().getAsArrayType(FQT)) {
        const ConstantArrayType *CArray =
          dyn_cast_or_null<ConstantArrayType>(Array);
        ElCount *= CArray->getSize().getZExtValue();
        FQT = CArray->getElementType();
      }
      if (FQT->isRecordType() && ElCount) {
        int OldIndex = IvarsInfo.size() - 1;
        int OldSkIndex = SkipIvars.size() -1;

        const RecordType *RT = FQT->getAs<RecordType>();
        BuildAggrIvarRecordLayout(RT, BytePos + FieldOffset,
                                  ForStrongLayout, HasUnion);

        // Replicate layout information for each array element. Note that
        // one element is already done.
        uint64_t ElIx = 1;
        for (int FirstIndex = IvarsInfo.size() - 1,
               FirstSkIndex = SkipIvars.size() - 1 ;ElIx < ElCount; ElIx++) {
          uint64_t Size = CGM.getContext().getTypeSize(RT)/ByteSizeInBits;
          for (int i = OldIndex+1; i <= FirstIndex; ++i)
            IvarsInfo.push_back(GC_IVAR(IvarsInfo[i].ivar_bytepos + Size*ElIx,
                                        IvarsInfo[i].ivar_size));
          for (int i = OldSkIndex+1; i <= FirstSkIndex; ++i)
            SkipIvars.push_back(GC_IVAR(SkipIvars[i].ivar_bytepos + Size*ElIx,
                                        SkipIvars[i].ivar_size));
        }
        continue;
      }
    }
    // At this point, we are done with Record/Union and array there of.
    // For other arrays we are down to its element type.
    Qualifiers::GC GCAttr = GetGCAttrTypeForType(CGM.getContext(), FQT);

    unsigned FieldSize = CGM.getContext().getTypeSize(Field->getType());
    if ((ForStrongLayout && GCAttr == Qualifiers::Strong)
        || (!ForStrongLayout && GCAttr == Qualifiers::Weak)) {
      if (IsUnion) {
        uint64_t UnionIvarSize = FieldSize / WordSizeInBits;
        if (UnionIvarSize > MaxUnionIvarSize) {
          MaxUnionIvarSize = UnionIvarSize;
          MaxField = Field;
          MaxFieldOffset = FieldOffset;
        }
      } else {
        IvarsInfo.push_back(GC_IVAR(BytePos + FieldOffset,
                                    FieldSize / WordSizeInBits));
      }
    } else if ((ForStrongLayout &&
                (GCAttr == Qualifiers::GCNone || GCAttr == Qualifiers::Weak))
               || (!ForStrongLayout && GCAttr != Qualifiers::Weak)) {
      if (IsUnion) {
        // FIXME: Why the asymmetry? We divide by word size in bits on other
        // side.
        uint64_t UnionIvarSize = FieldSize / ByteSizeInBits;
        if (UnionIvarSize > MaxSkippedUnionIvarSize) {
          MaxSkippedUnionIvarSize = UnionIvarSize;
          MaxSkippedField = Field;
          MaxSkippedFieldOffset = FieldOffset;
        }
      } else {
        // FIXME: Why the asymmetry, we divide by byte size in bits here?
        SkipIvars.push_back(GC_IVAR(BytePos + FieldOffset,
                                    FieldSize / ByteSizeInBits));
      }
    }
  }

  if (LastFieldBitfieldOrUnnamed) {
    if (LastFieldBitfieldOrUnnamed->isBitField()) {
      // Last field was a bitfield. Must update skip info.
      uint64_t BitFieldSize
          = LastFieldBitfieldOrUnnamed->getBitWidthValue(CGM.getContext());
      GC_IVAR skivar;
      skivar.ivar_bytepos = BytePos + LastBitfieldOrUnnamedOffset;
      skivar.ivar_size = (BitFieldSize / ByteSizeInBits)
        + ((BitFieldSize % ByteSizeInBits) != 0);
      SkipIvars.push_back(skivar);
    } else {
      assert(!LastFieldBitfieldOrUnnamed->getIdentifier() &&"Expected unnamed");
      // Last field was unnamed. Must update skip info.
      unsigned FieldSize
          = CGM.getContext().getTypeSize(LastFieldBitfieldOrUnnamed->getType());
      SkipIvars.push_back(GC_IVAR(BytePos + LastBitfieldOrUnnamedOffset,
                                  FieldSize / ByteSizeInBits));
    }
  }

  if (MaxField)
    IvarsInfo.push_back(GC_IVAR(BytePos + MaxFieldOffset,
                                MaxUnionIvarSize));
  if (MaxSkippedField)
    SkipIvars.push_back(GC_IVAR(BytePos + MaxSkippedFieldOffset,
                                MaxSkippedUnionIvarSize));
}

/// BuildIvarLayoutBitmap - This routine is the horsework for doing all
/// the computations and returning the layout bitmap (for ivar or blocks) in
/// the given argument BitMap string container. Routine reads
/// two containers, IvarsInfo and SkipIvars which are assumed to be
/// filled already by the caller.
llvm37::Constant *CGObjCCommonMac::BuildIvarLayoutBitmap(std::string &BitMap) {
  unsigned int WordsToScan, WordsToSkip;
  llvm37::Type *PtrTy = CGM.Int8PtrTy;
  
  // Build the string of skip/scan nibbles
  SmallVector<SKIP_SCAN, 32> SkipScanIvars;
  unsigned int WordSize =
  CGM.getTypes().getDataLayout().getTypeAllocSize(PtrTy);
  if (IvarsInfo[0].ivar_bytepos == 0) {
    WordsToSkip = 0;
    WordsToScan = IvarsInfo[0].ivar_size;
  } else {
    WordsToSkip = IvarsInfo[0].ivar_bytepos/WordSize;
    WordsToScan = IvarsInfo[0].ivar_size;
  }
  for (unsigned int i=1, Last=IvarsInfo.size(); i != Last; i++) {
    unsigned int TailPrevGCObjC =
    IvarsInfo[i-1].ivar_bytepos + IvarsInfo[i-1].ivar_size * WordSize;
    if (IvarsInfo[i].ivar_bytepos == TailPrevGCObjC) {
      // consecutive 'scanned' object pointers.
      WordsToScan += IvarsInfo[i].ivar_size;
    } else {
      // Skip over 'gc'able object pointer which lay over each other.
      if (TailPrevGCObjC > IvarsInfo[i].ivar_bytepos)
        continue;
      // Must skip over 1 or more words. We save current skip/scan values
      //  and start a new pair.
      SKIP_SCAN SkScan;
      SkScan.skip = WordsToSkip;
      SkScan.scan = WordsToScan;
      SkipScanIvars.push_back(SkScan);
      
      // Skip the hole.
      SkScan.skip = (IvarsInfo[i].ivar_bytepos - TailPrevGCObjC) / WordSize;
      SkScan.scan = 0;
      SkipScanIvars.push_back(SkScan);
      WordsToSkip = 0;
      WordsToScan = IvarsInfo[i].ivar_size;
    }
  }
  if (WordsToScan > 0) {
    SKIP_SCAN SkScan;
    SkScan.skip = WordsToSkip;
    SkScan.scan = WordsToScan;
    SkipScanIvars.push_back(SkScan);
  }
  
  if (!SkipIvars.empty()) {
    unsigned int LastIndex = SkipIvars.size()-1;
    int LastByteSkipped =
    SkipIvars[LastIndex].ivar_bytepos + SkipIvars[LastIndex].ivar_size;
    LastIndex = IvarsInfo.size()-1;
    int LastByteScanned =
    IvarsInfo[LastIndex].ivar_bytepos +
    IvarsInfo[LastIndex].ivar_size * WordSize;
    // Compute number of bytes to skip at the tail end of the last ivar scanned.
    if (LastByteSkipped > LastByteScanned) {
      unsigned int TotalWords = (LastByteSkipped + (WordSize -1)) / WordSize;
      SKIP_SCAN SkScan;
      SkScan.skip = TotalWords - (LastByteScanned/WordSize);
      SkScan.scan = 0;
      SkipScanIvars.push_back(SkScan);
    }
  }
  // Mini optimization of nibbles such that an 0xM0 followed by 0x0N is produced
  // as 0xMN.
  int SkipScan = SkipScanIvars.size()-1;
  for (int i = 0; i <= SkipScan; i++) {
    if ((i < SkipScan) && SkipScanIvars[i].skip && SkipScanIvars[i].scan == 0
        && SkipScanIvars[i+1].skip == 0 && SkipScanIvars[i+1].scan) {
      // 0xM0 followed by 0x0N detected.
      SkipScanIvars[i].scan = SkipScanIvars[i+1].scan;
      for (int j = i+1; j < SkipScan; j++)
        SkipScanIvars[j] = SkipScanIvars[j+1];
      --SkipScan;
    }
  }
  
  // Generate the string.
  for (int i = 0; i <= SkipScan; i++) {
    unsigned char byte;
    unsigned int skip_small = SkipScanIvars[i].skip % 0xf;
    unsigned int scan_small = SkipScanIvars[i].scan % 0xf;
    unsigned int skip_big  = SkipScanIvars[i].skip / 0xf;
    unsigned int scan_big  = SkipScanIvars[i].scan / 0xf;
    
    // first skip big.
    for (unsigned int ix = 0; ix < skip_big; ix++)
      BitMap += (unsigned char)(0xf0);
    
    // next (skip small, scan)
    if (skip_small) {
      byte = skip_small << 4;
      if (scan_big > 0) {
        byte |= 0xf;
        --scan_big;
      } else if (scan_small) {
        byte |= scan_small;
        scan_small = 0;
      }
      BitMap += byte;
    }
    // next scan big
    for (unsigned int ix = 0; ix < scan_big; ix++)
      BitMap += (unsigned char)(0x0f);
    // last scan small
    if (scan_small) {
      byte = scan_small;
      BitMap += byte;
    }
  }
  // null terminate string.
  unsigned char zero = 0;
  BitMap += zero;

  llvm37::GlobalVariable *Entry = CreateMetadataVar(
      "OBJC_CLASS_NAME_",
      llvm37::ConstantDataArray::getString(VMContext, BitMap, false),
      ((ObjCABI == 2) ? "__TEXT,__objc_classname,cstring_literals"
                      : "__TEXT,__cstring,cstring_literals"),
      1, true);
  return getConstantGEP(VMContext, Entry, 0, 0);
}

/// BuildIvarLayout - Builds ivar layout bitmap for the class
/// implementation for the __strong or __weak case.
/// The layout map displays which words in ivar list must be skipped
/// and which must be scanned by GC (see below). String is built of bytes.
/// Each byte is divided up in two nibbles (4-bit each). Left nibble is count
/// of words to skip and right nibble is count of words to scan. So, each
/// nibble represents up to 15 workds to skip or scan. Skipping the rest is
/// represented by a 0x00 byte which also ends the string.
/// 1. when ForStrongLayout is true, following ivars are scanned:
/// - id, Class
/// - object *
/// - __strong anything
///
/// 2. When ForStrongLayout is false, following ivars are scanned:
/// - __weak anything
///
llvm37::Constant *CGObjCCommonMac::BuildIvarLayout(
  const ObjCImplementationDecl *OMD,
  bool ForStrongLayout) {
  bool hasUnion = false;

  llvm37::Type *PtrTy = CGM.Int8PtrTy;
  if (CGM.getLangOpts().getGC() == LangOptions::NonGC &&
      !CGM.getLangOpts().ObjCAutoRefCount)
    return llvm37::Constant::getNullValue(PtrTy);

  const ObjCInterfaceDecl *OI = OMD->getClassInterface();
  SmallVector<const FieldDecl*, 32> RecFields;
  if (CGM.getLangOpts().ObjCAutoRefCount) {
    for (const ObjCIvarDecl *IVD = OI->all_declared_ivar_begin(); 
         IVD; IVD = IVD->getNextIvar())
      RecFields.push_back(cast<FieldDecl>(IVD));
  }
  else {
    SmallVector<const ObjCIvarDecl*, 32> Ivars;
    CGM.getContext().DeepCollectObjCIvars(OI, true, Ivars);

    // FIXME: This is not ideal; we shouldn't have to do this copy.
    RecFields.append(Ivars.begin(), Ivars.end());
  }

  if (RecFields.empty())
    return llvm37::Constant::getNullValue(PtrTy);

  SkipIvars.clear();
  IvarsInfo.clear();

  BuildAggrIvarLayout(OMD, nullptr, nullptr, RecFields, 0, ForStrongLayout,
                      hasUnion);
  if (IvarsInfo.empty())
    return llvm37::Constant::getNullValue(PtrTy);
  // Sort on byte position in case we encounterred a union nested in
  // the ivar list.
  if (hasUnion && !IvarsInfo.empty())
    std::sort(IvarsInfo.begin(), IvarsInfo.end());
  if (hasUnion && !SkipIvars.empty())
    std::sort(SkipIvars.begin(), SkipIvars.end());
  
  std::string BitMap;
  llvm37::Constant *C = BuildIvarLayoutBitmap(BitMap);
  
   if (CGM.getLangOpts().ObjCGCBitmapPrint) {
    printf("\n%s ivar layout for class '%s': ",
           ForStrongLayout ? "strong" : "weak",
           OMD->getClassInterface()->getName().str().c_str());
    const unsigned char *s = (const unsigned char*)BitMap.c_str();
    for (unsigned i = 0, e = BitMap.size(); i < e; i++)
      if (!(s[i] & 0xf0))
        printf("0x0%x%s", s[i], s[i] != 0 ? ", " : "");
      else
        printf("0x%x%s",  s[i], s[i] != 0 ? ", " : "");
    printf("\n");
  }
  return C;
}

llvm37::Constant *CGObjCCommonMac::GetMethodVarName(Selector Sel) {
  llvm37::GlobalVariable *&Entry = MethodVarNames[Sel];

  // FIXME: Avoid std::string in "Sel.getAsString()"
  if (!Entry)
    Entry = CreateMetadataVar(
        "OBJC_METH_VAR_NAME_",
        llvm37::ConstantDataArray::getString(VMContext, Sel.getAsString()),
        ((ObjCABI == 2) ? "__TEXT,__objc_methname,cstring_literals"
                        : "__TEXT,__cstring,cstring_literals"),
        1, true);

  return getConstantGEP(VMContext, Entry, 0, 0);
}

// FIXME: Merge into a single cstring creation function.
llvm37::Constant *CGObjCCommonMac::GetMethodVarName(IdentifierInfo *ID) {
  return GetMethodVarName(CGM.getContext().Selectors.getNullarySelector(ID));
}

llvm37::Constant *CGObjCCommonMac::GetMethodVarType(const FieldDecl *Field) {
  std::string TypeStr;
  CGM.getContext().getObjCEncodingForType(Field->getType(), TypeStr, Field);

  llvm37::GlobalVariable *&Entry = MethodVarTypes[TypeStr];

  if (!Entry)
    Entry = CreateMetadataVar(
        "OBJC_METH_VAR_TYPE_",
        llvm37::ConstantDataArray::getString(VMContext, TypeStr),
        ((ObjCABI == 2) ? "__TEXT,__objc_methtype,cstring_literals"
                        : "__TEXT,__cstring,cstring_literals"),
        1, true);

  return getConstantGEP(VMContext, Entry, 0, 0);
}

llvm37::Constant *CGObjCCommonMac::GetMethodVarType(const ObjCMethodDecl *D,
                                                  bool Extended) {
  std::string TypeStr;
  if (CGM.getContext().getObjCEncodingForMethodDecl(D, TypeStr, Extended))
    return nullptr;

  llvm37::GlobalVariable *&Entry = MethodVarTypes[TypeStr];

  if (!Entry)
    Entry = CreateMetadataVar(
        "OBJC_METH_VAR_TYPE_",
        llvm37::ConstantDataArray::getString(VMContext, TypeStr),
        ((ObjCABI == 2) ? "__TEXT,__objc_methtype,cstring_literals"
                        : "__TEXT,__cstring,cstring_literals"),
        1, true);

  return getConstantGEP(VMContext, Entry, 0, 0);
}

// FIXME: Merge into a single cstring creation function.
llvm37::Constant *CGObjCCommonMac::GetPropertyName(IdentifierInfo *Ident) {
  llvm37::GlobalVariable *&Entry = PropertyNames[Ident];

  if (!Entry)
    Entry = CreateMetadataVar(
        "OBJC_PROP_NAME_ATTR_",
        llvm37::ConstantDataArray::getString(VMContext, Ident->getName()),
        "__TEXT,__cstring,cstring_literals", 1, true);

  return getConstantGEP(VMContext, Entry, 0, 0);
}

// FIXME: Merge into a single cstring creation function.
// FIXME: This Decl should be more precise.
llvm37::Constant *
CGObjCCommonMac::GetPropertyTypeString(const ObjCPropertyDecl *PD,
                                       const Decl *Container) {
  std::string TypeStr;
  CGM.getContext().getObjCEncodingForPropertyDecl(PD, Container, TypeStr);
  return GetPropertyName(&CGM.getContext().Idents.get(TypeStr));
}

void CGObjCCommonMac::GetNameForMethod(const ObjCMethodDecl *D,
                                       const ObjCContainerDecl *CD,
                                       SmallVectorImpl<char> &Name) {
  llvm37::raw_svector_ostream OS(Name);
  assert (CD && "Missing container decl in GetNameForMethod");
  OS << '\01' << (D->isInstanceMethod() ? '-' : '+')
     << '[' << CD->getName();
  if (const ObjCCategoryImplDecl *CID =
      dyn_cast<ObjCCategoryImplDecl>(D->getDeclContext()))
    OS << '(' << *CID << ')';
  OS << ' ' << D->getSelector().getAsString() << ']';
}

void CGObjCMac::FinishModule() {
  EmitModuleInfo();

  // Emit the dummy bodies for any protocols which were referenced but
  // never defined.
  for (llvm37::DenseMap<IdentifierInfo*, llvm37::GlobalVariable*>::iterator
         I = Protocols.begin(), e = Protocols.end(); I != e; ++I) {
    if (I->second->hasInitializer())
      continue;

    llvm37::Constant *Values[5];
    Values[0] = llvm37::Constant::getNullValue(ObjCTypes.ProtocolExtensionPtrTy);
    Values[1] = GetClassName(I->first->getName());
    Values[2] = llvm37::Constant::getNullValue(ObjCTypes.ProtocolListPtrTy);
    Values[3] = Values[4] =
      llvm37::Constant::getNullValue(ObjCTypes.MethodDescriptionListPtrTy);
    I->second->setInitializer(llvm37::ConstantStruct::get(ObjCTypes.ProtocolTy,
                                                        Values));
    CGM.addCompilerUsedGlobal(I->second);
  }

  // Add assembler directives to add lazy undefined symbol references
  // for classes which are referenced but not defined. This is
  // important for correct linker interaction.
  //
  // FIXME: It would be nice if we had an LLVM37 construct for this.
  if (!LazySymbols.empty() || !DefinedSymbols.empty()) {
    SmallString<256> Asm;
    Asm += CGM.getModule().getModuleInlineAsm();
    if (!Asm.empty() && Asm.back() != '\n')
      Asm += '\n';

    llvm37::raw_svector_ostream OS(Asm);
    for (llvm37::SetVector<IdentifierInfo*>::iterator I = DefinedSymbols.begin(),
           e = DefinedSymbols.end(); I != e; ++I)
      OS << "\t.objc_class_name_" << (*I)->getName() << "=0\n"
         << "\t.globl .objc_class_name_" << (*I)->getName() << "\n";
    for (llvm37::SetVector<IdentifierInfo*>::iterator I = LazySymbols.begin(),
         e = LazySymbols.end(); I != e; ++I) {
      OS << "\t.lazy_reference .objc_class_name_" << (*I)->getName() << "\n";
    }

    for (size_t i = 0, e = DefinedCategoryNames.size(); i < e; ++i) {
      OS << "\t.objc_category_name_" << DefinedCategoryNames[i] << "=0\n"
         << "\t.globl .objc_category_name_" << DefinedCategoryNames[i] << "\n";
    }
    
    CGM.getModule().setModuleInlineAsm(OS.str());
  }
}

CGObjCNonFragileABIMac::CGObjCNonFragileABIMac(CodeGen::CodeGenModule &cgm)
  : CGObjCCommonMac(cgm),
    ObjCTypes(cgm) {
  ObjCEmptyCacheVar = ObjCEmptyVtableVar = nullptr;
  ObjCABI = 2;
}

/* *** */

ObjCCommonTypesHelper::ObjCCommonTypesHelper(CodeGen::CodeGenModule &cgm)
  : VMContext(cgm.getLLVM37Context()), CGM(cgm), ExternalProtocolPtrTy(nullptr)
{
  CodeGen::CodeGenTypes &Types = CGM.getTypes();
  ASTContext &Ctx = CGM.getContext();

  ShortTy = Types.ConvertType(Ctx.ShortTy);
  IntTy = Types.ConvertType(Ctx.IntTy);
  LongTy = Types.ConvertType(Ctx.LongTy);
  LongLongTy = Types.ConvertType(Ctx.LongLongTy);
  Int8PtrTy = CGM.Int8PtrTy;
  Int8PtrPtrTy = CGM.Int8PtrPtrTy;

  // arm64 targets use "int" ivar offset variables. All others,
  // including OS X x86_64 and Windows x86_64, use "long" ivar offsets.
  if (CGM.getTarget().getTriple().getArch() == llvm37::Triple::aarch64)
    IvarOffsetVarTy = IntTy;
  else
    IvarOffsetVarTy = LongTy;

  ObjectPtrTy = Types.ConvertType(Ctx.getObjCIdType());
  PtrObjectPtrTy = llvm37::PointerType::getUnqual(ObjectPtrTy);
  SelectorPtrTy = Types.ConvertType(Ctx.getObjCSelType());

  // I'm not sure I like this. The implicit coordination is a bit
  // gross. We should solve this in a reasonable fashion because this
  // is a pretty common task (match some runtime data structure with
  // an LLVM37 data structure).

  // FIXME: This is leaked.
  // FIXME: Merge with rewriter code?

  // struct _objc_super {
  //   id self;
  //   Class cls;
  // }
  RecordDecl *RD = RecordDecl::Create(Ctx, TTK_Struct,
                                      Ctx.getTranslationUnitDecl(),
                                      SourceLocation(), SourceLocation(),
                                      &Ctx.Idents.get("_objc_super"));
  RD->addDecl(FieldDecl::Create(Ctx, RD, SourceLocation(), SourceLocation(),
                                nullptr, Ctx.getObjCIdType(), nullptr, nullptr,
                                false, ICIS_NoInit));
  RD->addDecl(FieldDecl::Create(Ctx, RD, SourceLocation(), SourceLocation(),
                                nullptr, Ctx.getObjCClassType(), nullptr,
                                nullptr, false, ICIS_NoInit));
  RD->completeDefinition();

  SuperCTy = Ctx.getTagDeclType(RD);
  SuperPtrCTy = Ctx.getPointerType(SuperCTy);

  SuperTy = cast<llvm37::StructType>(Types.ConvertType(SuperCTy));
  SuperPtrTy = llvm37::PointerType::getUnqual(SuperTy);

  // struct _prop_t {
  //   char *name;
  //   char *attributes;
  // }
  PropertyTy = llvm37::StructType::create("struct._prop_t",
                                        Int8PtrTy, Int8PtrTy, nullptr);

  // struct _prop_list_t {
  //   uint32_t entsize;      // sizeof(struct _prop_t)
  //   uint32_t count_of_properties;
  //   struct _prop_t prop_list[count_of_properties];
  // }
  PropertyListTy =
    llvm37::StructType::create("struct._prop_list_t", IntTy, IntTy,
                             llvm37::ArrayType::get(PropertyTy, 0), nullptr);
  // struct _prop_list_t *
  PropertyListPtrTy = llvm37::PointerType::getUnqual(PropertyListTy);

  // struct _objc_method {
  //   SEL _cmd;
  //   char *method_type;
  //   char *_imp;
  // }
  MethodTy = llvm37::StructType::create("struct._objc_method",
                                      SelectorPtrTy, Int8PtrTy, Int8PtrTy,
                                      nullptr);

  // struct _objc_cache *
  CacheTy = llvm37::StructType::create(VMContext, "struct._objc_cache");
  CachePtrTy = llvm37::PointerType::getUnqual(CacheTy);
    
}

ObjCTypesHelper::ObjCTypesHelper(CodeGen::CodeGenModule &cgm)
  : ObjCCommonTypesHelper(cgm) {
  // struct _objc_method_description {
  //   SEL name;
  //   char *types;
  // }
  MethodDescriptionTy =
    llvm37::StructType::create("struct._objc_method_description",
                             SelectorPtrTy, Int8PtrTy, nullptr);

  // struct _objc_method_description_list {
  //   int count;
  //   struct _objc_method_description[1];
  // }
  MethodDescriptionListTy = llvm37::StructType::create(
      "struct._objc_method_description_list", IntTy,
      llvm37::ArrayType::get(MethodDescriptionTy, 0), nullptr);

  // struct _objc_method_description_list *
  MethodDescriptionListPtrTy =
    llvm37::PointerType::getUnqual(MethodDescriptionListTy);

  // Protocol description structures

  // struct _objc_protocol_extension {
  //   uint32_t size;  // sizeof(struct _objc_protocol_extension)
  //   struct _objc_method_description_list *optional_instance_methods;
  //   struct _objc_method_description_list *optional_class_methods;
  //   struct _objc_property_list *instance_properties;
  //   const char ** extendedMethodTypes;
  // }
  ProtocolExtensionTy =
    llvm37::StructType::create("struct._objc_protocol_extension",
                             IntTy, MethodDescriptionListPtrTy,
                             MethodDescriptionListPtrTy, PropertyListPtrTy,
                             Int8PtrPtrTy, nullptr);

  // struct _objc_protocol_extension *
  ProtocolExtensionPtrTy = llvm37::PointerType::getUnqual(ProtocolExtensionTy);

  // Handle recursive construction of Protocol and ProtocolList types

  ProtocolTy =
    llvm37::StructType::create(VMContext, "struct._objc_protocol");

  ProtocolListTy =
    llvm37::StructType::create(VMContext, "struct._objc_protocol_list");
  ProtocolListTy->setBody(llvm37::PointerType::getUnqual(ProtocolListTy),
                          LongTy,
                          llvm37::ArrayType::get(ProtocolTy, 0),
                          nullptr);

  // struct _objc_protocol {
  //   struct _objc_protocol_extension *isa;
  //   char *protocol_name;
  //   struct _objc_protocol **_objc_protocol_list;
  //   struct _objc_method_description_list *instance_methods;
  //   struct _objc_method_description_list *class_methods;
  // }
  ProtocolTy->setBody(ProtocolExtensionPtrTy, Int8PtrTy,
                      llvm37::PointerType::getUnqual(ProtocolListTy),
                      MethodDescriptionListPtrTy,
                      MethodDescriptionListPtrTy,
                      nullptr);

  // struct _objc_protocol_list *
  ProtocolListPtrTy = llvm37::PointerType::getUnqual(ProtocolListTy);

  ProtocolPtrTy = llvm37::PointerType::getUnqual(ProtocolTy);

  // Class description structures

  // struct _objc_ivar {
  //   char *ivar_name;
  //   char *ivar_type;
  //   int  ivar_offset;
  // }
  IvarTy = llvm37::StructType::create("struct._objc_ivar",
                                    Int8PtrTy, Int8PtrTy, IntTy, nullptr);

  // struct _objc_ivar_list *
  IvarListTy =
    llvm37::StructType::create(VMContext, "struct._objc_ivar_list");
  IvarListPtrTy = llvm37::PointerType::getUnqual(IvarListTy);

  // struct _objc_method_list *
  MethodListTy =
    llvm37::StructType::create(VMContext, "struct._objc_method_list");
  MethodListPtrTy = llvm37::PointerType::getUnqual(MethodListTy);

  // struct _objc_class_extension *
  ClassExtensionTy =
    llvm37::StructType::create("struct._objc_class_extension",
                             IntTy, Int8PtrTy, PropertyListPtrTy, nullptr);
  ClassExtensionPtrTy = llvm37::PointerType::getUnqual(ClassExtensionTy);

  ClassTy = llvm37::StructType::create(VMContext, "struct._objc_class");

  // struct _objc_class {
  //   Class isa;
  //   Class super_class;
  //   char *name;
  //   long version;
  //   long info;
  //   long instance_size;
  //   struct _objc_ivar_list *ivars;
  //   struct _objc_method_list *methods;
  //   struct _objc_cache *cache;
  //   struct _objc_protocol_list *protocols;
  //   char *ivar_layout;
  //   struct _objc_class_ext *ext;
  // };
  ClassTy->setBody(llvm37::PointerType::getUnqual(ClassTy),
                   llvm37::PointerType::getUnqual(ClassTy),
                   Int8PtrTy,
                   LongTy,
                   LongTy,
                   LongTy,
                   IvarListPtrTy,
                   MethodListPtrTy,
                   CachePtrTy,
                   ProtocolListPtrTy,
                   Int8PtrTy,
                   ClassExtensionPtrTy,
                   nullptr);

  ClassPtrTy = llvm37::PointerType::getUnqual(ClassTy);

  // struct _objc_category {
  //   char *category_name;
  //   char *class_name;
  //   struct _objc_method_list *instance_method;
  //   struct _objc_method_list *class_method;
  //   uint32_t size;  // sizeof(struct _objc_category)
  //   struct _objc_property_list *instance_properties;// category's @property
  // }
  CategoryTy =
    llvm37::StructType::create("struct._objc_category",
                             Int8PtrTy, Int8PtrTy, MethodListPtrTy,
                             MethodListPtrTy, ProtocolListPtrTy,
                             IntTy, PropertyListPtrTy, nullptr);

  // Global metadata structures

  // struct _objc_symtab {
  //   long sel_ref_cnt;
  //   SEL *refs;
  //   short cls_def_cnt;
  //   short cat_def_cnt;
  //   char *defs[cls_def_cnt + cat_def_cnt];
  // }
  SymtabTy =
    llvm37::StructType::create("struct._objc_symtab",
                             LongTy, SelectorPtrTy, ShortTy, ShortTy,
                             llvm37::ArrayType::get(Int8PtrTy, 0), nullptr);
  SymtabPtrTy = llvm37::PointerType::getUnqual(SymtabTy);

  // struct _objc_module {
  //   long version;
  //   long size;   // sizeof(struct _objc_module)
  //   char *name;
  //   struct _objc_symtab* symtab;
  //  }
  ModuleTy =
    llvm37::StructType::create("struct._objc_module",
                             LongTy, LongTy, Int8PtrTy, SymtabPtrTy, nullptr);


  // FIXME: This is the size of the setjmp buffer and should be target
  // specific. 18 is what's used on 32-bit X86.
  uint64_t SetJmpBufferSize = 18;

  // Exceptions
  llvm37::Type *StackPtrTy = llvm37::ArrayType::get(CGM.Int8PtrTy, 4);

  ExceptionDataTy =
    llvm37::StructType::create("struct._objc_exception_data",
                             llvm37::ArrayType::get(CGM.Int32Ty,SetJmpBufferSize),
                             StackPtrTy, nullptr);

}

ObjCNonFragileABITypesHelper::ObjCNonFragileABITypesHelper(CodeGen::CodeGenModule &cgm)
  : ObjCCommonTypesHelper(cgm) {
  // struct _method_list_t {
  //   uint32_t entsize;  // sizeof(struct _objc_method)
  //   uint32_t method_count;
  //   struct _objc_method method_list[method_count];
  // }
  MethodListnfABITy =
    llvm37::StructType::create("struct.__method_list_t", IntTy, IntTy,
                             llvm37::ArrayType::get(MethodTy, 0), nullptr);
  // struct method_list_t *
  MethodListnfABIPtrTy = llvm37::PointerType::getUnqual(MethodListnfABITy);

  // struct _protocol_t {
  //   id isa;  // NULL
  //   const char * const protocol_name;
  //   const struct _protocol_list_t * protocol_list; // super protocols
  //   const struct method_list_t * const instance_methods;
  //   const struct method_list_t * const class_methods;
  //   const struct method_list_t *optionalInstanceMethods;
  //   const struct method_list_t *optionalClassMethods;
  //   const struct _prop_list_t * properties;
  //   const uint32_t size;  // sizeof(struct _protocol_t)
  //   const uint32_t flags;  // = 0
  //   const char ** extendedMethodTypes;
  //   const char *demangledName;
  // }

  // Holder for struct _protocol_list_t *
  ProtocolListnfABITy =
    llvm37::StructType::create(VMContext, "struct._objc_protocol_list");

  ProtocolnfABITy =
    llvm37::StructType::create("struct._protocol_t", ObjectPtrTy, Int8PtrTy,
                             llvm37::PointerType::getUnqual(ProtocolListnfABITy),
                             MethodListnfABIPtrTy, MethodListnfABIPtrTy,
                             MethodListnfABIPtrTy, MethodListnfABIPtrTy,
                             PropertyListPtrTy, IntTy, IntTy, Int8PtrPtrTy,
                             Int8PtrTy,
                             nullptr);

  // struct _protocol_t*
  ProtocolnfABIPtrTy = llvm37::PointerType::getUnqual(ProtocolnfABITy);

  // struct _protocol_list_t {
  //   long protocol_count;   // Note, this is 32/64 bit
  //   struct _protocol_t *[protocol_count];
  // }
  ProtocolListnfABITy->setBody(LongTy,
                               llvm37::ArrayType::get(ProtocolnfABIPtrTy, 0),
                               nullptr);

  // struct _objc_protocol_list*
  ProtocolListnfABIPtrTy = llvm37::PointerType::getUnqual(ProtocolListnfABITy);

  // struct _ivar_t {
  //   unsigned [long] int *offset;  // pointer to ivar offset location
  //   char *name;
  //   char *type;
  //   uint32_t alignment;
  //   uint32_t size;
  // }
  IvarnfABITy = llvm37::StructType::create(
      "struct._ivar_t", llvm37::PointerType::getUnqual(IvarOffsetVarTy),
      Int8PtrTy, Int8PtrTy, IntTy, IntTy, nullptr);

  // struct _ivar_list_t {
  //   uint32 entsize;  // sizeof(struct _ivar_t)
  //   uint32 count;
  //   struct _iver_t list[count];
  // }
  IvarListnfABITy =
    llvm37::StructType::create("struct._ivar_list_t", IntTy, IntTy,
                             llvm37::ArrayType::get(IvarnfABITy, 0), nullptr);

  IvarListnfABIPtrTy = llvm37::PointerType::getUnqual(IvarListnfABITy);

  // struct _class_ro_t {
  //   uint32_t const flags;
  //   uint32_t const instanceStart;
  //   uint32_t const instanceSize;
  //   uint32_t const reserved;  // only when building for 64bit targets
  //   const uint8_t * const ivarLayout;
  //   const char *const name;
  //   const struct _method_list_t * const baseMethods;
  //   const struct _objc_protocol_list *const baseProtocols;
  //   const struct _ivar_list_t *const ivars;
  //   const uint8_t * const weakIvarLayout;
  //   const struct _prop_list_t * const properties;
  // }

  // FIXME. Add 'reserved' field in 64bit abi mode!
  ClassRonfABITy = llvm37::StructType::create("struct._class_ro_t",
                                            IntTy, IntTy, IntTy, Int8PtrTy,
                                            Int8PtrTy, MethodListnfABIPtrTy,
                                            ProtocolListnfABIPtrTy,
                                            IvarListnfABIPtrTy,
                                            Int8PtrTy, PropertyListPtrTy,
                                            nullptr);

  // ImpnfABITy - LLVM37 for id (*)(id, SEL, ...)
  llvm37::Type *params[] = { ObjectPtrTy, SelectorPtrTy };
  ImpnfABITy = llvm37::FunctionType::get(ObjectPtrTy, params, false)
                 ->getPointerTo();

  // struct _class_t {
  //   struct _class_t *isa;
  //   struct _class_t * const superclass;
  //   void *cache;
  //   IMP *vtable;
  //   struct class_ro_t *ro;
  // }

  ClassnfABITy = llvm37::StructType::create(VMContext, "struct._class_t");
  ClassnfABITy->setBody(llvm37::PointerType::getUnqual(ClassnfABITy),
                        llvm37::PointerType::getUnqual(ClassnfABITy),
                        CachePtrTy,
                        llvm37::PointerType::getUnqual(ImpnfABITy),
                        llvm37::PointerType::getUnqual(ClassRonfABITy),
                        nullptr);

  // LLVM37 for struct _class_t *
  ClassnfABIPtrTy = llvm37::PointerType::getUnqual(ClassnfABITy);

  // struct _category_t {
  //   const char * const name;
  //   struct _class_t *const cls;
  //   const struct _method_list_t * const instance_methods;
  //   const struct _method_list_t * const class_methods;
  //   const struct _protocol_list_t * const protocols;
  //   const struct _prop_list_t * const properties;
  // }
  CategorynfABITy = llvm37::StructType::create("struct._category_t",
                                             Int8PtrTy, ClassnfABIPtrTy,
                                             MethodListnfABIPtrTy,
                                             MethodListnfABIPtrTy,
                                             ProtocolListnfABIPtrTy,
                                             PropertyListPtrTy,
                                             nullptr);

  // New types for nonfragile abi messaging.
  CodeGen::CodeGenTypes &Types = CGM.getTypes();
  ASTContext &Ctx = CGM.getContext();

  // MessageRefTy - LLVM37 for:
  // struct _message_ref_t {
  //   IMP messenger;
  //   SEL name;
  // };

  // First the clang type for struct _message_ref_t
  RecordDecl *RD = RecordDecl::Create(Ctx, TTK_Struct,
                                      Ctx.getTranslationUnitDecl(),
                                      SourceLocation(), SourceLocation(),
                                      &Ctx.Idents.get("_message_ref_t"));
  RD->addDecl(FieldDecl::Create(Ctx, RD, SourceLocation(), SourceLocation(),
                                nullptr, Ctx.VoidPtrTy, nullptr, nullptr, false,
                                ICIS_NoInit));
  RD->addDecl(FieldDecl::Create(Ctx, RD, SourceLocation(), SourceLocation(),
                                nullptr, Ctx.getObjCSelType(), nullptr, nullptr,
                                false, ICIS_NoInit));
  RD->completeDefinition();

  MessageRefCTy = Ctx.getTagDeclType(RD);
  MessageRefCPtrTy = Ctx.getPointerType(MessageRefCTy);
  MessageRefTy = cast<llvm37::StructType>(Types.ConvertType(MessageRefCTy));

  // MessageRefPtrTy - LLVM37 for struct _message_ref_t*
  MessageRefPtrTy = llvm37::PointerType::getUnqual(MessageRefTy);

  // SuperMessageRefTy - LLVM37 for:
  // struct _super_message_ref_t {
  //   SUPER_IMP messenger;
  //   SEL name;
  // };
  SuperMessageRefTy =
    llvm37::StructType::create("struct._super_message_ref_t",
                             ImpnfABITy, SelectorPtrTy, nullptr);

  // SuperMessageRefPtrTy - LLVM37 for struct _super_message_ref_t*
  SuperMessageRefPtrTy = llvm37::PointerType::getUnqual(SuperMessageRefTy);
    

  // struct objc_typeinfo {
  //   const void** vtable; // objc_ehtype_vtable + 2
  //   const char*  name;    // c++ typeinfo string
  //   Class        cls;
  // };
  EHTypeTy =
    llvm37::StructType::create("struct._objc_typeinfo",
                             llvm37::PointerType::getUnqual(Int8PtrTy),
                             Int8PtrTy, ClassnfABIPtrTy, nullptr);
  EHTypePtrTy = llvm37::PointerType::getUnqual(EHTypeTy);
}

llvm37::Function *CGObjCNonFragileABIMac::ModuleInitFunction() {
  FinishNonFragileABIModule();

  return nullptr;
}

void CGObjCNonFragileABIMac::
AddModuleClassList(ArrayRef<llvm37::GlobalValue*> Container,
                   const char *SymbolName,
                   const char *SectionName) {
  unsigned NumClasses = Container.size();

  if (!NumClasses)
    return;

  SmallVector<llvm37::Constant*, 8> Symbols(NumClasses);
  for (unsigned i=0; i<NumClasses; i++)
    Symbols[i] = llvm37::ConstantExpr::getBitCast(Container[i],
                                                ObjCTypes.Int8PtrTy);
  llvm37::Constant *Init =
    llvm37::ConstantArray::get(llvm37::ArrayType::get(ObjCTypes.Int8PtrTy,
                                                  Symbols.size()),
                             Symbols);

  llvm37::GlobalVariable *GV =
    new llvm37::GlobalVariable(CGM.getModule(), Init->getType(), false,
                             llvm37::GlobalValue::PrivateLinkage,
                             Init,
                             SymbolName);
  GV->setAlignment(CGM.getDataLayout().getABITypeAlignment(Init->getType()));
  GV->setSection(SectionName);
  CGM.addCompilerUsedGlobal(GV);
}

void CGObjCNonFragileABIMac::FinishNonFragileABIModule() {
  // nonfragile abi has no module definition.

  // Build list of all implemented class addresses in array
  // L_OBJC_LABEL_CLASS_$.

  for (unsigned i=0, NumClasses=ImplementedClasses.size(); i<NumClasses; i++) {
    const ObjCInterfaceDecl *ID = ImplementedClasses[i];
    assert(ID);
    if (ObjCImplementationDecl *IMP = ID->getImplementation())
      // We are implementing a weak imported interface. Give it external linkage
      if (ID->isWeakImported() && !IMP->isWeakImported()) {
        DefinedClasses[i]->setLinkage(llvm37::GlobalVariable::ExternalLinkage);
        DefinedMetaClasses[i]->setLinkage(llvm37::GlobalVariable::ExternalLinkage);
      }
  }

  AddModuleClassList(DefinedClasses, "OBJC_LABEL_CLASS_$",
                     "__DATA, __objc_classlist, regular, no_dead_strip");

  AddModuleClassList(DefinedNonLazyClasses, "OBJC_LABEL_NONLAZY_CLASS_$",
                     "__DATA, __objc_nlclslist, regular, no_dead_strip");

  // Build list of all implemented category addresses in array
  // L_OBJC_LABEL_CATEGORY_$.
  AddModuleClassList(DefinedCategories, "OBJC_LABEL_CATEGORY_$",
                     "__DATA, __objc_catlist, regular, no_dead_strip");
  AddModuleClassList(DefinedNonLazyCategories, "OBJC_LABEL_NONLAZY_CATEGORY_$",
                     "__DATA, __objc_nlcatlist, regular, no_dead_strip");

  EmitImageInfo();
}

/// isVTableDispatchedSelector - Returns true if SEL is not in the list of
/// VTableDispatchMethods; false otherwise. What this means is that
/// except for the 19 selectors in the list, we generate 32bit-style
/// message dispatch call for all the rest.
bool CGObjCNonFragileABIMac::isVTableDispatchedSelector(Selector Sel) {
  // At various points we've experimented with using vtable-based
  // dispatch for all methods.
  switch (CGM.getCodeGenOpts().getObjCDispatchMethod()) {
  case CodeGenOptions::Legacy:
    return false;
  case CodeGenOptions::NonLegacy:
    return true;
  case CodeGenOptions::Mixed:
    break;
  }

  // If so, see whether this selector is in the white-list of things which must
  // use the new dispatch convention. We lazily build a dense set for this.
  if (VTableDispatchMethods.empty()) {
    VTableDispatchMethods.insert(GetNullarySelector("alloc"));
    VTableDispatchMethods.insert(GetNullarySelector("class"));
    VTableDispatchMethods.insert(GetNullarySelector("self"));
    VTableDispatchMethods.insert(GetNullarySelector("isFlipped"));
    VTableDispatchMethods.insert(GetNullarySelector("length"));
    VTableDispatchMethods.insert(GetNullarySelector("count"));

    // These are vtable-based if GC is disabled.
    // Optimistically use vtable dispatch for hybrid compiles.
    if (CGM.getLangOpts().getGC() != LangOptions::GCOnly) {
      VTableDispatchMethods.insert(GetNullarySelector("retain"));
      VTableDispatchMethods.insert(GetNullarySelector("release"));
      VTableDispatchMethods.insert(GetNullarySelector("autorelease"));
    }

    VTableDispatchMethods.insert(GetUnarySelector("allocWithZone"));
    VTableDispatchMethods.insert(GetUnarySelector("isKindOfClass"));
    VTableDispatchMethods.insert(GetUnarySelector("respondsToSelector"));
    VTableDispatchMethods.insert(GetUnarySelector("objectForKey"));
    VTableDispatchMethods.insert(GetUnarySelector("objectAtIndex"));
    VTableDispatchMethods.insert(GetUnarySelector("isEqualToString"));
    VTableDispatchMethods.insert(GetUnarySelector("isEqual"));

    // These are vtable-based if GC is enabled.
    // Optimistically use vtable dispatch for hybrid compiles.
    if (CGM.getLangOpts().getGC() != LangOptions::NonGC) {
      VTableDispatchMethods.insert(GetNullarySelector("hash"));
      VTableDispatchMethods.insert(GetUnarySelector("addObject"));
    
      // "countByEnumeratingWithState:objects:count"
      IdentifierInfo *KeyIdents[] = {
        &CGM.getContext().Idents.get("countByEnumeratingWithState"),
        &CGM.getContext().Idents.get("objects"),
        &CGM.getContext().Idents.get("count")
      };
      VTableDispatchMethods.insert(
        CGM.getContext().Selectors.getSelector(3, KeyIdents));
    }
  }

  return VTableDispatchMethods.count(Sel);
}

/// BuildClassRoTInitializer - generate meta-data for:
/// struct _class_ro_t {
///   uint32_t const flags;
///   uint32_t const instanceStart;
///   uint32_t const instanceSize;
///   uint32_t const reserved;  // only when building for 64bit targets
///   const uint8_t * const ivarLayout;
///   const char *const name;
///   const struct _method_list_t * const baseMethods;
///   const struct _protocol_list_t *const baseProtocols;
///   const struct _ivar_list_t *const ivars;
///   const uint8_t * const weakIvarLayout;
///   const struct _prop_list_t * const properties;
/// }
///
llvm37::GlobalVariable * CGObjCNonFragileABIMac::BuildClassRoTInitializer(
  unsigned flags,
  unsigned InstanceStart,
  unsigned InstanceSize,
  const ObjCImplementationDecl *ID) {
  std::string ClassName = ID->getObjCRuntimeNameAsString();
  llvm37::Constant *Values[10]; // 11 for 64bit targets!

  if (CGM.getLangOpts().ObjCAutoRefCount)
    flags |= NonFragileABI_Class_CompiledByARC;

  Values[ 0] = llvm37::ConstantInt::get(ObjCTypes.IntTy, flags);
  Values[ 1] = llvm37::ConstantInt::get(ObjCTypes.IntTy, InstanceStart);
  Values[ 2] = llvm37::ConstantInt::get(ObjCTypes.IntTy, InstanceSize);
  // FIXME. For 64bit targets add 0 here.
  Values[ 3] = (flags & NonFragileABI_Class_Meta)
    ? GetIvarLayoutName(nullptr, ObjCTypes)
    : BuildIvarLayout(ID, true);
  Values[ 4] = GetClassName(ID->getObjCRuntimeNameAsString());
  // const struct _method_list_t * const baseMethods;
  std::vector<llvm37::Constant*> Methods;
  std::string MethodListName("\01l_OBJC_$_");
  if (flags & NonFragileABI_Class_Meta) {
    MethodListName += "CLASS_METHODS_";
    MethodListName += ID->getObjCRuntimeNameAsString();
    for (const auto *I : ID->class_methods())
      // Class methods should always be defined.
      Methods.push_back(GetMethodConstant(I));
  } else {
    MethodListName += "INSTANCE_METHODS_";
    MethodListName += ID->getObjCRuntimeNameAsString();
    for (const auto *I : ID->instance_methods())
      // Instance methods should always be defined.
      Methods.push_back(GetMethodConstant(I));

    for (const auto *PID : ID->property_impls()) {
      if (PID->getPropertyImplementation() == ObjCPropertyImplDecl::Synthesize){
        ObjCPropertyDecl *PD = PID->getPropertyDecl();

        if (ObjCMethodDecl *MD = PD->getGetterMethodDecl())
          if (llvm37::Constant *C = GetMethodConstant(MD))
            Methods.push_back(C);
        if (ObjCMethodDecl *MD = PD->getSetterMethodDecl())
          if (llvm37::Constant *C = GetMethodConstant(MD))
            Methods.push_back(C);
      }
    }
  }
  Values[ 5] = EmitMethodList(MethodListName,
                              "__DATA, __objc_const", Methods);

  const ObjCInterfaceDecl *OID = ID->getClassInterface();
  assert(OID && "CGObjCNonFragileABIMac::BuildClassRoTInitializer");
  Values[ 6] = EmitProtocolList("\01l_OBJC_CLASS_PROTOCOLS_$_"
                                + OID->getObjCRuntimeNameAsString(),
                                OID->all_referenced_protocol_begin(),
                                OID->all_referenced_protocol_end());

  if (flags & NonFragileABI_Class_Meta) {
    Values[ 7] = llvm37::Constant::getNullValue(ObjCTypes.IvarListnfABIPtrTy);
    Values[ 8] = GetIvarLayoutName(nullptr, ObjCTypes);
    Values[ 9] = llvm37::Constant::getNullValue(ObjCTypes.PropertyListPtrTy);
  } else {
    Values[ 7] = EmitIvarList(ID);
    Values[ 8] = BuildIvarLayout(ID, false);
    Values[ 9] = EmitPropertyList("\01l_OBJC_$_PROP_LIST_" + ID->getObjCRuntimeNameAsString(),
                                  ID, ID->getClassInterface(), ObjCTypes);
  }
  llvm37::Constant *Init = llvm37::ConstantStruct::get(ObjCTypes.ClassRonfABITy,
                                                   Values);
  llvm37::GlobalVariable *CLASS_RO_GV =
    new llvm37::GlobalVariable(CGM.getModule(), ObjCTypes.ClassRonfABITy, false,
                             llvm37::GlobalValue::PrivateLinkage,
                             Init,
                             (flags & NonFragileABI_Class_Meta) ?
                             std::string("\01l_OBJC_METACLASS_RO_$_")+ClassName :
                             std::string("\01l_OBJC_CLASS_RO_$_")+ClassName);
  CLASS_RO_GV->setAlignment(
    CGM.getDataLayout().getABITypeAlignment(ObjCTypes.ClassRonfABITy));
  CLASS_RO_GV->setSection("__DATA, __objc_const");
  return CLASS_RO_GV;

}

/// BuildClassMetaData - This routine defines that to-level meta-data
/// for the given ClassName for:
/// struct _class_t {
///   struct _class_t *isa;
///   struct _class_t * const superclass;
///   void *cache;
///   IMP *vtable;
///   struct class_ro_t *ro;
/// }
///
llvm37::GlobalVariable *CGObjCNonFragileABIMac::BuildClassMetaData(
    const std::string &ClassName, llvm37::Constant *IsAGV, llvm37::Constant *SuperClassGV,
    llvm37::Constant *ClassRoGV, bool HiddenVisibility, bool Weak) {
  llvm37::Constant *Values[] = {
    IsAGV,
    SuperClassGV,
    ObjCEmptyCacheVar,  // &ObjCEmptyCacheVar
    ObjCEmptyVtableVar, // &ObjCEmptyVtableVar
    ClassRoGV           // &CLASS_RO_GV
  };
  if (!Values[1])
    Values[1] = llvm37::Constant::getNullValue(ObjCTypes.ClassnfABIPtrTy);
  if (!Values[3])
    Values[3] = llvm37::Constant::getNullValue(
                  llvm37::PointerType::getUnqual(ObjCTypes.ImpnfABITy));
  llvm37::Constant *Init = llvm37::ConstantStruct::get(ObjCTypes.ClassnfABITy,
                                                   Values);
  llvm37::GlobalVariable *GV = GetClassGlobal(ClassName, Weak);
  GV->setInitializer(Init);
  GV->setSection("__DATA, __objc_data");
  GV->setAlignment(
    CGM.getDataLayout().getABITypeAlignment(ObjCTypes.ClassnfABITy));
  if (HiddenVisibility)
    GV->setVisibility(llvm37::GlobalValue::HiddenVisibility);
  return GV;
}

bool
CGObjCNonFragileABIMac::ImplementationIsNonLazy(const ObjCImplDecl *OD) const {
  return OD->getClassMethod(GetNullarySelector("load")) != nullptr;
}

void CGObjCNonFragileABIMac::GetClassSizeInfo(const ObjCImplementationDecl *OID,
                                              uint32_t &InstanceStart,
                                              uint32_t &InstanceSize) {
  const ASTRecordLayout &RL =
    CGM.getContext().getASTObjCImplementationLayout(OID);

  // InstanceSize is really instance end.
  InstanceSize = RL.getDataSize().getQuantity();

  // If there are no fields, the start is the same as the end.
  if (!RL.getFieldCount())
    InstanceStart = InstanceSize;
  else
    InstanceStart = RL.getFieldOffset(0) / CGM.getContext().getCharWidth();
}

void CGObjCNonFragileABIMac::GenerateClass(const ObjCImplementationDecl *ID) {
  std::string ClassName = ID->getObjCRuntimeNameAsString();
  if (!ObjCEmptyCacheVar) {
    ObjCEmptyCacheVar = new llvm37::GlobalVariable(
      CGM.getModule(),
      ObjCTypes.CacheTy,
      false,
      llvm37::GlobalValue::ExternalLinkage,
      nullptr,
      "_objc_empty_cache");

    // Make this entry NULL for any iOS device target, any iOS simulator target,
    // OS X with deployment target 10.9 or later.
    const llvm37::Triple &Triple = CGM.getTarget().getTriple();
    if (Triple.isiOS() || (Triple.isMacOSX() && !Triple.isMacOSXVersionLT(10, 9)))
      // This entry will be null.
      ObjCEmptyVtableVar = nullptr;
    else
      ObjCEmptyVtableVar = new llvm37::GlobalVariable(
                                                    CGM.getModule(),
                                                    ObjCTypes.ImpnfABITy,
                                                    false,
                                                    llvm37::GlobalValue::ExternalLinkage,
                                                    nullptr,
                                                    "_objc_empty_vtable");
  }
  assert(ID->getClassInterface() &&
         "CGObjCNonFragileABIMac::GenerateClass - class is 0");
  // FIXME: Is this correct (that meta class size is never computed)?
  uint32_t InstanceStart =
    CGM.getDataLayout().getTypeAllocSize(ObjCTypes.ClassnfABITy);
  uint32_t InstanceSize = InstanceStart;
  uint32_t flags = NonFragileABI_Class_Meta;
  llvm37::SmallString<64> ObjCMetaClassName(getMetaclassSymbolPrefix());
  llvm37::SmallString<64> ObjCClassName(getClassSymbolPrefix());
  llvm37::SmallString<64> TClassName;

  llvm37::GlobalVariable *SuperClassGV, *IsAGV;

  // Build the flags for the metaclass.
  bool classIsHidden =
    ID->getClassInterface()->getVisibility() == HiddenVisibility;
  if (classIsHidden)
    flags |= NonFragileABI_Class_Hidden;

  // FIXME: why is this flag set on the metaclass?
  // ObjC metaclasses have no fields and don't really get constructed.
  if (ID->hasNonZeroConstructors() || ID->hasDestructors()) {
    flags |= NonFragileABI_Class_HasCXXStructors;
    if (!ID->hasNonZeroConstructors())
      flags |= NonFragileABI_Class_HasCXXDestructorOnly;  
  }

  if (!ID->getClassInterface()->getSuperClass()) {
    // class is root
    flags |= NonFragileABI_Class_Root;
    TClassName = ObjCClassName;
    TClassName += ClassName;
    SuperClassGV = GetClassGlobal(TClassName.str(),
                                  ID->getClassInterface()->isWeakImported());
    TClassName = ObjCMetaClassName;
    TClassName += ClassName;
    IsAGV = GetClassGlobal(TClassName.str(),
                           ID->getClassInterface()->isWeakImported());
  } else {
    // Has a root. Current class is not a root.
    const ObjCInterfaceDecl *Root = ID->getClassInterface();
    while (const ObjCInterfaceDecl *Super = Root->getSuperClass())
      Root = Super;
    TClassName = ObjCMetaClassName ;
    TClassName += Root->getObjCRuntimeNameAsString();
    IsAGV = GetClassGlobal(TClassName.str(),
                           Root->isWeakImported());

    // work on super class metadata symbol.
    TClassName = ObjCMetaClassName;
    TClassName += ID->getClassInterface()->getSuperClass()->getObjCRuntimeNameAsString();
    SuperClassGV = GetClassGlobal(
                                  TClassName.str(),
                                  ID->getClassInterface()->getSuperClass()->isWeakImported());
  }
  llvm37::GlobalVariable *CLASS_RO_GV = BuildClassRoTInitializer(flags,
                                                               InstanceStart,
                                                               InstanceSize,ID);
  TClassName = ObjCMetaClassName;
  TClassName += ClassName;
  llvm37::GlobalVariable *MetaTClass = BuildClassMetaData(
      TClassName.str(), IsAGV, SuperClassGV, CLASS_RO_GV, classIsHidden,
      ID->getClassInterface()->isWeakImported());
  DefinedMetaClasses.push_back(MetaTClass);

  // Metadata for the class
  flags = 0;
  if (classIsHidden)
    flags |= NonFragileABI_Class_Hidden;

  if (ID->hasNonZeroConstructors() || ID->hasDestructors()) {
    flags |= NonFragileABI_Class_HasCXXStructors;

    // Set a flag to enable a runtime optimization when a class has
    // fields that require destruction but which don't require
    // anything except zero-initialization during construction.  This
    // is most notably true of __strong and __weak types, but you can
    // also imagine there being C++ types with non-trivial default
    // constructors that merely set all fields to null.
    if (!ID->hasNonZeroConstructors())
      flags |= NonFragileABI_Class_HasCXXDestructorOnly;
  }

  if (hasObjCExceptionAttribute(CGM.getContext(), ID->getClassInterface()))
    flags |= NonFragileABI_Class_Exception;

  if (!ID->getClassInterface()->getSuperClass()) {
    flags |= NonFragileABI_Class_Root;
    SuperClassGV = nullptr;
  } else {
    // Has a root. Current class is not a root.
    TClassName = ObjCClassName;
    TClassName += ID->getClassInterface()->getSuperClass()->getObjCRuntimeNameAsString();
    SuperClassGV = GetClassGlobal(
                                  TClassName.str(),
                                  ID->getClassInterface()->getSuperClass()->isWeakImported());
  }
  GetClassSizeInfo(ID, InstanceStart, InstanceSize);
  CLASS_RO_GV = BuildClassRoTInitializer(flags,
                                         InstanceStart,
                                         InstanceSize,
                                         ID);

  TClassName = ObjCClassName;
  TClassName += ClassName;
  llvm37::GlobalVariable *ClassMD =
    BuildClassMetaData(TClassName.str(), MetaTClass, SuperClassGV, CLASS_RO_GV,
                       classIsHidden,
                       ID->getClassInterface()->isWeakImported());
  DefinedClasses.push_back(ClassMD);
  ImplementedClasses.push_back(ID->getClassInterface());

  // Determine if this class is also "non-lazy".
  if (ImplementationIsNonLazy(ID))
    DefinedNonLazyClasses.push_back(ClassMD);

  // Force the definition of the EHType if necessary.
  if (flags & NonFragileABI_Class_Exception)
    GetInterfaceEHType(ID->getClassInterface(), true);
  // Make sure method definition entries are all clear for next implementation.
  MethodDefinitions.clear();
}

/// GenerateProtocolRef - This routine is called to generate code for
/// a protocol reference expression; as in:
/// @code
///   @protocol(Proto1);
/// @endcode
/// It generates a weak reference to l_OBJC_PROTOCOL_REFERENCE_$_Proto1
/// which will hold address of the protocol meta-data.
///
llvm37::Value *CGObjCNonFragileABIMac::GenerateProtocolRef(CodeGenFunction &CGF,
                                                         const ObjCProtocolDecl *PD) {

  // This routine is called for @protocol only. So, we must build definition
  // of protocol's meta-data (not a reference to it!)
  //
  llvm37::Constant *Init =
    llvm37::ConstantExpr::getBitCast(GetOrEmitProtocol(PD),
                                   ObjCTypes.getExternalProtocolPtrTy());

  std::string ProtocolName("\01l_OBJC_PROTOCOL_REFERENCE_$_");
  ProtocolName += PD->getObjCRuntimeNameAsString();

  llvm37::GlobalVariable *PTGV = CGM.getModule().getGlobalVariable(ProtocolName);
  if (PTGV)
    return CGF.Builder.CreateLoad(PTGV);
  PTGV = new llvm37::GlobalVariable(
    CGM.getModule(),
    Init->getType(), false,
    llvm37::GlobalValue::WeakAnyLinkage,
    Init,
    ProtocolName);
  PTGV->setSection("__DATA, __objc_protorefs, coalesced, no_dead_strip");
  PTGV->setVisibility(llvm37::GlobalValue::HiddenVisibility);
  CGM.addCompilerUsedGlobal(PTGV);
  return CGF.Builder.CreateLoad(PTGV);
}

/// GenerateCategory - Build metadata for a category implementation.
/// struct _category_t {
///   const char * const name;
///   struct _class_t *const cls;
///   const struct _method_list_t * const instance_methods;
///   const struct _method_list_t * const class_methods;
///   const struct _protocol_list_t * const protocols;
///   const struct _prop_list_t * const properties;
/// }
///
void CGObjCNonFragileABIMac::GenerateCategory(const ObjCCategoryImplDecl *OCD) {
  const ObjCInterfaceDecl *Interface = OCD->getClassInterface();
  const char *Prefix = "\01l_OBJC_$_CATEGORY_";
    
  llvm37::SmallString<64> ExtCatName(Prefix);
  ExtCatName += Interface->getObjCRuntimeNameAsString();
  ExtCatName += "_$_";
  ExtCatName += OCD->getNameAsString();
    
  llvm37::SmallString<64> ExtClassName(getClassSymbolPrefix());
  ExtClassName += Interface->getObjCRuntimeNameAsString();

  llvm37::Constant *Values[6];
  Values[0] = GetClassName(OCD->getIdentifier()->getName());
  // meta-class entry symbol
  llvm37::GlobalVariable *ClassGV =
      GetClassGlobal(ExtClassName.str(), Interface->isWeakImported());

  Values[1] = ClassGV;
  std::vector<llvm37::Constant*> Methods;
  llvm37::SmallString<64> MethodListName(Prefix);
    
  MethodListName += "INSTANCE_METHODS_";
  MethodListName += Interface->getObjCRuntimeNameAsString();
  MethodListName += "_$_";
  MethodListName += OCD->getName();

  for (const auto *I : OCD->instance_methods())
    // Instance methods should always be defined.
    Methods.push_back(GetMethodConstant(I));

  Values[2] = EmitMethodList(MethodListName.str(),
                             "__DATA, __objc_const",
                             Methods);

  MethodListName = Prefix;
  MethodListName += "CLASS_METHODS_";
  MethodListName += Interface->getObjCRuntimeNameAsString();
  MethodListName += "_$_";
  MethodListName += OCD->getNameAsString();
    
  Methods.clear();
  for (const auto *I : OCD->class_methods())
    // Class methods should always be defined.
    Methods.push_back(GetMethodConstant(I));

  Values[3] = EmitMethodList(MethodListName.str(),
                             "__DATA, __objc_const",
                             Methods);
  const ObjCCategoryDecl *Category =
    Interface->FindCategoryDeclaration(OCD->getIdentifier());
  if (Category) {
    SmallString<256> ExtName;
    llvm37::raw_svector_ostream(ExtName) << Interface->getObjCRuntimeNameAsString() << "_$_"
                                       << OCD->getName();
    Values[4] = EmitProtocolList("\01l_OBJC_CATEGORY_PROTOCOLS_$_"
                                   + Interface->getObjCRuntimeNameAsString() + "_$_"
                                   + Category->getName(),
                                   Category->protocol_begin(),
                                   Category->protocol_end());
    Values[5] = EmitPropertyList("\01l_OBJC_$_PROP_LIST_" + ExtName.str(),
                                 OCD, Category, ObjCTypes);
  } else {
    Values[4] = llvm37::Constant::getNullValue(ObjCTypes.ProtocolListnfABIPtrTy);
    Values[5] = llvm37::Constant::getNullValue(ObjCTypes.PropertyListPtrTy);
  }

  llvm37::Constant *Init =
    llvm37::ConstantStruct::get(ObjCTypes.CategorynfABITy,
                              Values);
  llvm37::GlobalVariable *GCATV
    = new llvm37::GlobalVariable(CGM.getModule(), ObjCTypes.CategorynfABITy,
                               false,
                               llvm37::GlobalValue::PrivateLinkage,
                               Init,
                               ExtCatName.str());
  GCATV->setAlignment(
    CGM.getDataLayout().getABITypeAlignment(ObjCTypes.CategorynfABITy));
  GCATV->setSection("__DATA, __objc_const");
  CGM.addCompilerUsedGlobal(GCATV);
  DefinedCategories.push_back(GCATV);

  // Determine if this category is also "non-lazy".
  if (ImplementationIsNonLazy(OCD))
    DefinedNonLazyCategories.push_back(GCATV);
  // method definition entries must be clear for next implementation.
  MethodDefinitions.clear();
}

/// GetMethodConstant - Return a struct objc_method constant for the
/// given method if it has been defined. The result is null if the
/// method has not been defined. The return value has type MethodPtrTy.
llvm37::Constant *CGObjCNonFragileABIMac::GetMethodConstant(
  const ObjCMethodDecl *MD) {
  llvm37::Function *Fn = GetMethodDefinition(MD);
  if (!Fn)
    return nullptr;

  llvm37::Constant *Method[] = {
    llvm37::ConstantExpr::getBitCast(GetMethodVarName(MD->getSelector()),
                                   ObjCTypes.SelectorPtrTy),
    GetMethodVarType(MD),
    llvm37::ConstantExpr::getBitCast(Fn, ObjCTypes.Int8PtrTy)
  };
  return llvm37::ConstantStruct::get(ObjCTypes.MethodTy, Method);
}

/// EmitMethodList - Build meta-data for method declarations
/// struct _method_list_t {
///   uint32_t entsize;  // sizeof(struct _objc_method)
///   uint32_t method_count;
///   struct _objc_method method_list[method_count];
/// }
///
llvm37::Constant *
CGObjCNonFragileABIMac::EmitMethodList(Twine Name,
                                       const char *Section,
                                       ArrayRef<llvm37::Constant*> Methods) {
  // Return null for empty list.
  if (Methods.empty())
    return llvm37::Constant::getNullValue(ObjCTypes.MethodListnfABIPtrTy);

  llvm37::Constant *Values[3];
  // sizeof(struct _objc_method)
  unsigned Size = CGM.getDataLayout().getTypeAllocSize(ObjCTypes.MethodTy);
  Values[0] = llvm37::ConstantInt::get(ObjCTypes.IntTy, Size);
  // method_count
  Values[1] = llvm37::ConstantInt::get(ObjCTypes.IntTy, Methods.size());
  llvm37::ArrayType *AT = llvm37::ArrayType::get(ObjCTypes.MethodTy,
                                             Methods.size());
  Values[2] = llvm37::ConstantArray::get(AT, Methods);
  llvm37::Constant *Init = llvm37::ConstantStruct::getAnon(Values);

  llvm37::GlobalVariable *GV =
    new llvm37::GlobalVariable(CGM.getModule(), Init->getType(), false,
                             llvm37::GlobalValue::PrivateLinkage, Init, Name);
  GV->setAlignment(CGM.getDataLayout().getABITypeAlignment(Init->getType()));
  GV->setSection(Section);
  CGM.addCompilerUsedGlobal(GV);
  return llvm37::ConstantExpr::getBitCast(GV, ObjCTypes.MethodListnfABIPtrTy);
}

/// ObjCIvarOffsetVariable - Returns the ivar offset variable for
/// the given ivar.
llvm37::GlobalVariable *
CGObjCNonFragileABIMac::ObjCIvarOffsetVariable(const ObjCInterfaceDecl *ID,
                                               const ObjCIvarDecl *Ivar) {
    
  const ObjCInterfaceDecl *Container = Ivar->getContainingInterface();
  llvm37::SmallString<64> Name("OBJC_IVAR_$_");
  Name += Container->getObjCRuntimeNameAsString();
  Name += ".";
  Name += Ivar->getName();
  llvm37::GlobalVariable *IvarOffsetGV =
    CGM.getModule().getGlobalVariable(Name);
  if (!IvarOffsetGV)
    IvarOffsetGV = new llvm37::GlobalVariable(
      CGM.getModule(), ObjCTypes.IvarOffsetVarTy, false,
      llvm37::GlobalValue::ExternalLinkage, nullptr, Name.str());
  return IvarOffsetGV;
}

llvm37::Constant *
CGObjCNonFragileABIMac::EmitIvarOffsetVar(const ObjCInterfaceDecl *ID,
                                          const ObjCIvarDecl *Ivar,
                                          unsigned long int Offset) {
  llvm37::GlobalVariable *IvarOffsetGV = ObjCIvarOffsetVariable(ID, Ivar);
  IvarOffsetGV->setInitializer(
      llvm37::ConstantInt::get(ObjCTypes.IvarOffsetVarTy, Offset));
  IvarOffsetGV->setAlignment(
      CGM.getDataLayout().getABITypeAlignment(ObjCTypes.IvarOffsetVarTy));

  // FIXME: This matches gcc, but shouldn't the visibility be set on the use as
  // well (i.e., in ObjCIvarOffsetVariable).
  if (Ivar->getAccessControl() == ObjCIvarDecl::Private ||
      Ivar->getAccessControl() == ObjCIvarDecl::Package ||
      ID->getVisibility() == HiddenVisibility)
    IvarOffsetGV->setVisibility(llvm37::GlobalValue::HiddenVisibility);
  else
    IvarOffsetGV->setVisibility(llvm37::GlobalValue::DefaultVisibility);
  IvarOffsetGV->setSection("__DATA, __objc_ivar");
  return IvarOffsetGV;
}

/// EmitIvarList - Emit the ivar list for the given
/// implementation. The return value has type
/// IvarListnfABIPtrTy.
///  struct _ivar_t {
///   unsigned [long] int *offset;  // pointer to ivar offset location
///   char *name;
///   char *type;
///   uint32_t alignment;
///   uint32_t size;
/// }
/// struct _ivar_list_t {
///   uint32 entsize;  // sizeof(struct _ivar_t)
///   uint32 count;
///   struct _iver_t list[count];
/// }
///

llvm37::Constant *CGObjCNonFragileABIMac::EmitIvarList(
  const ObjCImplementationDecl *ID) {

  std::vector<llvm37::Constant*> Ivars;

  const ObjCInterfaceDecl *OID = ID->getClassInterface();
  assert(OID && "CGObjCNonFragileABIMac::EmitIvarList - null interface");

  // FIXME. Consolidate this with similar code in GenerateClass.

  for (const ObjCIvarDecl *IVD = OID->all_declared_ivar_begin(); 
       IVD; IVD = IVD->getNextIvar()) {
    // Ignore unnamed bit-fields.
    if (!IVD->getDeclName())
      continue;
    llvm37::Constant *Ivar[5];
    Ivar[0] = EmitIvarOffsetVar(ID->getClassInterface(), IVD,
                                ComputeIvarBaseOffset(CGM, ID, IVD));
    Ivar[1] = GetMethodVarName(IVD->getIdentifier());
    Ivar[2] = GetMethodVarType(IVD);
    llvm37::Type *FieldTy =
      CGM.getTypes().ConvertTypeForMem(IVD->getType());
    unsigned Size = CGM.getDataLayout().getTypeAllocSize(FieldTy);
    unsigned Align = CGM.getContext().getPreferredTypeAlign(
      IVD->getType().getTypePtr()) >> 3;
    Align = llvm37::Log2_32(Align);
    Ivar[3] = llvm37::ConstantInt::get(ObjCTypes.IntTy, Align);
    // NOTE. Size of a bitfield does not match gcc's, because of the
    // way bitfields are treated special in each. But I am told that
    // 'size' for bitfield ivars is ignored by the runtime so it does
    // not matter.  If it matters, there is enough info to get the
    // bitfield right!
    Ivar[4] = llvm37::ConstantInt::get(ObjCTypes.IntTy, Size);
    Ivars.push_back(llvm37::ConstantStruct::get(ObjCTypes.IvarnfABITy, Ivar));
  }
  // Return null for empty list.
  if (Ivars.empty())
    return llvm37::Constant::getNullValue(ObjCTypes.IvarListnfABIPtrTy);

  llvm37::Constant *Values[3];
  unsigned Size = CGM.getDataLayout().getTypeAllocSize(ObjCTypes.IvarnfABITy);
  Values[0] = llvm37::ConstantInt::get(ObjCTypes.IntTy, Size);
  Values[1] = llvm37::ConstantInt::get(ObjCTypes.IntTy, Ivars.size());
  llvm37::ArrayType *AT = llvm37::ArrayType::get(ObjCTypes.IvarnfABITy,
                                             Ivars.size());
  Values[2] = llvm37::ConstantArray::get(AT, Ivars);
  llvm37::Constant *Init = llvm37::ConstantStruct::getAnon(Values);
  const char *Prefix = "\01l_OBJC_$_INSTANCE_VARIABLES_";
  llvm37::GlobalVariable *GV =
    new llvm37::GlobalVariable(CGM.getModule(), Init->getType(), false,
                             llvm37::GlobalValue::PrivateLinkage,
                             Init,
                             Prefix + OID->getObjCRuntimeNameAsString());
  GV->setAlignment(
    CGM.getDataLayout().getABITypeAlignment(Init->getType()));
  GV->setSection("__DATA, __objc_const");

  CGM.addCompilerUsedGlobal(GV);
  return llvm37::ConstantExpr::getBitCast(GV, ObjCTypes.IvarListnfABIPtrTy);
}

llvm37::Constant *CGObjCNonFragileABIMac::GetOrEmitProtocolRef(
  const ObjCProtocolDecl *PD) {
  llvm37::GlobalVariable *&Entry = Protocols[PD->getIdentifier()];

  if (!Entry) {
    // We use the initializer as a marker of whether this is a forward
    // reference or not. At module finalization we add the empty
    // contents for protocols which were referenced but never defined.
    Entry =
        new llvm37::GlobalVariable(CGM.getModule(), ObjCTypes.ProtocolnfABITy,
                                 false, llvm37::GlobalValue::ExternalLinkage,
                                 nullptr,
                                 "\01l_OBJC_PROTOCOL_$_" + PD->getObjCRuntimeNameAsString());
    Entry->setSection("__DATA,__datacoal_nt,coalesced");
  }

  return Entry;
}

/// GetOrEmitProtocol - Generate the protocol meta-data:
/// @code
/// struct _protocol_t {
///   id isa;  // NULL
///   const char * const protocol_name;
///   const struct _protocol_list_t * protocol_list; // super protocols
///   const struct method_list_t * const instance_methods;
///   const struct method_list_t * const class_methods;
///   const struct method_list_t *optionalInstanceMethods;
///   const struct method_list_t *optionalClassMethods;
///   const struct _prop_list_t * properties;
///   const uint32_t size;  // sizeof(struct _protocol_t)
///   const uint32_t flags;  // = 0
///   const char ** extendedMethodTypes;
///   const char *demangledName;
/// }
/// @endcode
///

llvm37::Constant *CGObjCNonFragileABIMac::GetOrEmitProtocol(
  const ObjCProtocolDecl *PD) {
  llvm37::GlobalVariable *Entry = Protocols[PD->getIdentifier()];

  // Early exit if a defining object has already been generated.
  if (Entry && Entry->hasInitializer())
    return Entry;

  // Use the protocol definition, if there is one.
  if (const ObjCProtocolDecl *Def = PD->getDefinition())
    PD = Def;
  
  // Construct method lists.
  std::vector<llvm37::Constant*> InstanceMethods, ClassMethods;
  std::vector<llvm37::Constant*> OptInstanceMethods, OptClassMethods;
  std::vector<llvm37::Constant*> MethodTypesExt, OptMethodTypesExt;
  for (const auto *MD : PD->instance_methods()) {
    llvm37::Constant *C = GetMethodDescriptionConstant(MD);
    if (!C)
      return GetOrEmitProtocolRef(PD);
    
    if (MD->getImplementationControl() == ObjCMethodDecl::Optional) {
      OptInstanceMethods.push_back(C);
      OptMethodTypesExt.push_back(GetMethodVarType(MD, true));
    } else {
      InstanceMethods.push_back(C);
      MethodTypesExt.push_back(GetMethodVarType(MD, true));
    }
  }

  for (const auto *MD : PD->class_methods()) {
    llvm37::Constant *C = GetMethodDescriptionConstant(MD);
    if (!C)
      return GetOrEmitProtocolRef(PD);

    if (MD->getImplementationControl() == ObjCMethodDecl::Optional) {
      OptClassMethods.push_back(C);
      OptMethodTypesExt.push_back(GetMethodVarType(MD, true));
    } else {
      ClassMethods.push_back(C);
      MethodTypesExt.push_back(GetMethodVarType(MD, true));
    }
  }

  MethodTypesExt.insert(MethodTypesExt.end(),
                        OptMethodTypesExt.begin(), OptMethodTypesExt.end());

  llvm37::Constant *Values[12];
  // isa is NULL
  Values[0] = llvm37::Constant::getNullValue(ObjCTypes.ObjectPtrTy);
  Values[1] = GetClassName(PD->getObjCRuntimeNameAsString());
  Values[2] = EmitProtocolList("\01l_OBJC_$_PROTOCOL_REFS_" + PD->getObjCRuntimeNameAsString(),
                               PD->protocol_begin(),
                               PD->protocol_end());

  Values[3] = EmitMethodList("\01l_OBJC_$_PROTOCOL_INSTANCE_METHODS_"
                             + PD->getObjCRuntimeNameAsString(),
                             "__DATA, __objc_const",
                             InstanceMethods);
  Values[4] = EmitMethodList("\01l_OBJC_$_PROTOCOL_CLASS_METHODS_"
                             + PD->getObjCRuntimeNameAsString(),
                             "__DATA, __objc_const",
                             ClassMethods);
  Values[5] = EmitMethodList("\01l_OBJC_$_PROTOCOL_INSTANCE_METHODS_OPT_"
                             + PD->getObjCRuntimeNameAsString(),
                             "__DATA, __objc_const",
                             OptInstanceMethods);
  Values[6] = EmitMethodList("\01l_OBJC_$_PROTOCOL_CLASS_METHODS_OPT_"
                             + PD->getObjCRuntimeNameAsString(),
                             "__DATA, __objc_const",
                             OptClassMethods);
  Values[7] = EmitPropertyList("\01l_OBJC_$_PROP_LIST_" + PD->getObjCRuntimeNameAsString(),
                               nullptr, PD, ObjCTypes);
  uint32_t Size =
    CGM.getDataLayout().getTypeAllocSize(ObjCTypes.ProtocolnfABITy);
  Values[8] = llvm37::ConstantInt::get(ObjCTypes.IntTy, Size);
  Values[9] = llvm37::Constant::getNullValue(ObjCTypes.IntTy);
  Values[10] = EmitProtocolMethodTypes("\01l_OBJC_$_PROTOCOL_METHOD_TYPES_"
                                       + PD->getObjCRuntimeNameAsString(),
                                       MethodTypesExt, ObjCTypes);
  // const char *demangledName;
  Values[11] = llvm37::Constant::getNullValue(ObjCTypes.Int8PtrTy);
    
  llvm37::Constant *Init = llvm37::ConstantStruct::get(ObjCTypes.ProtocolnfABITy,
                                                   Values);

  if (Entry) {
    // Already created, fix the linkage and update the initializer.
    Entry->setLinkage(llvm37::GlobalValue::WeakAnyLinkage);
    Entry->setInitializer(Init);
  } else {
    Entry =
      new llvm37::GlobalVariable(CGM.getModule(), ObjCTypes.ProtocolnfABITy,
                               false, llvm37::GlobalValue::WeakAnyLinkage, Init,
                               "\01l_OBJC_PROTOCOL_$_" + PD->getObjCRuntimeNameAsString());
    Entry->setAlignment(
      CGM.getDataLayout().getABITypeAlignment(ObjCTypes.ProtocolnfABITy));
    Entry->setSection("__DATA,__datacoal_nt,coalesced");

    Protocols[PD->getIdentifier()] = Entry;
  }
  Entry->setVisibility(llvm37::GlobalValue::HiddenVisibility);
  CGM.addCompilerUsedGlobal(Entry);

  // Use this protocol meta-data to build protocol list table in section
  // __DATA, __objc_protolist
  llvm37::GlobalVariable *PTGV =
    new llvm37::GlobalVariable(CGM.getModule(), ObjCTypes.ProtocolnfABIPtrTy,
                             false, llvm37::GlobalValue::WeakAnyLinkage, Entry,
                             "\01l_OBJC_LABEL_PROTOCOL_$_" + PD->getObjCRuntimeNameAsString());
  PTGV->setAlignment(
    CGM.getDataLayout().getABITypeAlignment(ObjCTypes.ProtocolnfABIPtrTy));
  PTGV->setSection("__DATA, __objc_protolist, coalesced, no_dead_strip");
  PTGV->setVisibility(llvm37::GlobalValue::HiddenVisibility);
  CGM.addCompilerUsedGlobal(PTGV);
  return Entry;
}

/// EmitProtocolList - Generate protocol list meta-data:
/// @code
/// struct _protocol_list_t {
///   long protocol_count;   // Note, this is 32/64 bit
///   struct _protocol_t[protocol_count];
/// }
/// @endcode
///
llvm37::Constant *
CGObjCNonFragileABIMac::EmitProtocolList(Twine Name,
                                      ObjCProtocolDecl::protocol_iterator begin,
                                      ObjCProtocolDecl::protocol_iterator end) {
  SmallVector<llvm37::Constant *, 16> ProtocolRefs;

  // Just return null for empty protocol lists
  if (begin == end)
    return llvm37::Constant::getNullValue(ObjCTypes.ProtocolListnfABIPtrTy);

  // FIXME: We shouldn't need to do this lookup here, should we?
  SmallString<256> TmpName;
  Name.toVector(TmpName);
  llvm37::GlobalVariable *GV =
    CGM.getModule().getGlobalVariable(TmpName.str(), true);
  if (GV)
    return llvm37::ConstantExpr::getBitCast(GV, ObjCTypes.ProtocolListnfABIPtrTy);

  for (; begin != end; ++begin)
    ProtocolRefs.push_back(GetProtocolRef(*begin));  // Implemented???

  // This list is null terminated.
  ProtocolRefs.push_back(llvm37::Constant::getNullValue(
                           ObjCTypes.ProtocolnfABIPtrTy));

  llvm37::Constant *Values[2];
  Values[0] =
    llvm37::ConstantInt::get(ObjCTypes.LongTy, ProtocolRefs.size() - 1);
  Values[1] =
    llvm37::ConstantArray::get(llvm37::ArrayType::get(ObjCTypes.ProtocolnfABIPtrTy,
                                                  ProtocolRefs.size()),
                             ProtocolRefs);

  llvm37::Constant *Init = llvm37::ConstantStruct::getAnon(Values);
  GV = new llvm37::GlobalVariable(CGM.getModule(), Init->getType(), false,
                                llvm37::GlobalValue::PrivateLinkage,
                                Init, Name);
  GV->setSection("__DATA, __objc_const");
  GV->setAlignment(
    CGM.getDataLayout().getABITypeAlignment(Init->getType()));
  CGM.addCompilerUsedGlobal(GV);
  return llvm37::ConstantExpr::getBitCast(GV,
                                        ObjCTypes.ProtocolListnfABIPtrTy);
}

/// GetMethodDescriptionConstant - This routine build following meta-data:
/// struct _objc_method {
///   SEL _cmd;
///   char *method_type;
///   char *_imp;
/// }

llvm37::Constant *
CGObjCNonFragileABIMac::GetMethodDescriptionConstant(const ObjCMethodDecl *MD) {
  llvm37::Constant *Desc[3];
  Desc[0] =
    llvm37::ConstantExpr::getBitCast(GetMethodVarName(MD->getSelector()),
                                   ObjCTypes.SelectorPtrTy);
  Desc[1] = GetMethodVarType(MD);
  if (!Desc[1])
    return nullptr;

  // Protocol methods have no implementation. So, this entry is always NULL.
  Desc[2] = llvm37::Constant::getNullValue(ObjCTypes.Int8PtrTy);
  return llvm37::ConstantStruct::get(ObjCTypes.MethodTy, Desc);
}

/// EmitObjCValueForIvar - Code Gen for nonfragile ivar reference.
/// This code gen. amounts to generating code for:
/// @code
/// (type *)((char *)base + _OBJC_IVAR_$_.ivar;
/// @encode
///
LValue CGObjCNonFragileABIMac::EmitObjCValueForIvar(
                                               CodeGen::CodeGenFunction &CGF,
                                               QualType ObjectTy,
                                               llvm37::Value *BaseValue,
                                               const ObjCIvarDecl *Ivar,
                                               unsigned CVRQualifiers) {
  ObjCInterfaceDecl *ID = ObjectTy->getAs<ObjCObjectType>()->getInterface();
  llvm37::Value *Offset = EmitIvarOffset(CGF, ID, Ivar);
  return EmitValueForIvarAtOffset(CGF, ID, BaseValue, Ivar, CVRQualifiers,
                                  Offset);
}

llvm37::Value *CGObjCNonFragileABIMac::EmitIvarOffset(
  CodeGen::CodeGenFunction &CGF,
  const ObjCInterfaceDecl *Interface,
  const ObjCIvarDecl *Ivar) {
  llvm37::Value *IvarOffsetValue = ObjCIvarOffsetVariable(Interface, Ivar);
  IvarOffsetValue = CGF.Builder.CreateLoad(IvarOffsetValue, "ivar");
  if (IsIvarOffsetKnownIdempotent(CGF, Ivar))
    cast<llvm37::LoadInst>(IvarOffsetValue)
        ->setMetadata(CGM.getModule().getMDKindID("invariant.load"),
                      llvm37::MDNode::get(VMContext, None));

  // This could be 32bit int or 64bit integer depending on the architecture.
  // Cast it to 64bit integer value, if it is a 32bit integer ivar offset value
  //  as this is what caller always expectes.
  if (ObjCTypes.IvarOffsetVarTy == ObjCTypes.IntTy)
    IvarOffsetValue = CGF.Builder.CreateIntCast(
        IvarOffsetValue, ObjCTypes.LongTy, true, "ivar.conv");
  return IvarOffsetValue;
}

static void appendSelectorForMessageRefTable(std::string &buffer,
                                             Selector selector) {
  if (selector.isUnarySelector()) {
    buffer += selector.getNameForSlot(0);
    return;
  }

  for (unsigned i = 0, e = selector.getNumArgs(); i != e; ++i) {
    buffer += selector.getNameForSlot(i);
    buffer += '_';
  }
}

/// Emit a "v-table" message send.  We emit a weak hidden-visibility
/// struct, initially containing the selector pointer and a pointer to
/// a "fixup" variant of the appropriate objc_msgSend.  To call, we
/// load and call the function pointer, passing the address of the
/// struct as the second parameter.  The runtime determines whether
/// the selector is currently emitted using vtable dispatch; if so, it
/// substitutes a stub function which simply tail-calls through the
/// appropriate vtable slot, and if not, it substitues a stub function
/// which tail-calls objc_msgSend.  Both stubs adjust the selector
/// argument to correctly point to the selector.
RValue
CGObjCNonFragileABIMac::EmitVTableMessageSend(CodeGenFunction &CGF,
                                              ReturnValueSlot returnSlot,
                                              QualType resultType,
                                              Selector selector,
                                              llvm37::Value *arg0,
                                              QualType arg0Type,
                                              bool isSuper,
                                              const CallArgList &formalArgs,
                                              const ObjCMethodDecl *method) {
  // Compute the actual arguments.
  CallArgList args;

  // First argument: the receiver / super-call structure.
  if (!isSuper)
    arg0 = CGF.Builder.CreateBitCast(arg0, ObjCTypes.ObjectPtrTy);
  args.add(RValue::get(arg0), arg0Type);

  // Second argument: a pointer to the message ref structure.  Leave
  // the actual argument value blank for now.
  args.add(RValue::get(nullptr), ObjCTypes.MessageRefCPtrTy);

  args.insert(args.end(), formalArgs.begin(), formalArgs.end());

  MessageSendInfo MSI = getMessageSendInfo(method, resultType, args);

  NullReturnState nullReturn;

  // Find the function to call and the mangled name for the message
  // ref structure.  Using a different mangled name wouldn't actually
  // be a problem; it would just be a waste.
  //
  // The runtime currently never uses vtable dispatch for anything
  // except normal, non-super message-sends.
  // FIXME: don't use this for that.
  llvm37::Constant *fn = nullptr;
  std::string messageRefName("\01l_");
  if (CGM.ReturnSlotInterferesWithArgs(MSI.CallInfo)) {
    if (isSuper) {
      fn = ObjCTypes.getMessageSendSuper2StretFixupFn();
      messageRefName += "objc_msgSendSuper2_stret_fixup";
    } else {
      nullReturn.init(CGF, arg0);
      fn = ObjCTypes.getMessageSendStretFixupFn();
      messageRefName += "objc_msgSend_stret_fixup";
    }
  } else if (!isSuper && CGM.ReturnTypeUsesFPRet(resultType)) {
    fn = ObjCTypes.getMessageSendFpretFixupFn();
    messageRefName += "objc_msgSend_fpret_fixup";
  } else {
    if (isSuper) {
      fn = ObjCTypes.getMessageSendSuper2FixupFn();
      messageRefName += "objc_msgSendSuper2_fixup";
    } else {
      fn = ObjCTypes.getMessageSendFixupFn();
      messageRefName += "objc_msgSend_fixup";
    }
  }
  assert(fn && "CGObjCNonFragileABIMac::EmitMessageSend");
  messageRefName += '_';

  // Append the selector name, except use underscores anywhere we
  // would have used colons.
  appendSelectorForMessageRefTable(messageRefName, selector);

  llvm37::GlobalVariable *messageRef
    = CGM.getModule().getGlobalVariable(messageRefName);
  if (!messageRef) {
    // Build the message ref structure.
    llvm37::Constant *values[] = { fn, GetMethodVarName(selector) };
    llvm37::Constant *init = llvm37::ConstantStruct::getAnon(values);
    messageRef = new llvm37::GlobalVariable(CGM.getModule(),
                                          init->getType(),
                                          /*constant*/ false,
                                          llvm37::GlobalValue::WeakAnyLinkage,
                                          init,
                                          messageRefName);
    messageRef->setVisibility(llvm37::GlobalValue::HiddenVisibility);
    messageRef->setAlignment(16);
    messageRef->setSection("__DATA, __objc_msgrefs, coalesced");
  }
  
  bool requiresnullCheck = false;
  if (CGM.getLangOpts().ObjCAutoRefCount && method)
    for (const auto *ParamDecl : method->params()) {
      if (ParamDecl->hasAttr<NSConsumedAttr>()) {
        if (!nullReturn.NullBB)
          nullReturn.init(CGF, arg0);
        requiresnullCheck = true;
        break;
      }
    }
  
  llvm37::Value *mref =
    CGF.Builder.CreateBitCast(messageRef, ObjCTypes.MessageRefPtrTy);

  // Update the message ref argument.
  args[1].RV = RValue::get(mref);

  // Load the function to call from the message ref table.
  llvm37::Value *callee =
      CGF.Builder.CreateStructGEP(ObjCTypes.MessageRefTy, mref, 0);
  callee = CGF.Builder.CreateLoad(callee, "msgSend_fn");

  callee = CGF.Builder.CreateBitCast(callee, MSI.MessengerType);

  RValue result = CGF.EmitCall(MSI.CallInfo, callee, returnSlot, args);
  return nullReturn.complete(CGF, result, resultType, formalArgs,
                             requiresnullCheck ? method : nullptr);
}

/// Generate code for a message send expression in the nonfragile abi.
CodeGen::RValue
CGObjCNonFragileABIMac::GenerateMessageSend(CodeGen::CodeGenFunction &CGF,
                                            ReturnValueSlot Return,
                                            QualType ResultType,
                                            Selector Sel,
                                            llvm37::Value *Receiver,
                                            const CallArgList &CallArgs,
                                            const ObjCInterfaceDecl *Class,
                                            const ObjCMethodDecl *Method) {
  return isVTableDispatchedSelector(Sel)
    ? EmitVTableMessageSend(CGF, Return, ResultType, Sel,
                            Receiver, CGF.getContext().getObjCIdType(),
                            false, CallArgs, Method)
    : EmitMessageSend(CGF, Return, ResultType,
                      EmitSelector(CGF, Sel),
                      Receiver, CGF.getContext().getObjCIdType(),
                      false, CallArgs, Method, ObjCTypes);
}

llvm37::GlobalVariable *
CGObjCNonFragileABIMac::GetClassGlobal(const std::string &Name, bool Weak) {
  llvm37::GlobalValue::LinkageTypes L =
      Weak ? llvm37::GlobalValue::ExternalWeakLinkage
           : llvm37::GlobalValue::ExternalLinkage;

  llvm37::GlobalVariable *GV = CGM.getModule().getGlobalVariable(Name);

  if (!GV)
    GV = new llvm37::GlobalVariable(CGM.getModule(), ObjCTypes.ClassnfABITy,
                                  false, L, nullptr, Name);

  assert(GV->getLinkage() == L);
  return GV;
}

llvm37::Value *CGObjCNonFragileABIMac::EmitClassRefFromId(CodeGenFunction &CGF,
                                                        IdentifierInfo *II,
                                                        bool Weak,
                                                        const ObjCInterfaceDecl *ID) {
  llvm37::GlobalVariable *&Entry = ClassReferences[II];
  
  if (!Entry) {
    std::string ClassName(
      getClassSymbolPrefix() +
      (ID ? ID->getObjCRuntimeNameAsString() : II->getName()).str());
    llvm37::GlobalVariable *ClassGV = GetClassGlobal(ClassName, Weak);
    Entry = new llvm37::GlobalVariable(CGM.getModule(), ObjCTypes.ClassnfABIPtrTy,
                                     false, llvm37::GlobalValue::PrivateLinkage,
                                     ClassGV, "OBJC_CLASSLIST_REFERENCES_$_");
    Entry->setAlignment(
                        CGM.getDataLayout().getABITypeAlignment(
                                                                ObjCTypes.ClassnfABIPtrTy));
    Entry->setSection("__DATA, __objc_classrefs, regular, no_dead_strip");
    CGM.addCompilerUsedGlobal(Entry);
  }
  return CGF.Builder.CreateLoad(Entry);
}

llvm37::Value *CGObjCNonFragileABIMac::EmitClassRef(CodeGenFunction &CGF,
                                                  const ObjCInterfaceDecl *ID) {
  return EmitClassRefFromId(CGF, ID->getIdentifier(), ID->isWeakImported(), ID);
}

llvm37::Value *CGObjCNonFragileABIMac::EmitNSAutoreleasePoolClassRef(
                                                    CodeGenFunction &CGF) {
  IdentifierInfo *II = &CGM.getContext().Idents.get("NSAutoreleasePool");
  return EmitClassRefFromId(CGF, II, false, 0);
}

llvm37::Value *
CGObjCNonFragileABIMac::EmitSuperClassRef(CodeGenFunction &CGF,
                                          const ObjCInterfaceDecl *ID) {
  llvm37::GlobalVariable *&Entry = SuperClassReferences[ID->getIdentifier()];

  if (!Entry) {
    llvm37::SmallString<64> ClassName(getClassSymbolPrefix());
    ClassName += ID->getObjCRuntimeNameAsString();
    llvm37::GlobalVariable *ClassGV = GetClassGlobal(ClassName.str(),
                                                   ID->isWeakImported());
    Entry = new llvm37::GlobalVariable(CGM.getModule(), ObjCTypes.ClassnfABIPtrTy,
                                     false, llvm37::GlobalValue::PrivateLinkage,
                                     ClassGV, "OBJC_CLASSLIST_SUP_REFS_$_");
    Entry->setAlignment(
      CGM.getDataLayout().getABITypeAlignment(
        ObjCTypes.ClassnfABIPtrTy));
    Entry->setSection("__DATA, __objc_superrefs, regular, no_dead_strip");
    CGM.addCompilerUsedGlobal(Entry);
  }
  return CGF.Builder.CreateLoad(Entry);
}

/// EmitMetaClassRef - Return a Value * of the address of _class_t
/// meta-data
///
llvm37::Value *CGObjCNonFragileABIMac::EmitMetaClassRef(CodeGenFunction &CGF,
                                                      const ObjCInterfaceDecl *ID,
                                                      bool Weak) {
  llvm37::GlobalVariable * &Entry = MetaClassReferences[ID->getIdentifier()];
  if (!Entry) {
    llvm37::SmallString<64> MetaClassName(getMetaclassSymbolPrefix());
    MetaClassName += ID->getObjCRuntimeNameAsString();
    llvm37::GlobalVariable *MetaClassGV =
      GetClassGlobal(MetaClassName.str(), Weak);

    Entry = new llvm37::GlobalVariable(CGM.getModule(), ObjCTypes.ClassnfABIPtrTy,
                                     false, llvm37::GlobalValue::PrivateLinkage,
                                     MetaClassGV, "OBJC_CLASSLIST_SUP_REFS_$_");
    Entry->setAlignment(
        CGM.getDataLayout().getABITypeAlignment(ObjCTypes.ClassnfABIPtrTy));

    Entry->setSection("__DATA, __objc_superrefs, regular, no_dead_strip");
    CGM.addCompilerUsedGlobal(Entry);
  }

  return CGF.Builder.CreateLoad(Entry);
}

/// GetClass - Return a reference to the class for the given interface
/// decl.
llvm37::Value *CGObjCNonFragileABIMac::GetClass(CodeGenFunction &CGF,
                                              const ObjCInterfaceDecl *ID) {
  if (ID->isWeakImported()) {
    llvm37::SmallString<64> ClassName(getClassSymbolPrefix());
    ClassName += ID->getObjCRuntimeNameAsString();
    llvm37::GlobalVariable *ClassGV = GetClassGlobal(ClassName.str(), true);
    (void)ClassGV;
    assert(ClassGV->hasExternalWeakLinkage());
  }
  
  return EmitClassRef(CGF, ID);
}

/// Generates a message send where the super is the receiver.  This is
/// a message send to self with special delivery semantics indicating
/// which class's method should be called.
CodeGen::RValue
CGObjCNonFragileABIMac::GenerateMessageSendSuper(CodeGen::CodeGenFunction &CGF,
                                                 ReturnValueSlot Return,
                                                 QualType ResultType,
                                                 Selector Sel,
                                                 const ObjCInterfaceDecl *Class,
                                                 bool isCategoryImpl,
                                                 llvm37::Value *Receiver,
                                                 bool IsClassMessage,
                                                 const CodeGen::CallArgList &CallArgs,
                                                 const ObjCMethodDecl *Method) {
  // ...
  // Create and init a super structure; this is a (receiver, class)
  // pair we will pass to objc_msgSendSuper.
  llvm37::Value *ObjCSuper =
    CGF.CreateTempAlloca(ObjCTypes.SuperTy, "objc_super");

  llvm37::Value *ReceiverAsObject =
    CGF.Builder.CreateBitCast(Receiver, ObjCTypes.ObjectPtrTy);
  CGF.Builder.CreateStore(
      ReceiverAsObject,
      CGF.Builder.CreateStructGEP(ObjCTypes.SuperTy, ObjCSuper, 0));

  // If this is a class message the metaclass is passed as the target.
  llvm37::Value *Target;
  if (IsClassMessage)
      Target = EmitMetaClassRef(CGF, Class, Class->isWeakImported());
  else
    Target = EmitSuperClassRef(CGF, Class);

  // FIXME: We shouldn't need to do this cast, rectify the ASTContext and
  // ObjCTypes types.
  llvm37::Type *ClassTy =
    CGM.getTypes().ConvertType(CGF.getContext().getObjCClassType());
  Target = CGF.Builder.CreateBitCast(Target, ClassTy);
  CGF.Builder.CreateStore(
      Target, CGF.Builder.CreateStructGEP(ObjCTypes.SuperTy, ObjCSuper, 1));

  return (isVTableDispatchedSelector(Sel))
    ? EmitVTableMessageSend(CGF, Return, ResultType, Sel,
                            ObjCSuper, ObjCTypes.SuperPtrCTy,
                            true, CallArgs, Method)
    : EmitMessageSend(CGF, Return, ResultType,
                      EmitSelector(CGF, Sel),
                      ObjCSuper, ObjCTypes.SuperPtrCTy,
                      true, CallArgs, Method, ObjCTypes);
}

llvm37::Value *CGObjCNonFragileABIMac::EmitSelector(CodeGenFunction &CGF,
                                                  Selector Sel, bool lval) {
  llvm37::GlobalVariable *&Entry = SelectorReferences[Sel];

  if (!Entry) {
    llvm37::Constant *Casted =
      llvm37::ConstantExpr::getBitCast(GetMethodVarName(Sel),
                                     ObjCTypes.SelectorPtrTy);
    Entry = new llvm37::GlobalVariable(CGM.getModule(), ObjCTypes.SelectorPtrTy,
                                     false, llvm37::GlobalValue::PrivateLinkage,
                                     Casted, "OBJC_SELECTOR_REFERENCES_");
    Entry->setExternallyInitialized(true);
    Entry->setSection("__DATA, __objc_selrefs, literal_pointers, no_dead_strip");
    CGM.addCompilerUsedGlobal(Entry);
  }

  if (lval)
    return Entry;
  llvm37::LoadInst* LI = CGF.Builder.CreateLoad(Entry);
  
  LI->setMetadata(CGM.getModule().getMDKindID("invariant.load"), 
                  llvm37::MDNode::get(VMContext, None));
  return LI;
}
/// EmitObjCIvarAssign - Code gen for assigning to a __strong object.
/// objc_assign_ivar (id src, id *dst, ptrdiff_t)
///
void CGObjCNonFragileABIMac::EmitObjCIvarAssign(CodeGen::CodeGenFunction &CGF,
                                                llvm37::Value *src,
                                                llvm37::Value *dst,
                                                llvm37::Value *ivarOffset) {
  llvm37::Type * SrcTy = src->getType();
  if (!isa<llvm37::PointerType>(SrcTy)) {
    unsigned Size = CGM.getDataLayout().getTypeAllocSize(SrcTy);
    assert(Size <= 8 && "does not support size > 8");
    src = (Size == 4 ? CGF.Builder.CreateBitCast(src, ObjCTypes.IntTy)
           : CGF.Builder.CreateBitCast(src, ObjCTypes.LongTy));
    src = CGF.Builder.CreateIntToPtr(src, ObjCTypes.Int8PtrTy);
  }
  src = CGF.Builder.CreateBitCast(src, ObjCTypes.ObjectPtrTy);
  dst = CGF.Builder.CreateBitCast(dst, ObjCTypes.PtrObjectPtrTy);
  llvm37::Value *args[] = { src, dst, ivarOffset };
  CGF.EmitNounwindRuntimeCall(ObjCTypes.getGcAssignIvarFn(), args);
}

/// EmitObjCStrongCastAssign - Code gen for assigning to a __strong cast object.
/// objc_assign_strongCast (id src, id *dst)
///
void CGObjCNonFragileABIMac::EmitObjCStrongCastAssign(
  CodeGen::CodeGenFunction &CGF,
  llvm37::Value *src, llvm37::Value *dst) {
  llvm37::Type * SrcTy = src->getType();
  if (!isa<llvm37::PointerType>(SrcTy)) {
    unsigned Size = CGM.getDataLayout().getTypeAllocSize(SrcTy);
    assert(Size <= 8 && "does not support size > 8");
    src = (Size == 4 ? CGF.Builder.CreateBitCast(src, ObjCTypes.IntTy)
           : CGF.Builder.CreateBitCast(src, ObjCTypes.LongTy));
    src = CGF.Builder.CreateIntToPtr(src, ObjCTypes.Int8PtrTy);
  }
  src = CGF.Builder.CreateBitCast(src, ObjCTypes.ObjectPtrTy);
  dst = CGF.Builder.CreateBitCast(dst, ObjCTypes.PtrObjectPtrTy);
  llvm37::Value *args[] = { src, dst };
  CGF.EmitNounwindRuntimeCall(ObjCTypes.getGcAssignStrongCastFn(),
                              args, "weakassign");
}

void CGObjCNonFragileABIMac::EmitGCMemmoveCollectable(
  CodeGen::CodeGenFunction &CGF,
  llvm37::Value *DestPtr,
  llvm37::Value *SrcPtr,
  llvm37::Value *Size) {
  SrcPtr = CGF.Builder.CreateBitCast(SrcPtr, ObjCTypes.Int8PtrTy);
  DestPtr = CGF.Builder.CreateBitCast(DestPtr, ObjCTypes.Int8PtrTy);
  llvm37::Value *args[] = { DestPtr, SrcPtr, Size };
  CGF.EmitNounwindRuntimeCall(ObjCTypes.GcMemmoveCollectableFn(), args);
}

/// EmitObjCWeakRead - Code gen for loading value of a __weak
/// object: objc_read_weak (id *src)
///
llvm37::Value * CGObjCNonFragileABIMac::EmitObjCWeakRead(
  CodeGen::CodeGenFunction &CGF,
  llvm37::Value *AddrWeakObj) {
  llvm37::Type* DestTy =
    cast<llvm37::PointerType>(AddrWeakObj->getType())->getElementType();
  AddrWeakObj = CGF.Builder.CreateBitCast(AddrWeakObj, ObjCTypes.PtrObjectPtrTy);
  llvm37::Value *read_weak =
    CGF.EmitNounwindRuntimeCall(ObjCTypes.getGcReadWeakFn(),
                                AddrWeakObj, "weakread");
  read_weak = CGF.Builder.CreateBitCast(read_weak, DestTy);
  return read_weak;
}

/// EmitObjCWeakAssign - Code gen for assigning to a __weak object.
/// objc_assign_weak (id src, id *dst)
///
void CGObjCNonFragileABIMac::EmitObjCWeakAssign(CodeGen::CodeGenFunction &CGF,
                                                llvm37::Value *src, llvm37::Value *dst) {
  llvm37::Type * SrcTy = src->getType();
  if (!isa<llvm37::PointerType>(SrcTy)) {
    unsigned Size = CGM.getDataLayout().getTypeAllocSize(SrcTy);
    assert(Size <= 8 && "does not support size > 8");
    src = (Size == 4 ? CGF.Builder.CreateBitCast(src, ObjCTypes.IntTy)
           : CGF.Builder.CreateBitCast(src, ObjCTypes.LongTy));
    src = CGF.Builder.CreateIntToPtr(src, ObjCTypes.Int8PtrTy);
  }
  src = CGF.Builder.CreateBitCast(src, ObjCTypes.ObjectPtrTy);
  dst = CGF.Builder.CreateBitCast(dst, ObjCTypes.PtrObjectPtrTy);
  llvm37::Value *args[] = { src, dst };
  CGF.EmitNounwindRuntimeCall(ObjCTypes.getGcAssignWeakFn(),
                              args, "weakassign");
}

/// EmitObjCGlobalAssign - Code gen for assigning to a __strong object.
/// objc_assign_global (id src, id *dst)
///
void CGObjCNonFragileABIMac::EmitObjCGlobalAssign(CodeGen::CodeGenFunction &CGF,
                                          llvm37::Value *src, llvm37::Value *dst,
                                          bool threadlocal) {
  llvm37::Type * SrcTy = src->getType();
  if (!isa<llvm37::PointerType>(SrcTy)) {
    unsigned Size = CGM.getDataLayout().getTypeAllocSize(SrcTy);
    assert(Size <= 8 && "does not support size > 8");
    src = (Size == 4 ? CGF.Builder.CreateBitCast(src, ObjCTypes.IntTy)
           : CGF.Builder.CreateBitCast(src, ObjCTypes.LongTy));
    src = CGF.Builder.CreateIntToPtr(src, ObjCTypes.Int8PtrTy);
  }
  src = CGF.Builder.CreateBitCast(src, ObjCTypes.ObjectPtrTy);
  dst = CGF.Builder.CreateBitCast(dst, ObjCTypes.PtrObjectPtrTy);
  llvm37::Value *args[] = { src, dst };
  if (!threadlocal)
    CGF.EmitNounwindRuntimeCall(ObjCTypes.getGcAssignGlobalFn(),
                                args, "globalassign");
  else
    CGF.EmitNounwindRuntimeCall(ObjCTypes.getGcAssignThreadLocalFn(),
                                args, "threadlocalassign");
}

void
CGObjCNonFragileABIMac::EmitSynchronizedStmt(CodeGen::CodeGenFunction &CGF,
                                             const ObjCAtSynchronizedStmt &S) {
  EmitAtSynchronizedStmt(CGF, S,
      cast<llvm37::Function>(ObjCTypes.getSyncEnterFn()),
      cast<llvm37::Function>(ObjCTypes.getSyncExitFn()));
}

llvm37::Constant *
CGObjCNonFragileABIMac::GetEHType(QualType T) {
  // There's a particular fixed type info for 'id'.
  if (T->isObjCIdType() ||
      T->isObjCQualifiedIdType()) {
    llvm37::Constant *IDEHType =
      CGM.getModule().getGlobalVariable("OBJC_EHTYPE_id");
    if (!IDEHType)
      IDEHType =
        new llvm37::GlobalVariable(CGM.getModule(), ObjCTypes.EHTypeTy,
                                 false,
                                 llvm37::GlobalValue::ExternalLinkage,
                                 nullptr, "OBJC_EHTYPE_id");
    return IDEHType;
  }

  // All other types should be Objective-C interface pointer types.
  const ObjCObjectPointerType *PT =
    T->getAs<ObjCObjectPointerType>();
  assert(PT && "Invalid @catch type.");
  const ObjCInterfaceType *IT = PT->getInterfaceType();
  assert(IT && "Invalid @catch type.");
  return GetInterfaceEHType(IT->getDecl(), false);
}                                                  

void CGObjCNonFragileABIMac::EmitTryStmt(CodeGen::CodeGenFunction &CGF,
                                         const ObjCAtTryStmt &S) {
  EmitTryCatchStmt(CGF, S,
      cast<llvm37::Function>(ObjCTypes.getObjCBeginCatchFn()),
      cast<llvm37::Function>(ObjCTypes.getObjCEndCatchFn()),
      cast<llvm37::Function>(ObjCTypes.getExceptionRethrowFn()));
}

/// EmitThrowStmt - Generate code for a throw statement.
void CGObjCNonFragileABIMac::EmitThrowStmt(CodeGen::CodeGenFunction &CGF,
                                           const ObjCAtThrowStmt &S,
                                           bool ClearInsertionPoint) {
  if (const Expr *ThrowExpr = S.getThrowExpr()) {
    llvm37::Value *Exception = CGF.EmitObjCThrowOperand(ThrowExpr);
    Exception = CGF.Builder.CreateBitCast(Exception, ObjCTypes.ObjectPtrTy);
    CGF.EmitRuntimeCallOrInvoke(ObjCTypes.getExceptionThrowFn(), Exception)
      .setDoesNotReturn();
  } else {
    CGF.EmitRuntimeCallOrInvoke(ObjCTypes.getExceptionRethrowFn())
      .setDoesNotReturn();
  }

  CGF.Builder.CreateUnreachable();
  if (ClearInsertionPoint)
    CGF.Builder.ClearInsertionPoint();
}

llvm37::Constant *
CGObjCNonFragileABIMac::GetInterfaceEHType(const ObjCInterfaceDecl *ID,
                                           bool ForDefinition) {
  llvm37::GlobalVariable * &Entry = EHTypeReferences[ID->getIdentifier()];

  // If we don't need a definition, return the entry if found or check
  // if we use an external reference.
  if (!ForDefinition) {
    if (Entry)
      return Entry;

    // If this type (or a super class) has the __objc_exception__
    // attribute, emit an external reference.
    if (hasObjCExceptionAttribute(CGM.getContext(), ID))
      return Entry =
          new llvm37::GlobalVariable(CGM.getModule(), ObjCTypes.EHTypeTy, false,
                                   llvm37::GlobalValue::ExternalLinkage,
                                   nullptr,
                                   ("OBJC_EHTYPE_$_" +
                                    ID->getObjCRuntimeNameAsString()));
  }

  // Otherwise we need to either make a new entry or fill in the
  // initializer.
  assert((!Entry || !Entry->hasInitializer()) && "Duplicate EHType definition");
  llvm37::SmallString<64> ClassName(getClassSymbolPrefix());
  ClassName += ID->getObjCRuntimeNameAsString();
  std::string VTableName = "objc_ehtype_vtable";
  llvm37::GlobalVariable *VTableGV =
    CGM.getModule().getGlobalVariable(VTableName);
  if (!VTableGV)
    VTableGV = new llvm37::GlobalVariable(CGM.getModule(), ObjCTypes.Int8PtrTy,
                                        false,
                                        llvm37::GlobalValue::ExternalLinkage,
                                        nullptr, VTableName);

  llvm37::Value *VTableIdx = llvm37::ConstantInt::get(CGM.Int32Ty, 2);

  llvm37::Constant *Values[] = {
      llvm37::ConstantExpr::getGetElementPtr(VTableGV->getValueType(), VTableGV,
                                           VTableIdx),
      GetClassName(ID->getObjCRuntimeNameAsString()),
      GetClassGlobal(ClassName.str())};
  llvm37::Constant *Init =
    llvm37::ConstantStruct::get(ObjCTypes.EHTypeTy, Values);

  llvm37::GlobalValue::LinkageTypes L = ForDefinition
                                          ? llvm37::GlobalValue::ExternalLinkage
                                          : llvm37::GlobalValue::WeakAnyLinkage;
  if (Entry) {
    Entry->setInitializer(Init);
  } else {
    llvm37::SmallString<64> EHTYPEName("OBJC_EHTYPE_$_");
    EHTYPEName += ID->getObjCRuntimeNameAsString();
    Entry = new llvm37::GlobalVariable(CGM.getModule(), ObjCTypes.EHTypeTy, false,
                                     L,
                                     Init,
                                     EHTYPEName.str());
  }
  assert(Entry->getLinkage() == L);

  if (ID->getVisibility() == HiddenVisibility)
    Entry->setVisibility(llvm37::GlobalValue::HiddenVisibility);
  Entry->setAlignment(CGM.getDataLayout().getABITypeAlignment(
      ObjCTypes.EHTypeTy));

  if (ForDefinition)
    Entry->setSection("__DATA,__objc_const");
  else
    Entry->setSection("__DATA,__datacoal_nt,coalesced");

  return Entry;
}

/* *** */

CodeGen::CGObjCRuntime *
CodeGen::CreateMacObjCRuntime(CodeGen::CodeGenModule &CGM) {
  switch (CGM.getLangOpts().ObjCRuntime.getKind()) {
  case ObjCRuntime::FragileMacOSX:
  return new CGObjCMac(CGM);

  case ObjCRuntime::MacOSX:
  case ObjCRuntime::iOS:
    return new CGObjCNonFragileABIMac(CGM);

  case ObjCRuntime::GNUstep:
  case ObjCRuntime::GCC:
  case ObjCRuntime::ObjFW:
    llvm37_unreachable("these runtimes are not Mac runtimes");
  }
  llvm37_unreachable("bad runtime");
}

#endif // HLSL Change
