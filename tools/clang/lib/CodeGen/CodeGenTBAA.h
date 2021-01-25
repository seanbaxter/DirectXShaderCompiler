//===--- CodeGenTBAA.h - TBAA information for LLVM CodeGen ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is the code that manages TBAA information and defines the TBAA policy
// for the optimizer to use.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_LIB_CODEGEN_CODEGENTBAA_H
#define LLVM37_CLANG_LIB_CODEGEN_CODEGENTBAA_H

#include "clang/Basic/LLVM.h"
#include "llvm37/ADT/DenseMap.h"
#include "llvm37/IR/MDBuilder.h"

namespace llvm37 {
  class LLVMContext;
  class MDNode;
}

namespace clang {
  class ASTContext;
  class CodeGenOptions;
  class LangOptions;
  class MangleContext;
  class QualType;
  class Type;

namespace CodeGen {
  class CGRecordLayout;

  struct TBAAPathTag {
    TBAAPathTag(const Type *B, const llvm37::MDNode *A, uint64_t O)
      : BaseT(B), AccessN(A), Offset(O) {}
    const Type *BaseT;
    const llvm37::MDNode *AccessN;
    uint64_t Offset;
  };

/// CodeGenTBAA - This class organizes the cross-module state that is used
/// while lowering AST types to LLVM types.
class CodeGenTBAA {
  ASTContext &Context;
  const CodeGenOptions &CodeGenOpts;
  const LangOptions &Features;
  MangleContext &MContext;

  // MDHelper - Helper for creating metadata.
  llvm37::MDBuilder MDHelper;

  /// MetadataCache - This maps clang::Types to scalar llvm37::MDNodes describing
  /// them.
  llvm37::DenseMap<const Type *, llvm37::MDNode *> MetadataCache;
  /// This maps clang::Types to a struct node in the type DAG.
  llvm37::DenseMap<const Type *, llvm37::MDNode *> StructTypeMetadataCache;
  /// This maps TBAAPathTags to a tag node.
  llvm37::DenseMap<TBAAPathTag, llvm37::MDNode *> StructTagMetadataCache;
  /// This maps a scalar type to a scalar tag node.
  llvm37::DenseMap<const llvm37::MDNode *, llvm37::MDNode *> ScalarTagMetadataCache;

  /// StructMetadataCache - This maps clang::Types to llvm37::MDNodes describing
  /// them for struct assignments.
  llvm37::DenseMap<const Type *, llvm37::MDNode *> StructMetadataCache;

  llvm37::MDNode *Root;
  llvm37::MDNode *Char;

  /// getRoot - This is the mdnode for the root of the metadata type graph
  /// for this translation unit.
  llvm37::MDNode *getRoot();

  /// getChar - This is the mdnode for "char", which is special, and any types
  /// considered to be equivalent to it.
  llvm37::MDNode *getChar();

  /// CollectFields - Collect information about the fields of a type for
  /// !tbaa.struct metadata formation. Return false for an unsupported type.
  bool CollectFields(uint64_t BaseOffset,
                     QualType Ty,
                     SmallVectorImpl<llvm37::MDBuilder::TBAAStructField> &Fields,
                     bool MayAlias);

  /// A wrapper function to create a scalar type. For struct-path aware TBAA,
  /// the scalar type has the same format as the struct type: name, offset,
  /// pointer to another node in the type DAG.
  llvm37::MDNode *createTBAAScalarType(StringRef Name, llvm37::MDNode *Parent);

public:
  CodeGenTBAA(ASTContext &Ctx, llvm37::LLVMContext &VMContext,
              const CodeGenOptions &CGO,
              const LangOptions &Features,
              MangleContext &MContext);
  ~CodeGenTBAA();

  /// getTBAAInfo - Get the TBAA MDNode to be used for a dereference
  /// of the given type.
  llvm37::MDNode *getTBAAInfo(QualType QTy);

  /// getTBAAInfoForVTablePtr - Get the TBAA MDNode to be used for a
  /// dereference of a vtable pointer.
  llvm37::MDNode *getTBAAInfoForVTablePtr();

  /// getTBAAStructInfo - Get the TBAAStruct MDNode to be used for a memcpy of
  /// the given type.
  llvm37::MDNode *getTBAAStructInfo(QualType QTy);

  /// Get the MDNode in the type DAG for given struct type QType.
  llvm37::MDNode *getTBAAStructTypeInfo(QualType QType);
  /// Get the tag MDNode for a given base type, the actual scalar access MDNode
  /// and offset into the base type.
  llvm37::MDNode *getTBAAStructTagInfo(QualType BaseQType,
                                     llvm37::MDNode *AccessNode, uint64_t Offset);

  /// Get the scalar tag MDNode for a given scalar type.
  llvm37::MDNode *getTBAAScalarTagInfo(llvm37::MDNode *AccessNode);
};

}  // end namespace CodeGen
}  // end namespace clang

namespace llvm37 {

template<> struct DenseMapInfo<clang::CodeGen::TBAAPathTag> {
  static clang::CodeGen::TBAAPathTag getEmptyKey() {
    return clang::CodeGen::TBAAPathTag(
      DenseMapInfo<const clang::Type *>::getEmptyKey(),
      DenseMapInfo<const MDNode *>::getEmptyKey(),
      DenseMapInfo<uint64_t>::getEmptyKey());
  }

  static clang::CodeGen::TBAAPathTag getTombstoneKey() {
    return clang::CodeGen::TBAAPathTag(
      DenseMapInfo<const clang::Type *>::getTombstoneKey(),
      DenseMapInfo<const MDNode *>::getTombstoneKey(),
      DenseMapInfo<uint64_t>::getTombstoneKey());
  }

  static unsigned getHashValue(const clang::CodeGen::TBAAPathTag &Val) {
    return DenseMapInfo<const clang::Type *>::getHashValue(Val.BaseT) ^
           DenseMapInfo<const MDNode *>::getHashValue(Val.AccessN) ^
           DenseMapInfo<uint64_t>::getHashValue(Val.Offset);
  }

  static bool isEqual(const clang::CodeGen::TBAAPathTag &LHS,
                      const clang::CodeGen::TBAAPathTag &RHS) {
    return LHS.BaseT == RHS.BaseT &&
           LHS.AccessN == RHS.AccessN &&
           LHS.Offset == RHS.Offset;
  }
};

}  // end namespace llvm37

#endif
