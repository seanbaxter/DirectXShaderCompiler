//===------- SPIRVOptions.h -------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file outlines the command-line options used by SPIR-V CodeGen.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_SPIRV_OPTIONS_H
#define LLVM37_SPIRV_OPTIONS_H

#ifdef ENABLE_SPIRV_CODEGEN

#include "llvm37/ADT/ArrayRef.h"
#include "llvm37/ADT/StringRef.h"
#include "llvm37/Option/ArgList.h"

namespace clang {
namespace spirv {

enum class SpirvLayoutRule {
  Void,
  GLSLStd140,
  GLSLStd430,
  RelaxedGLSLStd140, // std140 with relaxed vector layout
  RelaxedGLSLStd430, // std430 with relaxed vector layout
  FxcCTBuffer,       // fxc.exe layout rule for cbuffer/tbuffer
  FxcSBuffer,        // fxc.exe layout rule for structured buffers
  Scalar,            // VK_EXT_scalar_block_layout
  Max,               // This is an invalid layout rule
};

struct SpirvCodeGenOptions {
  /// Disable legalization and optimization and emit raw SPIR-V
  bool codeGenHighLevel;
  bool debugInfoFile;
  bool debugInfoLine;
  bool debugInfoSource;
  bool debugInfoTool;
  bool debugInfoRich;
  bool defaultRowMajor;
  bool disableValidation;
  bool enable16BitTypes;
  bool enableReflect;
  bool invertY; // Additive inverse
  bool invertW; // Multiplicative inverse
  bool noWarnEmulatedFeatures;
  bool noWarnIgnoredFeatures;
  bool useDxLayout;
  bool useGlLayout;
  bool useScalarLayout;
  bool flattenResourceArrays;
  bool autoShiftBindings;
  SpirvLayoutRule cBufferLayoutRule;
  SpirvLayoutRule sBufferLayoutRule;
  SpirvLayoutRule tBufferLayoutRule;
  SpirvLayoutRule ampPayloadLayoutRule;
  llvm37::StringRef stageIoOrder;
  llvm37::StringRef targetEnv;
  llvm37::SmallVector<int32_t, 4> bShift;
  llvm37::SmallVector<int32_t, 4> sShift;
  llvm37::SmallVector<int32_t, 4> tShift;
  llvm37::SmallVector<int32_t, 4> uShift;
  llvm37::SmallVector<llvm37::StringRef, 4> allowedExtensions;
  llvm37::SmallVector<llvm37::StringRef, 4> optConfig;
  std::vector<std::string> bindRegister;
  std::vector<std::string> bindGlobals;

  // String representation of all command line options.
  std::string clOptions;
};

} // namespace spirv
} // namespace clang

#endif // ENABLE_SPIRV_CODEGEN
#endif // LLVM37_SPIRV_OPTIONS_H
