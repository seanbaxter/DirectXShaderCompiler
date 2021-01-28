///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilLinker.h                                                              //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// Representation of HLSL Linker.                                            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <unordered_map>
#include <unordered_set>
#include "llvm37/ADT/StringRef.h"
#include "llvm37/ADT/StringMap.h"
#include <memory>
#include "llvm37/Support/ErrorOr.h"
#include "dxc/HLSL/DxilExportMap.h"

namespace llvm37 {
class Function;
class GlobalVariable;
class Constant;
class Module;
class LLVM37Context;
} // namespace llvm37

namespace hlsl {
class DxilModule;
class DxilResourceBase;

// Linker for DxilModule.
class DxilLinker {
public:
  virtual ~DxilLinker() {}
  static DxilLinker *CreateLinker(llvm37::LLVM37Context &Ctx, unsigned valMajor, unsigned valMinor);

  void SetValidatorVersion(unsigned valMajor, unsigned valMinor) { m_valMajor = valMajor, m_valMinor = valMinor; }
  virtual bool HasLibNameRegistered(llvm37::StringRef name) = 0;
  virtual bool RegisterLib(llvm37::StringRef name,
                           std::unique_ptr<llvm37::Module> pModule,
                           std::unique_ptr<llvm37::Module> pDebugModule) = 0;
  virtual bool AttachLib(llvm37::StringRef name) = 0;
  virtual bool DetachLib(llvm37::StringRef name) = 0;
  virtual void DetachAll() = 0;

  virtual std::unique_ptr<llvm37::Module>
  Link(llvm37::StringRef entry, llvm37::StringRef profile, dxilutil::ExportMap &exportMap) = 0;

protected:
  DxilLinker(llvm37::LLVM37Context &Ctx, unsigned valMajor, unsigned valMinor) : m_ctx(Ctx), m_valMajor(valMajor), m_valMinor(valMinor) {}
  llvm37::LLVM37Context &m_ctx;
  unsigned m_valMajor, m_valMinor;
};

} // namespace hlsl
