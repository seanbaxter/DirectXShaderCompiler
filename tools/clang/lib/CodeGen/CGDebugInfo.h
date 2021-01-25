//===--- CGDebugInfo.h - DebugInfo for LLVM CodeGen -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is the source-level debug info generator for llvm translation.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_LIB_CODEGEN_CGDEBUGINFO_H
#define LLVM37_CLANG_LIB_CODEGEN_CGDEBUGINFO_H

#include "CGBuilder.h"
#include "clang/AST/Expr.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Frontend/CodeGenOptions.h"
#include "llvm37/ADT/DenseMap.h"
#include "llvm37/IR/DIBuilder.h"
#include "llvm37/IR/DebugInfo.h"
#include "llvm37/IR/ValueHandle.h"
#include "llvm37/Support/Allocator.h"

namespace llvm37 {
class MDNode;
}

namespace clang {
class CXXMethodDecl;
class VarDecl;
class ObjCInterfaceDecl;
class ObjCIvarDecl;
class ClassTemplateSpecializationDecl;
class GlobalDecl;
class UsingDecl;

namespace CodeGen {
class CodeGenModule;
class CodeGenFunction;
class CGBlockInfo;

/// This class gathers all debug information during compilation and is
/// responsible for emitting to llvm globals or pass directly to the
/// backend.
class CGDebugInfo {
  friend class ApplyDebugLocation;
  friend class SaveAndRestoreLocation;
  CodeGenModule &CGM;
  const CodeGenOptions::DebugInfoKind DebugKind;
  llvm37::DIBuilder DBuilder;
  llvm37::DICompileUnit *TheCU = nullptr;
  SourceLocation CurLoc;
  llvm37::DIType *VTablePtrType = nullptr;
  llvm37::DIType *ClassTy = nullptr;
  llvm37::DICompositeType *ObjTy = nullptr;
  llvm37::DIType *SelTy = nullptr;
  llvm37::DIType *OCLImage1dDITy = nullptr;
  llvm37::DIType *OCLImage1dArrayDITy = nullptr;
  llvm37::DIType *OCLImage1dBufferDITy = nullptr;
  llvm37::DIType *OCLImage2dDITy = nullptr;
  llvm37::DIType *OCLImage2dArrayDITy = nullptr;
  llvm37::DIType *OCLImage3dDITy = nullptr;
  llvm37::DIType *OCLEventDITy = nullptr;

  /// Cache of previously constructed Types.
  llvm37::DenseMap<const void *, llvm37::TrackingMDRef> TypeCache;

  struct ObjCInterfaceCacheEntry {
    const ObjCInterfaceType *Type;
    llvm37::DIType *Decl;
    llvm37::DIFile *Unit;
    ObjCInterfaceCacheEntry(const ObjCInterfaceType *Type, llvm37::DIType *Decl,
                            llvm37::DIFile *Unit)
        : Type(Type), Decl(Decl), Unit(Unit) {}
  };

  /// Cache of previously constructed interfaces which may change.
  llvm37::SmallVector<ObjCInterfaceCacheEntry, 32> ObjCInterfaceCache;

  /// Cache of references to AST files such as PCHs or modules.
  llvm37::DenseMap<uint64_t, llvm37::DIModule *> ModuleRefCache;

  /// List of interfaces we want to keep even if orphaned.
  std::vector<void *> RetainedTypes;

  /// Cache of forward declared types to RAUW at the end of
  /// compilation.
  std::vector<std::pair<const TagType *, llvm37::TrackingMDRef>> ReplaceMap;

  /// Cache of replaceable forward declarartions (functions and
  /// variables) to RAUW at the end of compilation.
  std::vector<std::pair<const DeclaratorDecl *, llvm37::TrackingMDRef>>
      FwdDeclReplaceMap;

  /// Keep track of our current nested lexical block.
  std::vector<llvm37::TypedTrackingMDRef<llvm37::DIScope>> LexicalBlockStack;
  llvm37::DenseMap<const Decl *, llvm37::TrackingMDRef> RegionMap;
  /// Keep track of LexicalBlockStack counter at the beginning of a
  /// function. This is used to pop unbalanced regions at the end of a
  /// function.
  std::vector<unsigned> FnBeginRegionCount;

  /// This is a storage for names that are constructed on demand. For
  /// example, C++ destructors, C++ operators etc..
  llvm37::BumpPtrAllocator DebugInfoNames;
  StringRef CWDName;

  llvm37::DenseMap<const char *, llvm37::TrackingMDRef> DIFileCache;
  llvm37::DenseMap<const FunctionDecl *, llvm37::TrackingMDRef> SPCache;
  /// Cache declarations relevant to DW_TAG_imported_declarations (C++
  /// using declarations) that aren't covered by other more specific caches.
  llvm37::DenseMap<const Decl *, llvm37::TrackingMDRef> DeclCache;
  llvm37::DenseMap<const NamespaceDecl *, llvm37::TrackingMDRef> NameSpaceCache;
  llvm37::DenseMap<const NamespaceAliasDecl *, llvm37::TrackingMDRef>
      NamespaceAliasCache;
  llvm37::DenseMap<const Decl *, llvm37::TrackingMDRef> StaticDataMemberCache;

  /// Helper functions for getOrCreateType.
  /// @{
  /// Currently the checksum of an interface includes the number of
  /// ivars and property accessors.
  unsigned Checksum(const ObjCInterfaceDecl *InterfaceDecl);
  llvm37::DIType *CreateType(const BuiltinType *Ty);
  llvm37::DIType *CreateType(const ComplexType *Ty);
  llvm37::DIType *CreateQualifiedType(QualType Ty, llvm37::DIFile *Fg);
  llvm37::DIType *CreateType(const TypedefType *Ty, llvm37::DIFile *Fg);
  llvm37::DIType *CreateType(const TemplateSpecializationType *Ty,
                           llvm37::DIFile *Fg);
  llvm37::DIType *CreateType(const ObjCObjectPointerType *Ty, llvm37::DIFile *F);
  llvm37::DIType *CreateType(const PointerType *Ty, llvm37::DIFile *F);
  llvm37::DIType *CreateType(const BlockPointerType *Ty, llvm37::DIFile *F);
  llvm37::DIType *CreateType(const FunctionType *Ty, llvm37::DIFile *F);
  /// Get structure or union type.
  llvm37::DIType *CreateType(const RecordType *Tyg);
  llvm37::DIType *CreateTypeDefinition(const RecordType *Ty);
  llvm37::DICompositeType *CreateLimitedType(const RecordType *Ty);
  void CollectContainingType(const CXXRecordDecl *RD,
                             llvm37::DICompositeType *CT);
  /// Get Objective-C interface type.
  llvm37::DIType *CreateType(const ObjCInterfaceType *Ty, llvm37::DIFile *F);
  llvm37::DIType *CreateTypeDefinition(const ObjCInterfaceType *Ty,
                                     llvm37::DIFile *F);
  /// Get Objective-C object type.
  llvm37::DIType *CreateType(const ObjCObjectType *Ty, llvm37::DIFile *F);
  llvm37::DIType *CreateType(const VectorType *Ty, llvm37::DIFile *F);
  llvm37::DIType *CreateType(const ArrayType *Ty, llvm37::DIFile *F);
  llvm37::DIType *CreateType(const LValueReferenceType *Ty, llvm37::DIFile *F);
  llvm37::DIType *CreateType(const RValueReferenceType *Ty, llvm37::DIFile *Unit);
  llvm37::DIType *CreateType(const MemberPointerType *Ty, llvm37::DIFile *F);
  llvm37::DIType *CreateType(const AtomicType *Ty, llvm37::DIFile *F);
  /// Get enumeration type.
  llvm37::DIType *CreateEnumType(const EnumType *Ty);
  llvm37::DIType *CreateTypeDefinition(const EnumType *Ty);
  /// Look up the completed type for a self pointer in the TypeCache and
  /// create a copy of it with the ObjectPointer and Artificial flags
  /// set. If the type is not cached, a new one is created. This should
  /// never happen though, since creating a type for the implicit self
  /// argument implies that we already parsed the interface definition
  /// and the ivar declarations in the implementation.
  llvm37::DIType *CreateSelfType(const QualType &QualTy, llvm37::DIType *Ty);
  /// @}

  /// Get the type from the cache or return null type if it doesn't
  /// exist.
  llvm37::DIType *getTypeOrNull(const QualType);
  /// Return the debug type for a C++ method.
  /// \arg CXXMethodDecl is of FunctionType. This function type is
  /// not updated to include implicit \c this pointer. Use this routine
  /// to get a method type which includes \c this pointer.
  llvm37::DISubroutineType *getOrCreateMethodType(const CXXMethodDecl *Method,
                                                llvm37::DIFile *F);
  llvm37::DISubroutineType *
  getOrCreateInstanceMethodType(QualType ThisPtr, const FunctionProtoType *Func,
                                llvm37::DIFile *Unit);
  llvm37::DISubroutineType *
  getOrCreateFunctionType(const Decl *D, QualType FnType, llvm37::DIFile *F);
  /// \return debug info descriptor for vtable.
  llvm37::DIType *getOrCreateVTablePtrType(llvm37::DIFile *F);
  /// \return namespace descriptor for the given namespace decl.
  llvm37::DINamespace *getOrCreateNameSpace(const NamespaceDecl *N);
  llvm37::DIType *getOrCreateTypeDeclaration(QualType PointeeTy, llvm37::DIFile *F);
  llvm37::DIType *CreatePointerLikeType(llvm37::dwarf::Tag Tag, const Type *Ty,
                                      QualType PointeeTy, llvm37::DIFile *F);

  llvm37::Value *getCachedInterfaceTypeOrNull(const QualType Ty);
  llvm37::DIType *getOrCreateStructPtrType(StringRef Name, llvm37::DIType *&Cache);

  /// A helper function to create a subprogram for a single member
  /// function GlobalDecl.
  llvm37::DISubprogram *CreateCXXMemberFunction(const CXXMethodDecl *Method,
                                              llvm37::DIFile *F,
                                              llvm37::DIType *RecordTy);

  /// A helper function to collect debug info for C++ member
  /// functions. This is used while creating debug info entry for a
  /// Record.
  void CollectCXXMemberFunctions(const CXXRecordDecl *Decl, llvm37::DIFile *F,
                                 SmallVectorImpl<llvm37::Metadata *> &E,
                                 llvm37::DIType *T);

  /// A helper function to collect debug info for C++ base
  /// classes. This is used while creating debug info entry for a
  /// Record.
  void CollectCXXBases(const CXXRecordDecl *Decl, llvm37::DIFile *F,
                       SmallVectorImpl<llvm37::Metadata *> &EltTys,
                       llvm37::DIType *RecordTy);

  /// A helper function to collect template parameters.
  llvm37::DINodeArray CollectTemplateParams(const TemplateParameterList *TPList,
                                          ArrayRef<TemplateArgument> TAList,
                                          llvm37::DIFile *Unit);
  /// A helper function to collect debug info for function template
  /// parameters.
  llvm37::DINodeArray CollectFunctionTemplateParams(const FunctionDecl *FD,
                                                  llvm37::DIFile *Unit);

  /// A helper function to collect debug info for template
  /// parameters.
  llvm37::DINodeArray
  CollectCXXTemplateParams(const ClassTemplateSpecializationDecl *TS,
                           llvm37::DIFile *F);

  llvm37::DIType *createFieldType(StringRef name, QualType type,
                                uint64_t sizeInBitsOverride, SourceLocation loc,
                                AccessSpecifier AS, uint64_t offsetInBits,
                                llvm37::DIFile *tunit, llvm37::DIScope *scope,
                                const RecordDecl *RD = nullptr);

  /// Helpers for collecting fields of a record.
  /// @{
  void CollectRecordLambdaFields(const CXXRecordDecl *CXXDecl,
                                 SmallVectorImpl<llvm37::Metadata *> &E,
                                 llvm37::DIType *RecordTy);
  llvm37::DIDerivedType *CreateRecordStaticField(const VarDecl *Var,
                                               llvm37::DIType *RecordTy,
                                               const RecordDecl *RD);
  void CollectRecordNormalField(const FieldDecl *Field, uint64_t OffsetInBits,
                                llvm37::DIFile *F,
                                SmallVectorImpl<llvm37::Metadata *> &E,
                                llvm37::DIType *RecordTy, const RecordDecl *RD);
  void CollectRecordFields(const RecordDecl *Decl, llvm37::DIFile *F,
                           SmallVectorImpl<llvm37::Metadata *> &E,
                           llvm37::DICompositeType *RecordTy);

  /// If the C++ class has vtable info then insert appropriate debug
  /// info entry in EltTys vector.
  void CollectVTableInfo(const CXXRecordDecl *Decl, llvm37::DIFile *F,
                         SmallVectorImpl<llvm37::Metadata *> &EltTys);
  /// @}

  /// Create a new lexical block node and push it on the stack.
  void CreateLexicalBlock(SourceLocation Loc);

  // HLSL Change Begins
private:
  bool TryCollectHLSLRecordElements(const RecordType *Ty,
                                    llvm37::DICompositeType *DITy,
                                    SmallVectorImpl<llvm37::Metadata *> &Elements);
  // HLSL Change Ends

public:
  CGDebugInfo(CodeGenModule &CGM);
  ~CGDebugInfo();

  void finalize();

  /// Update the current source location. If \arg loc is invalid it is
  /// ignored.
  void setLocation(SourceLocation Loc);

  /// Emit metadata to indicate a change in line/column information in
  /// the source file. If the location is invalid, the previous
  /// location will be reused.
  void EmitLocation(CGBuilderTy &Builder, SourceLocation Loc);

  /// Emit a call to llvm.dbg.function.start to indicate
  /// start of a new function.
  /// \param Loc       The location of the function header.
  /// \param ScopeLoc  The location of the function body.
  void EmitFunctionStart(GlobalDecl GD, SourceLocation Loc,
                         SourceLocation ScopeLoc, QualType FnType,
                         llvm37::Function *Fn, CGBuilderTy &Builder);

  /// Constructs the debug code for exiting a function.
  void EmitFunctionEnd(CGBuilderTy &Builder);

  /// Emit metadata to indicate the beginning of a new lexical block
  /// and push the block onto the stack.
  void EmitLexicalBlockStart(CGBuilderTy &Builder, SourceLocation Loc);

  /// Emit metadata to indicate the end of a new lexical block and pop
  /// the current block.
  void EmitLexicalBlockEnd(CGBuilderTy &Builder, SourceLocation Loc);

  /// Emit call to \c llvm.dbg.declare for an automatic variable
  /// declaration.
  void EmitDeclareOfAutoVariable(const VarDecl *Decl, llvm37::Value *AI,
                                 CGBuilderTy &Builder);

  /// Emit call to \c llvm.dbg.declare for an imported variable
  /// declaration in a block.
  void EmitDeclareOfBlockDeclRefVariable(const VarDecl *variable,
                                         llvm37::Value *storage,
                                         CGBuilderTy &Builder,
                                         const CGBlockInfo &blockInfo,
                                         llvm37::Instruction *InsertPoint = 0);

  /// Emit call to \c llvm.dbg.declare for an argument variable
  /// declaration.
  void EmitDeclareOfArgVariable(const VarDecl *Decl, llvm37::Value *AI,
                                unsigned ArgNo, CGBuilderTy &Builder);

  /// Emit call to \c llvm.dbg.declare for the block-literal argument
  /// to a block invocation function.
  void EmitDeclareOfBlockLiteralArgVariable(const CGBlockInfo &block,
                                            llvm37::Value *Arg, unsigned ArgNo,
                                            llvm37::Value *LocalAddr,
                                            CGBuilderTy &Builder);

  /// Emit information about a global variable.
  void EmitGlobalVariable(llvm37::GlobalVariable *GV, const VarDecl *Decl);

  /// Emit global variable's debug info.
  void EmitGlobalVariable(const ValueDecl *VD, llvm37::Constant *Init);

  /// Emit C++ using directive.
  void EmitUsingDirective(const UsingDirectiveDecl &UD);

  /// Emit the type explicitly casted to.
  void EmitExplicitCastType(QualType Ty);

  /// Emit C++ using declaration.
  void EmitUsingDecl(const UsingDecl &UD);

  /// Emit an @import declaration.
  void EmitImportDecl(const ImportDecl &ID);

  /// Emit C++ namespace alias.
  llvm37::DIImportedEntity *EmitNamespaceAlias(const NamespaceAliasDecl &NA);

  /// Emit record type's standalone debug info.
  llvm37::DIType *getOrCreateRecordType(QualType Ty, SourceLocation L);

  /// Emit an Objective-C interface type standalone debug info.
  llvm37::DIType *getOrCreateInterfaceType(QualType Ty, SourceLocation Loc);

  void completeType(const EnumDecl *ED);
  void completeType(const RecordDecl *RD);
  void completeRequiredType(const RecordDecl *RD);
  void completeClassData(const RecordDecl *RD);

  void completeTemplateDefinition(const ClassTemplateSpecializationDecl &SD);

private:
  /// Emit call to llvm.dbg.declare for a variable declaration.
  /// Tag accepts custom types DW_TAG_arg_variable and DW_TAG_auto_variable,
  /// otherwise would be of type llvm37::dwarf::Tag.
  void EmitDeclare(const VarDecl *decl, llvm37::dwarf::Tag Tag, llvm37::Value *AI,
                   unsigned ArgNo, CGBuilderTy &Builder);

  /// Build up structure info for the byref.  See \a BuildByRefType.
  llvm37::DIType *EmitTypeForVarWithBlocksAttr(const VarDecl *VD,
                                             uint64_t *OffSet);

  /// Get context info for the decl.
  llvm37::DIScope *getContextDescriptor(const Decl *Decl);

  llvm37::DIScope *getCurrentContextDescriptor(const Decl *Decl);

  /// Create a forward decl for a RecordType in a given context.
  llvm37::DICompositeType *getOrCreateRecordFwdDecl(const RecordType *,
                                                  llvm37::DIScope *);

  /// Return current directory name.
  StringRef getCurrentDirname();

  /// Create new compile unit.
  void CreateCompileUnit();

  /// Get the file debug info descriptor for the input location.
  llvm37::DIFile *getOrCreateFile(SourceLocation Loc);

  /// Get the file info for main compile unit.
  llvm37::DIFile *getOrCreateMainFile();

  /// Get the type from the cache or create a new type if necessary.
  llvm37::DIType *getOrCreateType(QualType Ty, llvm37::DIFile *Fg);

  /// Get a reference to a clang module.
  llvm37::DIModule *
  getOrCreateModuleRef(ExternalASTSource::ASTSourceDescriptor Mod);

  /// Get the type from the cache or create a new partial type if
  /// necessary.
  llvm37::DIType *getOrCreateLimitedType(const RecordType *Ty, llvm37::DIFile *F);

  /// Create type metadata for a source language type.
  llvm37::DIType *CreateTypeNode(QualType Ty, llvm37::DIFile *Fg);

  /// Return the underlying ObjCInterfaceDecl if \arg Ty is an
  /// ObjCInterface or a pointer to one.
  ObjCInterfaceDecl *getObjCInterfaceDecl(QualType Ty);

  /// Create new member and increase Offset by FType's size.
  llvm37::DIType *CreateMemberType(llvm37::DIFile *Unit, QualType FType,
                                 StringRef Name, uint64_t *Offset);

  /// Retrieve the DIDescriptor, if any, for the canonical form of this
  /// declaration.
  llvm37::DINode *getDeclarationOrDefinition(const Decl *D);

  /// \return debug info descriptor to describe method
  /// declaration for the given method definition.
  llvm37::DISubprogram *getFunctionDeclaration(const Decl *D);

  /// \return debug info descriptor to describe in-class static data
  /// member declaration for the given out-of-class definition.  If D
  /// is an out-of-class definition of a static data member of a
  /// class, find its corresponding in-class declaration.
  llvm37::DIDerivedType *
  getOrCreateStaticDataMemberDeclarationOrNull(const VarDecl *D);

  /// Create a subprogram describing the forward declaration
  /// represented in the given FunctionDecl.
  llvm37::DISubprogram *getFunctionForwardDeclaration(const FunctionDecl *FD);

  /// Create a global variable describing the forward decalration
  /// represented in the given VarDecl.
  llvm37::DIGlobalVariable *
  getGlobalVariableForwardDeclaration(const VarDecl *VD);

  /// \brief Return a global variable that represents one of the
  /// collection of global variables created for an anonmyous union.
  ///
  /// Recursively collect all of the member fields of a global
  /// anonymous decl and create static variables for them. The first
  /// time this is called it needs to be on a union and then from
  /// there we can have additional unnamed fields.
  llvm37::DIGlobalVariable *
  CollectAnonRecordDecls(const RecordDecl *RD, llvm37::DIFile *Unit,
                         unsigned LineNo, StringRef LinkageName,
                         llvm37::GlobalVariable *Var, llvm37::DIScope *DContext);

  /// Get function name for the given FunctionDecl. If the name is
  /// constructed on demand (e.g., C++ destructor) then the name is
  /// stored on the side.
  StringRef getFunctionName(const FunctionDecl *FD);

  /// Returns the unmangled name of an Objective-C method.
  /// This is the display name for the debugging info.
  StringRef getObjCMethodName(const ObjCMethodDecl *FD);

  /// Return selector name. This is used for debugging
  /// info.
  StringRef getSelectorName(Selector S);

  /// Get class name including template argument list.
  StringRef getClassName(const RecordDecl *RD);

  /// Get the vtable name for the given class.
  StringRef getVTableName(const CXXRecordDecl *Decl);

  /// Get line number for the location. If location is invalid
  /// then use current location.
  unsigned getLineNumber(SourceLocation Loc);

  /// Get column number for the location. If location is
  /// invalid then use current location.
  /// \param Force  Assume DebugColumnInfo option is true.
  unsigned getColumnNumber(SourceLocation Loc, bool Force = false);

  /// Collect various properties of a FunctionDecl.
  /// \param GD  A GlobalDecl whose getDecl() must return a FunctionDecl.
  void collectFunctionDeclProps(GlobalDecl GD, llvm37::DIFile *Unit,
                                StringRef &Name, StringRef &LinkageName,
                                llvm37::DIScope *&FDContext,
                                llvm37::DINodeArray &TParamsArray,
                                unsigned &Flags);

  /// Collect various properties of a VarDecl.
  void collectVarDeclProps(const VarDecl *VD, llvm37::DIFile *&Unit,
                           unsigned &LineNo, QualType &T, StringRef &Name,
                           StringRef &LinkageName, llvm37::DIScope *&VDContext);

  /// Allocate a copy of \p A using the DebugInfoNames allocator
  /// and return a reference to it. If multiple arguments are given the strings
  /// are concatenated.
  StringRef internString(StringRef A, StringRef B = StringRef()) {
    char *Data = DebugInfoNames.Allocate<char>(A.size() + B.size());
    if (!A.empty())
      std::memcpy(Data, A.data(), A.size());
    if (!B.empty())
      std::memcpy(Data + A.size(), B.data(), B.size());
    return StringRef(Data, A.size() + B.size());
  }
};

/// A scoped helper to set the current debug location to the specified
/// location or preferred location of the specified Expr.
class ApplyDebugLocation {
private:
  void init(SourceLocation TemporaryLocation, bool DefaultToEmpty = false);
  ApplyDebugLocation(CodeGenFunction &CGF, bool DefaultToEmpty,
                     SourceLocation TemporaryLocation);

  llvm37::DebugLoc OriginalLocation;
  CodeGenFunction &CGF;

public:
  /// Set the location to the (valid) TemporaryLocation.
  ApplyDebugLocation(CodeGenFunction &CGF, SourceLocation TemporaryLocation);
  ApplyDebugLocation(CodeGenFunction &CGF, const Expr *E);
  ApplyDebugLocation(CodeGenFunction &CGF, llvm37::DebugLoc Loc);

  ~ApplyDebugLocation();

  /// \brief Apply TemporaryLocation if it is valid. Otherwise switch
  /// to an artificial debug location that has a valid scope, but no
  /// line information.
  ///
  /// Artificial locations are useful when emitting compiler-generated
  /// helper functions that have no source location associated with
  /// them. The DWARF specification allows the compiler to use the
  /// special line number 0 to indicate code that can not be
  /// attributed to any source location. Note that passing an empty
  /// SourceLocation to CGDebugInfo::setLocation() will result in the
  /// last valid location being reused.
  static ApplyDebugLocation CreateArtificial(CodeGenFunction &CGF) {
    return ApplyDebugLocation(CGF, false, SourceLocation());
  }
  /// \brief Apply TemporaryLocation if it is valid. Otherwise switch
  /// to an artificial debug location that has a valid scope, but no
  /// line information.
  static ApplyDebugLocation
  CreateDefaultArtificial(CodeGenFunction &CGF,
                          SourceLocation TemporaryLocation) {
    return ApplyDebugLocation(CGF, false, TemporaryLocation);
  }

  /// Set the IRBuilder to not attach debug locations.  Note that
  /// passing an empty SourceLocation to \a CGDebugInfo::setLocation()
  /// will result in the last valid location being reused.  Note that
  /// all instructions that do not have a location at the beginning of
  /// a function are counted towards to funciton prologue.
  static ApplyDebugLocation CreateEmpty(CodeGenFunction &CGF) {
    return ApplyDebugLocation(CGF, true, SourceLocation());
  }

  /// \brief Apply TemporaryLocation if it is valid. Otherwise set the IRBuilder
  /// to not attach debug locations.
  static ApplyDebugLocation
  CreateDefaultEmpty(CodeGenFunction &CGF, SourceLocation TemporaryLocation) {
    return ApplyDebugLocation(CGF, true, TemporaryLocation);
  }
};

} // namespace CodeGen
} // namespace clang

#endif
