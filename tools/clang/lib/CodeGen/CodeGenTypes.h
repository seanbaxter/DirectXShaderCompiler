//===--- CodeGenTypes.h - Type translation for LLVM37 CodeGen -----*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is the code that handles AST -> LLVM37 type lowering.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_LIB_CODEGEN_CODEGENTYPES_H
#define LLVM37_CLANG_LIB_CODEGEN_CODEGENTYPES_H

#include "CGCall.h"
#include "clang/AST/GlobalDecl.h"
#include "clang/CodeGen/CGFunctionInfo.h"
#include "llvm37/ADT/DenseMap.h"
#include "llvm37/IR/Module.h"
#include <vector>

namespace llvm37 {
class FunctionType;
class Module;
class DataLayout;
class Type;
class LLVM37Context;
class StructType;
}

namespace clang {
class ABIInfo;
class ASTContext;
template <typename> class CanQual;
class CXXConstructorDecl;
class CXXDestructorDecl;
class CXXMethodDecl;
class CodeGenOptions;
class FieldDecl;
class FunctionProtoType;
class ObjCInterfaceDecl;
class ObjCIvarDecl;
class PointerType;
class QualType;
class RecordDecl;
class TagDecl;
class TargetInfo;
class Type;
typedef CanQual<Type> CanQualType;

namespace CodeGen {
class CGCXXABI;
class CGRecordLayout;
class CodeGenModule;
class RequiredArgs;

enum class StructorType {
  Complete, // constructor or destructor
  Base,     // constructor or destructor
  Deleting  // destructor only
};

inline CXXCtorType toCXXCtorType(StructorType T) {
  switch (T) {
  case StructorType::Complete:
    return Ctor_Complete;
  case StructorType::Base:
    return Ctor_Base;
  case StructorType::Deleting:
    llvm37_unreachable("cannot have a deleting ctor");
  }
  llvm37_unreachable("not a StructorType");
}

inline StructorType getFromCtorType(CXXCtorType T) {
  switch (T) {
  case Ctor_Complete:
    return StructorType::Complete;
  case Ctor_Base:
    return StructorType::Base;
  case Ctor_Comdat:
    llvm37_unreachable("not expecting a COMDAT");
  case Ctor_CopyingClosure:
  case Ctor_DefaultClosure:
    llvm37_unreachable("not expecting a closure");
  }
  llvm37_unreachable("not a CXXCtorType");
}

inline CXXDtorType toCXXDtorType(StructorType T) {
  switch (T) {
  case StructorType::Complete:
    return Dtor_Complete;
  case StructorType::Base:
    return Dtor_Base;
  case StructorType::Deleting:
    return Dtor_Deleting;
  }
  llvm37_unreachable("not a StructorType");
}

inline StructorType getFromDtorType(CXXDtorType T) {
  switch (T) {
  case Dtor_Deleting:
    return StructorType::Deleting;
  case Dtor_Complete:
    return StructorType::Complete;
  case Dtor_Base:
    return StructorType::Base;
  case Dtor_Comdat:
    llvm37_unreachable("not expecting a COMDAT");
  }
  llvm37_unreachable("not a CXXDtorType");
}

/// This class organizes the cross-module state that is used while lowering
/// AST types to LLVM37 types.
class CodeGenTypes {
  CodeGenModule &CGM;
  // Some of this stuff should probably be left on the CGM.
  ASTContext &Context;
  llvm37::Module &TheModule;
  const llvm37::DataLayout &TheDataLayout;
  const TargetInfo &Target;
  CGCXXABI &TheCXXABI;

  // This should not be moved earlier, since its initialization depends on some
  // of the previous reference members being already initialized
  const ABIInfo &TheABIInfo;

  /// The opaque type map for Objective-C interfaces. All direct
  /// manipulation is done by the runtime interfaces, which are
  /// responsible for coercing to the appropriate type; these opaque
  /// types are never refined.
  llvm37::DenseMap<const ObjCInterfaceType*, llvm37::Type *> InterfaceTypes;

  /// Maps clang struct type with corresponding record layout info.
  llvm37::DenseMap<const Type*, CGRecordLayout *> CGRecordLayouts;

  /// Contains the LLVM37 IR type for any converted RecordDecl.
  llvm37::DenseMap<const Type*, llvm37::StructType *> RecordDeclTypes;
  
  /// Hold memoized CGFunctionInfo results.
  llvm37::FoldingSet<CGFunctionInfo> FunctionInfos;

  /// This set keeps track of records that we're currently converting
  /// to an IR type.  For example, when converting:
  /// struct A { struct B { int x; } } when processing 'x', the 'A' and 'B'
  /// types will be in this set.
  llvm37::SmallPtrSet<const Type*, 4> RecordsBeingLaidOut;
  
  llvm37::SmallPtrSet<const CGFunctionInfo*, 4> FunctionsBeingProcessed;
  
  /// True if we didn't layout a function due to a being inside
  /// a recursive struct conversion, set this to true.
  bool SkippedLayout;

  SmallVector<const RecordDecl *, 8> DeferredRecords;
  
private:
  /// This map keeps cache of llvm37::Types and maps clang::Type to
  /// corresponding llvm37::Type.
  llvm37::DenseMap<const Type *, llvm37::Type *> TypeCache;

public:
  CodeGenTypes(CodeGenModule &cgm);
  ~CodeGenTypes();

  const llvm37::DataLayout &getDataLayout() const { return TheDataLayout; }
  ASTContext &getContext() const { return Context; }
  const ABIInfo &getABIInfo() const { return TheABIInfo; }
  const TargetInfo &getTarget() const { return Target; }
  CGCXXABI &getCXXABI() const { return TheCXXABI; }
  llvm37::LLVM37Context &getLLVM37Context() { return TheModule.getContext(); }

  /// ConvertType - Convert type T into a llvm37::Type.
  llvm37::Type *ConvertType(QualType T);

  /// ConvertTypeForMem - Convert type T into a llvm37::Type.  This differs from
  /// ConvertType in that it is used to convert to the memory representation for
  /// a type.  For example, the scalar representation for _Bool is i1, but the
  /// memory representation is usually i8 or i32, depending on the target.
  llvm37::Type *ConvertTypeForMem(QualType T);

  /// GetFunctionType - Get the LLVM37 function type for \arg Info.
  llvm37::FunctionType *GetFunctionType(const CGFunctionInfo &Info);

  llvm37::FunctionType *GetFunctionType(GlobalDecl GD);

  /// isFuncTypeConvertible - Utility to check whether a function type can
  /// be converted to an LLVM37 type (i.e. doesn't depend on an incomplete tag
  /// type).
  bool isFuncTypeConvertible(const FunctionType *FT);
  bool isFuncParamTypeConvertible(QualType Ty);

  /// GetFunctionTypeForVTable - Get the LLVM37 function type for use in a vtable,
  /// given a CXXMethodDecl. If the method to has an incomplete return type,
  /// and/or incomplete argument types, this will return the opaque type.
  llvm37::Type *GetFunctionTypeForVTable(GlobalDecl GD);

  const CGRecordLayout &getCGRecordLayout(const RecordDecl*);

  /// UpdateCompletedType - When we find the full definition for a TagDecl,
  /// replace the 'opaque' type we previously made for it if applicable.
  void UpdateCompletedType(const TagDecl *TD);

  /// getNullaryFunctionInfo - Get the function info for a void()
  /// function with standard CC.
  const CGFunctionInfo &arrangeNullaryFunction();

  // The arrangement methods are split into three families:
  //   - those meant to drive the signature and prologue/epilogue
  //     of a function declaration or definition,
  //   - those meant for the computation of the LLVM37 type for an abstract
  //     appearance of a function, and
  //   - those meant for performing the IR-generation of a call.
  // They differ mainly in how they deal with optional (i.e. variadic)
  // arguments, as well as unprototyped functions.
  //
  // Key points:
  // - The CGFunctionInfo for emitting a specific call site must include
  //   entries for the optional arguments.
  // - The function type used at the call site must reflect the formal
  //   signature of the declaration being called, or else the call will
  //   go awry.
  // - For the most part, unprototyped functions are called by casting to
  //   a formal signature inferred from the specific argument types used
  //   at the call-site.  However, some targets (e.g. x86-64) screw with
  //   this for compatibility reasons.

  const CGFunctionInfo &arrangeGlobalDeclaration(GlobalDecl GD);
  const CGFunctionInfo &arrangeFunctionDeclaration(const FunctionDecl *FD);
  const CGFunctionInfo &
  arrangeFreeFunctionDeclaration(QualType ResTy, const FunctionArgList &Args,
                                 const FunctionType::ExtInfo &Info,
                                 bool isVariadic);

  const CGFunctionInfo &arrangeObjCMethodDeclaration(const ObjCMethodDecl *MD);
  const CGFunctionInfo &arrangeObjCMessageSendSignature(const ObjCMethodDecl *MD,
                                                        QualType receiverType);

  const CGFunctionInfo &arrangeCXXMethodDeclaration(const CXXMethodDecl *MD);
  const CGFunctionInfo &arrangeCXXStructorDeclaration(const CXXMethodDecl *MD,
                                                      StructorType Type);
  const CGFunctionInfo &arrangeCXXConstructorCall(const CallArgList &Args,
                                                  const CXXConstructorDecl *D,
                                                  CXXCtorType CtorKind,
                                                  unsigned ExtraArgs);
  const CGFunctionInfo &arrangeFreeFunctionCall(const CallArgList &Args,
                                                const FunctionType *Ty,
                                                bool ChainCall);
  const CGFunctionInfo &arrangeFreeFunctionCall(QualType ResTy,
                                                const CallArgList &args,
                                                FunctionType::ExtInfo info,
                                                RequiredArgs required);
  const CGFunctionInfo &arrangeBlockFunctionCall(const CallArgList &args,
                                                 const FunctionType *type);

  const CGFunctionInfo &arrangeCXXMethodCall(const CallArgList &args,
                                             const FunctionProtoType *type,
                                             RequiredArgs required);
  const CGFunctionInfo &arrangeMSMemberPointerThunk(const CXXMethodDecl *MD);
  const CGFunctionInfo &arrangeMSCtorClosure(const CXXConstructorDecl *CD,
                                                 CXXCtorType CT);

  const CGFunctionInfo &arrangeFreeFunctionType(CanQual<FunctionProtoType> Ty);
  const CGFunctionInfo &arrangeFreeFunctionType(CanQual<FunctionNoProtoType> Ty);
  const CGFunctionInfo &arrangeCXXMethodType(const CXXRecordDecl *RD,
                                             const FunctionProtoType *FTP);

  /// "Arrange" the LLVM37 information for a call or type with the given
  /// signature.  This is largely an internal method; other clients
  /// should use one of the above routines, which ultimately defer to
  /// this.
  ///
  /// \param argTypes - must all actually be canonical as params
  const CGFunctionInfo &arrangeLLVM37FunctionInfo(CanQualType returnType,
                                                bool instanceMethod,
                                                bool chainCall,
                                                ArrayRef<CanQualType> argTypes,
                                                FunctionType::ExtInfo info,
                                                RequiredArgs args);

  /// \brief Compute a new LLVM37 record layout object for the given record.
  CGRecordLayout *ComputeRecordLayout(const RecordDecl *D,
                                      llvm37::StructType *Ty);

  /// addRecordTypeName - Compute a name from the given record decl with an
  /// optional suffix and name the given LLVM37 type using it.
  void addRecordTypeName(const RecordDecl *RD, llvm37::StructType *Ty,
                         StringRef suffix);
  

public:  // These are internal details of CGT that shouldn't be used externally.
  /// ConvertRecordDeclType - Lay out a tagged decl type like struct or union.
  llvm37::StructType *ConvertRecordDeclType(const RecordDecl *TD);

  /// getExpandedTypes - Expand the type \arg Ty into the LLVM37
  /// argument types it would be passed as. See ABIArgInfo::Expand.
  void getExpandedTypes(QualType Ty,
                        SmallVectorImpl<llvm37::Type *>::iterator &TI);

  /// IsZeroInitializable - Return whether a type can be
  /// zero-initialized (in the C++ sense) with an LLVM37 zeroinitializer.
  bool isZeroInitializable(QualType T);

  /// IsZeroInitializable - Return whether a record type can be
  /// zero-initialized (in the C++ sense) with an LLVM37 zeroinitializer.
  bool isZeroInitializable(const RecordDecl *RD);
  
  bool isRecordLayoutComplete(const Type *Ty) const;
  bool noRecordsBeingLaidOut() const {
    return RecordsBeingLaidOut.empty();
  }
  bool isRecordBeingLaidOut(const Type *Ty) const {
    return RecordsBeingLaidOut.count(Ty);
  }
                            
};

}  // end namespace CodeGen
}  // end namespace clang

#endif
