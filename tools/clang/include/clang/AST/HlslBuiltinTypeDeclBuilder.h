///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifndef LLVM37_CLANG_AST_HLSLBUILTINTYPEDECLBUILDER_H
#define LLVM37_CLANG_AST_HLSLBUILTINTYPEDECLBUILDER_H

#include "clang/AST/Decl.h"
#include "clang/AST/Type.h"
#include "llvm37/ADT/ArrayRef.h"
#include "llvm37/ADT/Optional.h"
#include "llvm37/ADT/StringRef.h"

namespace clang {
  class ASTContext;
  class DeclContext;
  class CXXRecordDecl;
  class ClassTemplateDecl;
  class NamedDecl;
}

namespace hlsl {
// Helper to declare a builtin HLSL type in the clang AST with minimal boilerplate.
class BuiltinTypeDeclBuilder final {
public:
  BuiltinTypeDeclBuilder(clang::DeclContext* declContext, llvm37::StringRef name,
    clang::TagDecl::TagKind tagKind = clang::TagDecl::TagKind::TTK_Class);

  clang::TemplateTypeParmDecl* addTypeTemplateParam(llvm37::StringRef name, clang::TypeSourceInfo* defaultValue = nullptr);
  clang::TemplateTypeParmDecl* addTypeTemplateParam(llvm37::StringRef name, clang::QualType defaultValue);
  clang::NonTypeTemplateParmDecl* addIntegerTemplateParam(llvm37::StringRef name, clang::QualType type,
    llvm37::Optional<int64_t> defaultValue = llvm37::None);

  void startDefinition();

  clang::FieldDecl* addField(llvm37::StringRef name, clang::QualType type,
    clang::AccessSpecifier access = clang::AccessSpecifier::AS_private);

  clang::CXXRecordDecl* completeDefinition();

  clang::CXXRecordDecl* getRecordDecl() const { return m_recordDecl; }
  clang::ClassTemplateDecl* getTemplateDecl() const;

private:
  clang::CXXRecordDecl* m_recordDecl = nullptr;
  clang::ClassTemplateDecl* m_templateDecl = nullptr;
  llvm37::SmallVector<clang::NamedDecl*, 2> m_templateParams;
};
} // end hlsl namespace
#endif
