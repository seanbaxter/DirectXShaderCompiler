//===--- BackendUtil.h - LLVM37 Backend Utilities -----------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_CODEGEN_BACKENDUTIL_H
#define LLVM37_CLANG_CODEGEN_BACKENDUTIL_H

#include "clang/Basic/LLVM.h"

namespace llvm37 {
  class Module;
}

namespace clang {
  class DiagnosticsEngine;
  class CodeGenOptions;
  class TargetOptions;
  class LangOptions;

  enum BackendAction {
    Backend_EmitAssembly,  ///< Emit native assembly files
    Backend_EmitBC,        ///< Emit LLVM37 bitcode files
    Backend_EmitLL,        ///< Emit human-readable LLVM37 assembly
    Backend_EmitNothing,   ///< Don't emit anything (benchmarking mode)
    Backend_EmitMCNull,    ///< Run CodeGen, but don't emit anything
    Backend_EmitObj,       ///< Emit native object files
    Backend_EmitPasses     ///< Emit pass configuration - HLSL Change
  };

  void EmitBackendOutput(DiagnosticsEngine &Diags, const CodeGenOptions &CGOpts,
                         const TargetOptions &TOpts, const LangOptions &LOpts,
                         StringRef TDesc, llvm37::Module *M, BackendAction Action,
                         raw_pwrite_stream *OS);
}

#endif
