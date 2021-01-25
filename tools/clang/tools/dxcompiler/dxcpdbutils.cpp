///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// dxilpdbutils.cpp                                                          //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// Implements IDxcPdbUtils interface, which allows getting basic stuff from  //
// shader PDBS.                                                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32

#include "dxc/Support/Global.h"
#include "dxc/Support/WinIncludes.h"
#include "dxc/Support/dxcapi.use.h"
#include "dxc/Support/FileIOHelper.h"
#include "llvm37/Support/MSFileSystem.h"
#include "llvm37/Support/FileSystem.h"
#include "llvm37/Support/MemoryBuffer.h"
#include "llvm37/IR/LLVMContext.h"
#include "llvm37/IR/Module.h"

#include "dxc/dxcapi.h"
#include "dxc/dxcpix.h"
#include "dxc/Support/microcom.h"
#include "dxc/DxilContainer/DxilContainer.h"
#include "dxc/DXIL/DxilUtil.h"
#include "dxc/DXIL/DxilPDB.h"
#include "dxc/DXIL/DxilMetadataHelper.h"
#include "dxc/DXIL/DxilModule.h"

#include <vector>
#include <locale>
#include <codecvt>
#include <string>
#include <dia2.h>

using namespace dxc;
using namespace llvm37;

static std::wstring ToWstring(const char *ptr, size_t size) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t> > converter;
  return converter.from_bytes(ptr, ptr+size);
}
static std::wstring ToWstring(const char *ptr) {
  return ToWstring(ptr, strlen(ptr));
}
static std::wstring ToWstring(const std::string &str) {
  return ToWstring(str.data(), str.size());
}
static std::wstring ToWstring(StringRef str) {
  return ToWstring(str.data(), str.size());
}
static HRESULT CopyWstringToBSTR(const std::wstring &str, BSTR *pResult) {
  if (!pResult) return E_POINTER;
  *pResult = nullptr;

  if (str.empty())
    return S_OK;

  CComBSTR buf(str.data());
  *pResult = buf.Detach();
  return S_OK;
}

static bool ShouldIncludeInFlags(StringRef strRef, bool *skip_next_arg) {
  *skip_next_arg = false;
  const char *specialCases[] = { "/T", "-T", "-D", "/D", "-E", "/E", };
  for (unsigned i = 0; i < _countof(specialCases); i++) {
    if (strRef == specialCases[i]) {
      *skip_next_arg = true;
      return false;
    }
    else if (strRef.startswith(specialCases[i])) {
      return false;
    }
  }
  return true;
}

struct DxcPdbUtils : public IDxcPdbUtils, public IDxcPixDxilDebugInfoFactory
{
private:
  DXC_MICROCOM_TM_REF_FIELDS()

  struct Source_File {
    std::wstring Name;
    CComPtr<IDxcBlobEncoding> Content;
  };

  CComPtr<IDxcBlob> m_InputBlob;
  CComPtr<IDxcBlob> m_pDebugProgramBlob;
  CComPtr<IDxcBlob> m_ContainerBlob;
  std::vector<Source_File> m_SourceFiles;
  std::vector<std::wstring> m_Defines;
  std::vector<std::wstring> m_Args;
  std::vector<std::wstring> m_Flags;
  std::wstring m_EntryPoint;
  std::wstring m_TargetProfile;
  std::wstring m_Name;
  std::wstring m_MainFileName;
  CComPtr<IDxcBlob> m_HashBlob;

  void Reset() {
    m_pDebugProgramBlob = nullptr;
    m_InputBlob = nullptr;
    m_ContainerBlob = nullptr;
    m_SourceFiles.clear();
    m_Defines.clear();
    m_Args.clear();
    m_Flags.clear();
    m_EntryPoint.clear();
    m_TargetProfile.clear();
    m_Name.clear();
    m_MainFileName.clear();
    m_HashBlob = nullptr;
  }

  HRESULT HandleDebugProgramHeaderLegacy(IDxcBlob *pProgramBlob) {
    UINT32 bitcode_size = 0;
    const char *bitcode = nullptr;

    if (!hlsl::IsValidDxilProgramHeader((hlsl::DxilProgramHeader *)pProgramBlob->GetBufferPointer(), pProgramBlob->GetBufferSize()))
      return E_FAIL;

    hlsl::GetDxilProgramBitcode((hlsl::DxilProgramHeader *)pProgramBlob->GetBufferPointer(), &bitcode, &bitcode_size);

    llvm37::LLVMContext context;
    std::unique_ptr<llvm37::Module> pModule;

    // NOTE: this doesn't copy the memory, just references it.
    std::unique_ptr<llvm37::MemoryBuffer> mb = llvm37::MemoryBuffer::getMemBuffer(StringRef(bitcode, bitcode_size), "-", /*RequiresNullTerminator*/ false);

    // Lazily parse the module
    std::string DiagStr;
    pModule = hlsl::dxilutil::LoadModuleFromBitcodeLazy(std::move(mb), context, DiagStr);
    if (!pModule)
      return E_FAIL;

    // Materialize only the stuff we need, so it's fast
    {
      llvm37::StringRef DebugMetadataList[] = {
        hlsl::DxilMDHelper::kDxilSourceContentsMDName,
        hlsl::DxilMDHelper::kDxilSourceDefinesMDName,
        hlsl::DxilMDHelper::kDxilSourceArgsMDName,
        hlsl::DxilMDHelper::kDxilVersionMDName,
        hlsl::DxilMDHelper::kDxilShaderModelMDName,
        hlsl::DxilMDHelper::kDxilEntryPointsMDName,
        hlsl::DxilMDHelper::kDxilSourceMainFileNameMDName,
      };
      pModule->materializeSelectNamedMetadata(DebugMetadataList);
    }

    hlsl::DxilModule &DM = pModule->GetOrCreateDxilModule();
    m_EntryPoint = ToWstring(DM.GetEntryFunctionName());
    m_TargetProfile = ToWstring(DM.GetShaderModel()->GetName());

    // For each all the named metadata node in the module
    for (llvm37::NamedMDNode &node : pModule->named_metadata()) {
      llvm37::StringRef node_name = node.getName();

      // dx.source.content
      if (node_name == hlsl::DxilMDHelper::kDxilSourceContentsMDName ||
          node_name == hlsl::DxilMDHelper::kDxilSourceContentsOldMDName)
      {
        for (unsigned i = 0; i < node.getNumOperands(); i++) {
          llvm37::MDTuple *tup = cast<llvm37::MDTuple>(node.getOperand(i));
          MDString *md_name = cast<MDString>(tup->getOperand(0));
          MDString *md_content = cast<MDString>(tup->getOperand(1));

          // File name
          Source_File file;
          file.Name = ToWstring(md_name->getString());

          // File content
          IFR(hlsl::DxcCreateBlobWithEncodingOnHeapCopy(
            md_content->getString().data(),
            md_content->getString().size(),
            CP_ACP, // NOTE: ACP instead of UTF8 because it's possible for compiler implementations to
                    // inject non-UTF8 data here.
            &file.Content));

          m_SourceFiles.push_back(std::move(file));
        }
      }
      // dx.source.defines
      else if (node_name == hlsl::DxilMDHelper::kDxilSourceDefinesMDName ||
               node_name == hlsl::DxilMDHelper::kDxilSourceDefinesOldMDName)
      {
        MDTuple *tup = cast<MDTuple>(node.getOperand(0));
        for (unsigned i = 0; i < tup->getNumOperands(); i++) {
          StringRef define = cast<MDString>(tup->getOperand(i))->getString();
          m_Defines.push_back(ToWstring(define));
        }
      }
      // dx.source.mainFileName
      else if (node_name == hlsl::DxilMDHelper::kDxilSourceMainFileNameMDName ||
               node_name == hlsl::DxilMDHelper::kDxilSourceMainFileNameOldMDName)
      {
        MDTuple *tup = cast<MDTuple>(node.getOperand(0));
        MDString *str = cast<MDString>(tup->getOperand(0));
        m_MainFileName = ToWstring(str->getString());
      }
      // dx.source.args
      else if (node_name == hlsl::DxilMDHelper::kDxilSourceArgsMDName ||
               node_name == hlsl::DxilMDHelper::kDxilSourceArgsOldMDName)
      {
        MDTuple *tup = cast<MDTuple>(node.getOperand(0));
        // Args
        for (unsigned i = 0; i < tup->getNumOperands(); i++) {
          StringRef arg = cast<MDString>(tup->getOperand(i))->getString();
          m_Args.push_back(ToWstring(arg));
        }

        // Flags - which exclude entry point, target profile, and defines
        for (unsigned i = 0; i < tup->getNumOperands(); i++) {
          StringRef arg = cast<MDString>(tup->getOperand(i))->getString();
          bool skip_another_arg = false;
          if (ShouldIncludeInFlags(arg, &skip_another_arg)) {
            m_Flags.push_back(ToWstring(arg));
          }
          if (skip_another_arg)
            i++;
        }
      }
    }

    return S_OK;
  }

  HRESULT HandleDxilContainer(IDxcBlob *pContainer, IDxcBlob **ppDebugProgramBlob) {
    const hlsl::DxilContainerHeader *header = (hlsl::DxilContainerHeader *)m_ContainerBlob->GetBufferPointer();
    for (auto it = hlsl::begin(header); it != hlsl::end(header); it++) {
      const hlsl::DxilPartHeader *part = *it;
      hlsl::DxilFourCC four_cc = (hlsl::DxilFourCC)part->PartFourCC;

      switch (four_cc) {

      case hlsl::DFCC_ShaderHash:
      {
        const hlsl::DxilShaderHash *hash_header = (hlsl::DxilShaderHash *)(part+1);
        IFR(hlsl::DxcCreateBlobOnHeapCopy(hash_header, sizeof(*hash_header), &m_HashBlob));
      } break;

      case hlsl::DFCC_ShaderDebugName:
      {
        const hlsl::DxilShaderDebugName *name_header = (hlsl::DxilShaderDebugName *)(part+1);
        const char *ptr = (char *)(name_header+1);
        m_Name = ToWstring(ptr, name_header->NameLength);
      } break;

      case hlsl::DFCC_ShaderDebugInfoDXIL:
      {
        hlsl::DxilProgramHeader *program_header = (hlsl::DxilProgramHeader *)(part+1);

        CComPtr<IDxcBlobEncoding> pProgramHeaderBlob;
        IFR(hlsl::DxcCreateBlobWithEncodingFromPinned(program_header, program_header->SizeInUint32*sizeof(UINT32), CP_ACP, &pProgramHeaderBlob));
        IFR(pProgramHeaderBlob.QueryInterface(ppDebugProgramBlob));

      } break; // hlsl::DFCC_ShaderDebugInfoDXIL
      } // switch (four_cc)
    } // For each part

    return S_OK;
  }

public:
  DXC_MICROCOM_TM_ADDREF_RELEASE_IMPL()
  DXC_MICROCOM_TM_ALLOC(DxcPdbUtils)

  DxcPdbUtils(IMalloc *pMalloc) : m_dwRef(0), m_pMalloc(pMalloc) {}

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **ppvObject) override {
    return DoBasicQueryInterface<IDxcPdbUtils, IDxcPixDxilDebugInfoFactory>(this, iid, ppvObject);
  }

  HRESULT STDMETHODCALLTYPE Load(_In_ IDxcBlob *pPdbOrDxil) override {
    DxcThreadMalloc TM(m_pMalloc);

    ::llvm37::sys::fs::MSFileSystem *msfPtr = nullptr;
    IFT(CreateMSFileSystemForDisk(&msfPtr));
    std::unique_ptr<::llvm37::sys::fs::MSFileSystem> msf(msfPtr);
  
    ::llvm37::sys::fs::AutoPerThreadSystem pts(msf.get());
    IFTLLVM(pts.error_code());

    if (!pPdbOrDxil)
      return E_POINTER;

    // Remove all the data
    Reset();

    m_InputBlob = pPdbOrDxil;

    // Right now, what we do here is just to support the current PDB formats.
    {
      CComPtr<IStream> pStream;
      IFR(hlsl::CreateReadOnlyBlobStream(pPdbOrDxil, &pStream));

      // PDB
      if (SUCCEEDED(hlsl::pdb::LoadDataFromStream(m_pMalloc, pStream, &m_ContainerBlob))) {
        IFR(HandleDxilContainer(m_ContainerBlob, &m_pDebugProgramBlob));
        if (m_pDebugProgramBlob) {
          IFR(HandleDebugProgramHeaderLegacy(m_pDebugProgramBlob));
        }
        else {
          // Must have a debug program part
          return E_FAIL;
        }
      }
      // DXIL Container
      else if (hlsl::IsValidDxilContainer((const hlsl::DxilContainerHeader *)pPdbOrDxil->GetBufferPointer(), pPdbOrDxil->GetBufferSize())) {
        m_ContainerBlob = pPdbOrDxil;
        IFR(HandleDxilContainer(m_ContainerBlob, &m_pDebugProgramBlob));
        // If we have a Debug DXIL, populate the debug info.
        if (m_pDebugProgramBlob) {
          IFR(HandleDebugProgramHeaderLegacy(m_pDebugProgramBlob));
        }
      }
      // DXIL program header
      else if (hlsl::IsValidDxilProgramHeader((hlsl::DxilProgramHeader *)pPdbOrDxil->GetBufferPointer(), pPdbOrDxil->GetBufferSize())) {
        CComPtr<IDxcBlobEncoding> pProgramHeaderBlob;
        IFR(hlsl::DxcCreateBlobWithEncodingFromPinned(
          (hlsl::DxilProgramHeader *)pPdbOrDxil->GetBufferPointer(),
          pPdbOrDxil->GetBufferSize(), CP_ACP, &pProgramHeaderBlob));
        IFR(pProgramHeaderBlob.QueryInterface(&m_pDebugProgramBlob));
        IFR(HandleDebugProgramHeaderLegacy(m_pDebugProgramBlob));
      }
      else {
        return E_INVALIDARG;
      }
    }


    return S_OK;
  }

  virtual HRESULT STDMETHODCALLTYPE GetSourceCount(_Out_ UINT32 *pCount) override {
    if (!pCount) return E_POINTER;
    *pCount = (UINT32)m_SourceFiles.size();
    return S_OK;
  }

  virtual HRESULT STDMETHODCALLTYPE GetSource(_In_ UINT32 uIndex, _COM_Outptr_ IDxcBlobEncoding **ppResult) override {
    if (uIndex >= m_SourceFiles.size()) return E_INVALIDARG;
    if (!ppResult) return E_POINTER;
    *ppResult = nullptr;
    return m_SourceFiles[uIndex].Content.QueryInterface(ppResult);
  }

  virtual HRESULT STDMETHODCALLTYPE GetSourceName(_In_ UINT32 uIndex, _Outptr_result_z_ BSTR *pResult) override {
    if (uIndex >= m_SourceFiles.size()) return E_INVALIDARG;
    return CopyWstringToBSTR(m_SourceFiles[uIndex].Name, pResult);
  }

  static inline HRESULT GetStringCount(const std::vector<std::wstring> &list, _Out_ UINT32 *pCount) {
    if (!pCount) return E_POINTER;
    *pCount = (UINT32)list.size();
    return S_OK;
  }

  static inline HRESULT GetStringOption(const std::vector<std::wstring> &list, _In_ UINT32 uIndex, _Outptr_result_z_ BSTR *pResult) {
    if (uIndex >= list.size()) return E_INVALIDARG;
    return CopyWstringToBSTR(list[uIndex], pResult);
  }

  virtual HRESULT STDMETHODCALLTYPE GetFlagCount(_Out_ UINT32 *pCount) override {  return GetStringCount(m_Flags, pCount); }
  virtual HRESULT STDMETHODCALLTYPE GetFlag(_In_ UINT32 uIndex, _Outptr_result_z_ BSTR *pResult) override { return GetStringOption(m_Flags, uIndex, pResult); }
  virtual HRESULT STDMETHODCALLTYPE GetArgCount(_Out_ UINT32 *pCount) override { return GetStringCount(m_Args, pCount); }
  virtual HRESULT STDMETHODCALLTYPE GetArg(_In_ UINT32 uIndex, _Outptr_result_z_ BSTR *pResult) override { return GetStringOption(m_Args, uIndex, pResult); }
  virtual HRESULT STDMETHODCALLTYPE GetDefineCount(_Out_ UINT32 *pCount) override { return GetStringCount(m_Defines, pCount); }
  virtual HRESULT STDMETHODCALLTYPE GetDefine(_In_ UINT32 uIndex, _Outptr_result_z_ BSTR *pResult) override { return GetStringOption(m_Defines, uIndex, pResult); }

  virtual HRESULT STDMETHODCALLTYPE GetTargetProfile(_Outptr_result_z_ BSTR *pResult) override {
    return CopyWstringToBSTR(m_TargetProfile, pResult);
  }
  virtual HRESULT STDMETHODCALLTYPE GetEntryPoint(_Outptr_result_z_ BSTR *pResult) override {
    return CopyWstringToBSTR(m_EntryPoint, pResult);
  }
  virtual HRESULT STDMETHODCALLTYPE GetMainFileName(_Outptr_result_z_ BSTR *pResult) {
    return CopyWstringToBSTR(m_MainFileName, pResult);
  }

  virtual BOOL STDMETHODCALLTYPE IsFullPDB() override {
    return m_pDebugProgramBlob != nullptr;
  }

  virtual HRESULT STDMETHODCALLTYPE GetFullPDB(_COM_Outptr_ IDxcBlob **ppFullPDB) override {
    if (!m_InputBlob)
      return E_FAIL;
    if (!ppFullPDB) return E_POINTER;
    *ppFullPDB = nullptr;
    return m_InputBlob.QueryInterface(ppFullPDB);
  }

  virtual HRESULT STDMETHODCALLTYPE GetHash(_COM_Outptr_ IDxcBlob **ppResult) override {
    if (!ppResult) return E_POINTER;
    *ppResult = nullptr;
    if (m_HashBlob)
      return m_HashBlob.QueryInterface(ppResult);
    return E_FAIL;
  }

  virtual HRESULT STDMETHODCALLTYPE GetName(_Outptr_result_z_ BSTR *pResult) override {
    return CopyWstringToBSTR(m_Name, pResult);
  }

  virtual STDMETHODIMP NewDxcPixDxilDebugInfo(
      _COM_Outptr_ IDxcPixDxilDebugInfo **ppDxilDebugInfo) override
  {
    if (!m_pDebugProgramBlob)
      return E_FAIL;

    DxcThreadMalloc TM(m_pMalloc);

    CComPtr<IDiaDataSource> pDataSource;
    IFR(DxcCreateInstance2(m_pMalloc, CLSID_DxcDiaDataSource, __uuidof(IDiaDataSource),
      (void **)&pDataSource));

    CComPtr<IStream> pStream;
    IFR(hlsl::CreateReadOnlyBlobStream(m_pDebugProgramBlob, &pStream));

    IFR(pDataSource->loadDataFromIStream(pStream));

    CComPtr<IDiaSession> pSession;
    IFR(pDataSource->openSession(&pSession));

    CComPtr<IDxcPixDxilDebugInfoFactory> pFactory;
    IFR(pSession.QueryInterface(&pFactory));

    return pFactory->NewDxcPixDxilDebugInfo(ppDxilDebugInfo);
  }

  virtual STDMETHODIMP NewDxcPixCompilationInfo(
      _COM_Outptr_ IDxcPixCompilationInfo **ppCompilationInfo) override
  {
    return E_NOTIMPL;
  }
};

HRESULT CreateDxcPdbUtils(_In_ REFIID riid, _Out_ LPVOID *ppv) {
  CComPtr<DxcPdbUtils> result = CreateOnMalloc<DxcPdbUtils>(DxcGetThreadMallocNoRef());
  if (result == nullptr) {
    *ppv = nullptr;
    return E_OUTOFMEMORY;
  }
  return result.p->QueryInterface(riid, ppv);
}

#else

#include "dxc/Support/WinIncludes.h"

HRESULT CreateDxcPdbUtils(_In_ REFIID riid, _Out_ LPVOID *ppv) {
  return E_NOTIMPL;
}

#endif


