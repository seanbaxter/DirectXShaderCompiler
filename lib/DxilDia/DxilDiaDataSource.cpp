///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilDiaDataSource.cpp                                                     //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// DIA API implementation for DXIL modules.                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "DxilDiaDataSource.h"

#include "dxc/DxilContainer/DxilContainer.h"
#include "dxc/DXIL/DxilUtil.h"
#include "dxc/DXIL/DxilPDB.h"
#include "dxc/Support/FileIOHelper.h"
#include "dxc/Support/dxcapi.impl.h"

#include "llvm37/Support/MSFileSystem.h"
#include "llvm37/Support/FileSystem.h"

#include "llvm37/Support/MemoryBuffer.h"
#include "llvm37/IR/DebugInfo.h"
#include "llvm37/IR/LLVMContext.h"
#include "llvm37/IR/Module.h"

#include "DxilDiaSession.h"

dxil_dia::DataSource::DataSource(IMalloc *pMalloc) : m_pMalloc(pMalloc) {
}

dxil_dia::DataSource::~DataSource() {
  // These are cross-referenced, so let's be explicit.
  m_finder.reset();
  m_module.reset();
  m_context.reset();
}

STDMETHODIMP dxil_dia::DataSource::get_lastError(BSTR *pRetVal) {
  *pRetVal = nullptr;
  return S_OK;
}

namespace dxil_dia
{
std::unique_ptr<llvm37::MemoryBuffer> getMemBufferFromBlob(_In_ IDxcBlob *pBlob,
                                                         const llvm37::Twine &BufferName) {
  llvm37::StringRef Data((LPSTR)pBlob->GetBufferPointer(), pBlob->GetBufferSize());
  return llvm37::MemoryBuffer::getMemBufferCopy(Data, BufferName);
}

std::unique_ptr<llvm37::MemoryBuffer> getMemBufferFromStream(_In_ IStream *pStream,
                                                           const llvm37::Twine &BufferName) {
  CComPtr<IDxcBlob> pBlob;
  if (SUCCEEDED(pStream->QueryInterface(&pBlob))) {
    return getMemBufferFromBlob(pBlob, BufferName);
  }

  STATSTG statstg;
  IFT(pStream->Stat(&statstg, STATFLAG_NONAME));
  size_t size = statstg.cbSize.LowPart;
  std::unique_ptr<llvm37::MemoryBuffer> result(
    llvm37::MemoryBuffer::getNewUninitMemBuffer(size, BufferName));
  char *pBuffer = (char *)result.get()->getBufferStart();
  ULONG read;
  IFT(pStream->Read(pBuffer, size, &read));
  return result;
}
}  // namespace dxil_dia

STDMETHODIMP dxil_dia::DataSource::loadDataFromIStream(_In_ IStream *pInputIStream) {
  try {
    DxcThreadMalloc TM(m_pMalloc);
    if (m_module.get() != nullptr) {
      return E_FAIL;
    }

    // Setup filesystem because bitcode reader might emit warning
    ::llvm37::sys::fs::MSFileSystem* msfPtr;
    IFT(CreateMSFileSystemForDisk(&msfPtr));
    std::unique_ptr<::llvm37::sys::fs::MSFileSystem> msf(msfPtr);

    ::llvm37::sys::fs::AutoPerThreadSystem pts(msf.get());
    IFTLLVM(pts.error_code());

    CComPtr<IStream> pIStream = pInputIStream;
    CComPtr<IDxcBlob> pContainer;
    if (SUCCEEDED(hlsl::pdb::LoadDataFromStream(m_pMalloc, pInputIStream, &pContainer))) {
      const hlsl::DxilContainerHeader *pContainerHeader = 
        hlsl::IsDxilContainerLike(pContainer->GetBufferPointer(), pContainer->GetBufferSize());
      if (!hlsl::IsValidDxilContainer(pContainerHeader, pContainer->GetBufferSize()))
        return E_FAIL;
      const hlsl::DxilPartHeader *PartHeader =
        hlsl::GetDxilPartByType(pContainerHeader, hlsl::DFCC_ShaderDebugInfoDXIL);
      if (!PartHeader)
        return E_FAIL;
      CComPtr<IDxcBlobEncoding> pPinnedBlob;
      IFR(hlsl::DxcCreateBlobWithEncodingFromPinned(PartHeader+1, PartHeader->PartSize, CP_ACP, &pPinnedBlob));
      pIStream.Release();
      IFR(hlsl::CreateReadOnlyBlobStream(pPinnedBlob, &pIStream));
    }

    m_context.reset();
    m_finder.reset();

    m_context = std::make_shared<llvm37::LLVMContext>();
    llvm37::MemoryBuffer *pBitcodeBuffer;
    std::unique_ptr<llvm37::MemoryBuffer> pEmbeddedBuffer;
    std::unique_ptr<llvm37::MemoryBuffer> pBuffer =
      getMemBufferFromStream(pIStream, "data");
    size_t bufferSize = pBuffer->getBufferSize();

    // The buffer can hold LLVM bitcode for a module, or the ILDB
    // part from a container.
    if (bufferSize < sizeof(UINT32)) {
      return DXC_E_MALFORMED_CONTAINER;
    }
    const UINT32 BC_C0DE = ((INT32)(INT8)'B' | (INT32)(INT8)'C' << 8 | (INT32)0xDEC0 << 16); // BC0xc0de in big endian
    if (BC_C0DE == *(const UINT32*)pBuffer->getBufferStart()) {
      pBitcodeBuffer = pBuffer.get();
    } else {
      if (bufferSize <= sizeof(hlsl::DxilProgramHeader)) {
        return DXC_E_MALFORMED_CONTAINER;
      }

      hlsl::DxilProgramHeader *pDxilProgramHeader = (hlsl::DxilProgramHeader *)pBuffer->getBufferStart();
      if (pDxilProgramHeader->BitcodeHeader.DxilMagic != hlsl::DxilMagicValue) {
        return DXC_E_MALFORMED_CONTAINER;
      }

      UINT32 BlobSize;
      const char *pBitcode = nullptr;
      hlsl::GetDxilProgramBitcode(pDxilProgramHeader, &pBitcode, &BlobSize);
      std::unique_ptr<llvm37::MemoryBuffer> p = llvm37::MemoryBuffer::getMemBuffer(
        llvm37::StringRef(pBitcode, BlobSize), "data", false /* RequiresNullTerminator */);
      pEmbeddedBuffer.swap(p);
      pBitcodeBuffer = pEmbeddedBuffer.get();
    }

    std::string DiagStr;
    std::unique_ptr<llvm37::Module> pModule = hlsl::dxilutil::LoadModuleFromBitcode(
      pBitcodeBuffer, *m_context.get(), DiagStr);
    if (!pModule.get())
      return E_FAIL;
    m_finder = std::make_shared<llvm37::DebugInfoFinder>();
    m_finder->processModule(*pModule.get());
    m_module.reset(pModule.release());
  }
  CATCH_CPP_RETURN_HRESULT();
  return S_OK;
}

STDMETHODIMP dxil_dia::DataSource::openSession(_COM_Outptr_ IDiaSession **ppSession) {
  DxcThreadMalloc TM(m_pMalloc);
  *ppSession = nullptr;
  if (m_module.get() == nullptr)
    return E_FAIL;

  ::llvm37::sys::fs::MSFileSystem *msfPtr;
  IFT(CreateMSFileSystemForDisk(&msfPtr));
  std::unique_ptr<::llvm37::sys::fs::MSFileSystem> msf(msfPtr);

  ::llvm37::sys::fs::AutoPerThreadSystem pts(msf.get());
  IFTLLVM(pts.error_code());

  CComPtr<Session> pSession = Session::Alloc(DxcGetThreadMallocNoRef());
  IFROOM(pSession.p);
  pSession->Init(m_context, m_module, m_finder);
  *ppSession = pSession.Detach();
  return S_OK;
}


HRESULT CreateDxcDiaDataSource(_In_ REFIID riid, _Out_ LPVOID* ppv) {
  CComPtr<dxil_dia::DataSource> result = CreateOnMalloc<dxil_dia::DataSource>(DxcGetThreadMallocNoRef());
  if (result == nullptr) {
    *ppv = nullptr;
    return E_OUTOFMEMORY;
  }

  return result.p->QueryInterface(riid, ppv);
}
