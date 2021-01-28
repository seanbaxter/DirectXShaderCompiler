//===-- llvm37/Support/CodeGen.h - CodeGen Concepts ---------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file define some types which define code generation concepts. For
// example, relocation model.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_SUPPORT_CODEGEN_H
#define LLVM37_SUPPORT_CODEGEN_H

#include "llvm37-c/TargetMachine.h"
#include "llvm37/Support/ErrorHandling.h"

namespace llvm37 {

  // Relocation model types.
  namespace Reloc {
    enum Model { Default, Static, PIC_, DynamicNoPIC };
  }

  // Code model types.
  namespace CodeModel {
    enum Model { Default, JITDefault, Small, Kernel, Medium, Large };
  }

  namespace PICLevel {
    enum Level { Default=0, Small=1, Large=2 };
  }

  // TLS models.
  namespace TLSModel {
    enum Model {
      GeneralDynamic,
      LocalDynamic,
      InitialExec,
      LocalExec
    };
  }

  // Code generation optimization level.
  namespace CodeGenOpt {
    enum Level {
      None,        // -O0
      Less,        // -O1
      Default,     // -O2, -Os
      Aggressive   // -O3
    };
  }

  // Create wrappers for C Binding types (see CBindingWrapping.h).
  inline CodeModel::Model unwrap(LLVM37CodeModel Model) {
    switch (Model) {
      case LLVM37CodeModelDefault:
        return CodeModel::Default;
      case LLVM37CodeModelJITDefault:
        return CodeModel::JITDefault;
      case LLVM37CodeModelSmall:
        return CodeModel::Small;
      case LLVM37CodeModelKernel:
        return CodeModel::Kernel;
      case LLVM37CodeModelMedium:
        return CodeModel::Medium;
      case LLVM37CodeModelLarge:
        return CodeModel::Large;
    }
    return CodeModel::Default;
  }

  inline LLVM37CodeModel wrap(CodeModel::Model Model) {
    switch (Model) {
      case CodeModel::Default:
        return LLVM37CodeModelDefault;
      case CodeModel::JITDefault:
        return LLVM37CodeModelJITDefault;
      case CodeModel::Small:
        return LLVM37CodeModelSmall;
      case CodeModel::Kernel:
        return LLVM37CodeModelKernel;
      case CodeModel::Medium:
        return LLVM37CodeModelMedium;
      case CodeModel::Large:
        return LLVM37CodeModelLarge;
    }
    llvm37_unreachable("Bad CodeModel!");
  }
}  // end llvm37 namespace

#endif
