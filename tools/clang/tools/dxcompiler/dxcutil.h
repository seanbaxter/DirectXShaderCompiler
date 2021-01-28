///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// dxcutil.h                                                                 //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// Provides helper code for dxcompiler.                                      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "dxc/dxcapi.h"
#include "dxc/Support/microcom.h"
#include <memory>
#include "llvm37/ADT/StringRef.h"

namespace clang {
class DiagnosticsEngine;
}

namespace llvm37 {
class LLVM37Context;
class MemoryBuffer;
class Module;
class raw_string_ostream;
class Twine;
} // namespace llvm37

namespace hlsl {
enum class SerializeDxilFlags : uint32_t;
struct DxilShaderHash;
class AbstractMemoryStream;
namespace options {
class MainArgs;
class DxcOpts;
} // namespace options
} // namespace hlsl

namespace dxcutil {
struct AssembleInputs {
  AssembleInputs(std::unique_ptr<llvm37::Module> &&pM,
                 CComPtr<IDxcBlob> &pOutputContainerBlob,
                 IMalloc *pMalloc,
                 hlsl::SerializeDxilFlags SerializeFlags,
                 CComPtr<hlsl::AbstractMemoryStream> &pModuleBitcode,
                 bool bDebugInfo = false,
                 llvm37::StringRef DebugName = llvm37::StringRef(),
                 clang::DiagnosticsEngine *pDiag = nullptr,
                 hlsl::DxilShaderHash *pShaderHashOut = nullptr,
                 hlsl::AbstractMemoryStream *pReflectionOut = nullptr,
                 hlsl::AbstractMemoryStream *pRootSigOut = nullptr);
  std::unique_ptr<llvm37::Module> pM;
  CComPtr<IDxcBlob> &pOutputContainerBlob;
  IMalloc *pMalloc;
  hlsl::SerializeDxilFlags SerializeFlags;
  CComPtr<hlsl::AbstractMemoryStream> &pModuleBitcode;
  bool bDebugInfo;
  llvm37::StringRef DebugName = llvm37::StringRef();
  clang::DiagnosticsEngine *pDiag;
  hlsl::DxilShaderHash *pShaderHashOut = nullptr;
  hlsl::AbstractMemoryStream *pReflectionOut = nullptr;
  hlsl::AbstractMemoryStream *pRootSigOut = nullptr;
};
HRESULT ValidateAndAssembleToContainer(AssembleInputs &inputs);
HRESULT ValidateRootSignatureInContainer(
    IDxcBlob *pRootSigContainer, clang::DiagnosticsEngine *pDiag = nullptr);
void GetValidatorVersion(unsigned *pMajor, unsigned *pMinor);
void AssembleToContainer(AssembleInputs &inputs);
HRESULT Disassemble(IDxcBlob *pProgram, llvm37::raw_string_ostream &Stream);
void ReadOptsAndValidate(hlsl::options::MainArgs &mainArgs,
                         hlsl::options::DxcOpts &opts,
                         hlsl::AbstractMemoryStream *pOutputStream,
                         _COM_Outptr_ IDxcOperationResult **ppResult,
                         bool &finished);
void CreateOperationResultFromOutputs(
    DXC_OUT_KIND resultKind, UINT32 textEncoding,
    IDxcBlob *pResultBlob, CComPtr<IStream> &pErrorStream,
    const std::string &warnings, bool hasErrorOccurred,
    _COM_Outptr_ IDxcOperationResult **ppResult);
void CreateOperationResultFromOutputs(
    IDxcBlob *pResultBlob, CComPtr<IStream> &pErrorStream,
    const std::string &warnings, bool hasErrorOccurred,
    _COM_Outptr_ IDxcOperationResult **ppResult);

bool IsAbsoluteOrCurDirRelative(const llvm37::Twine &T);

} // namespace dxcutil