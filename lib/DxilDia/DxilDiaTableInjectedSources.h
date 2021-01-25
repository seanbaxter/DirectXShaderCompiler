///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilDiaTableInjectedSources.h                                             //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// DIA API implementation for DXIL modules.                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "dxc/Support/WinIncludes.h"

#include <vector>

#include "dia2.h"

#include "dxc/Support/Global.h"
#include "dxc/Support/microcom.h"
#include "llvm37/ADT/StringRef.h"
#include "llvm37/IR/Metadata.h"

#include "DxilDia.h"
#include "DxilDiaTable.h"

namespace dxil_dia {

class InjectedSource : public IDiaInjectedSource {
private:
  DXC_MICROCOM_TM_REF_FIELDS()
  CComPtr<Session> m_pSession;
  DWORD m_index;

public:
  DXC_MICROCOM_TM_ADDREF_RELEASE_IMPL()
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **ppvObject) {
    return DoBasicQueryInterface<IDiaInjectedSource>(this, iid, ppvObject);
  }

  InjectedSource(IMalloc *pMalloc, Session *pSession, DWORD index)
    : m_pMalloc(pMalloc), m_pSession(pSession), m_index(index) {}

  llvm37::MDTuple *NameContent();
  llvm37::StringRef Name();
  llvm37::StringRef Content();

  STDMETHODIMP get_crc(
    /* [retval][out] */ DWORD *pRetVal) override { return ENotImpl(); }

  STDMETHODIMP get_length(_Out_ ULONGLONG *pRetVal) override;

  STDMETHODIMP get_filename(BSTR *pRetVal) override;

  STDMETHODIMP get_objectFilename(BSTR *pRetVal) override;

  STDMETHODIMP get_virtualFilename(BSTR *pRetVal) override;

  STDMETHODIMP get_sourceCompression(
    /* [retval][out] */ DWORD *pRetVal) override { return ENotImpl(); }

  STDMETHODIMP get_source(
    /* [in] */ DWORD cbData,
    /* [out] */ DWORD *pcbData,
    /* [size_is][out] */ BYTE *pbData) override;
};

class InjectedSourcesTable : public impl::TableBase<IDiaEnumInjectedSources,
                                                    IDiaInjectedSource> {
public:
  InjectedSourcesTable(IMalloc *pMalloc, Session *pSession);

  HRESULT GetItem(DWORD index, IDiaInjectedSource **ppItem) override;

  void Init(llvm37::StringRef filename);

private:
  std::vector<unsigned> m_indexList;
};

}  // namespace dxil_dia
