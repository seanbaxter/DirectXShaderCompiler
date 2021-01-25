//===-- OProfileJITEventListener.cpp - Tell OProfile about JITted code ----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a JITEventListener object that uses OProfileWrapper to tell
// oprofile about JITted functions, including source line information.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Config/config.h"
#include "llvm37/CodeGen/MachineFunction.h"
#include "llvm37/ExecutionEngine/JITEventListener.h"
#include "llvm37/ExecutionEngine/OProfileWrapper.h"
#include "llvm37/ExecutionEngine/RuntimeDyld.h"
#include "llvm37/IR/DebugInfo.h"
#include "llvm37/IR/Function.h"
#include "llvm37/Object/ObjectFile.h"
#include "llvm37/Object/SymbolSize.h"
#include "llvm37/Support/Debug.h"
#include "llvm37/Support/Errno.h"
#include "llvm37/Support/raw_ostream.h"
#include <dirent.h>
#include <fcntl.h>

using namespace llvm37;
using namespace llvm37::object;

#define DEBUG_TYPE "oprofile-jit-event-listener"

namespace {

class OProfileJITEventListener : public JITEventListener {
  std::unique_ptr<OProfileWrapper> Wrapper;

  void initialize();
  std::map<const char*, OwningBinary<ObjectFile>> DebugObjects;

public:
  OProfileJITEventListener(std::unique_ptr<OProfileWrapper> LibraryWrapper)
    : Wrapper(std::move(LibraryWrapper)) {
    initialize();
  }

  ~OProfileJITEventListener();

  void NotifyObjectEmitted(const ObjectFile &Obj,
                           const RuntimeDyld::LoadedObjectInfo &L) override;

  void NotifyFreeingObject(const ObjectFile &Obj) override;
};

void OProfileJITEventListener::initialize() {
  if (!Wrapper->op_open_agent()) {
    const std::string err_str = sys::StrError();
    DEBUG(dbgs() << "Failed to connect to OProfile agent: " << err_str << "\n");
  } else {
    DEBUG(dbgs() << "Connected to OProfile agent.\n");
  }
}

OProfileJITEventListener::~OProfileJITEventListener() {
  if (Wrapper->isAgentAvailable()) {
    if (Wrapper->op_close_agent() == -1) {
      const std::string err_str = sys::StrError();
      DEBUG(dbgs() << "Failed to disconnect from OProfile agent: "
                   << err_str << "\n");
    } else {
      DEBUG(dbgs() << "Disconnected from OProfile agent.\n");
    }
  }
}

void OProfileJITEventListener::NotifyObjectEmitted(
                                       const ObjectFile &Obj,
                                       const RuntimeDyld::LoadedObjectInfo &L) {
  if (!Wrapper->isAgentAvailable()) {
    return;
  }

  OwningBinary<ObjectFile> DebugObjOwner = L.getObjectForDebug(Obj);
  const ObjectFile &DebugObj = *DebugObjOwner.getBinary();

  // Use symbol info to iterate functions in the object.
  for (const std::pair<SymbolRef, uint64_t> &P : computeSymbolSizes(DebugObj)) {
    SymbolRef Sym = P.first;
    if (Sym.getType() != SymbolRef::ST_Function)
      continue;

    ErrorOr<StringRef> NameOrErr = Sym.getName();
    if (NameOrErr.getError())
      continue;
    StringRef Name = *NameOrErr;
    ErrorOr<uint64_t> AddrOrErr = Sym.getAddress();
    if (AddrOrErr.getError())
      continue;
    uint64_t Addr = *AddrOrErr;
    uint64_t Size = P.second;

    if (Wrapper->op_write_native_code(Name.data(), Addr, (void *)Addr, Size) ==
        -1) {
      DEBUG(dbgs() << "Failed to tell OProfile about native function " << Name
                   << " at [" << (void *)Addr << "-" << ((char *)Addr + Size)
                   << "]\n");
      continue;
    }
    // TODO: support line number info (similar to IntelJITEventListener.cpp)
  }

  DebugObjects[Obj.getData().data()] = std::move(DebugObjOwner);
}

void OProfileJITEventListener::NotifyFreeingObject(const ObjectFile &Obj) {
  if (Wrapper->isAgentAvailable()) {

    // If there was no agent registered when the original object was loaded then
    // we won't have created a debug object for it, so bail out.
    if (DebugObjects.find(Obj.getData().data()) == DebugObjects.end())
      return;

    const ObjectFile &DebugObj = *DebugObjects[Obj.getData().data()].getBinary();

    // Use symbol info to iterate functions in the object.
    for (symbol_iterator I = DebugObj.symbol_begin(),
                         E = DebugObj.symbol_end();
         I != E; ++I) {
      if (I->getType() == SymbolRef::ST_Function) {
        ErrorOr<uint64_t> AddrOrErr = I->getAddress();
        if (AddrOrErr.getError())
          continue;
        uint64_t Addr = *AddrOrErr;

        if (Wrapper->op_unload_native_code(Addr) == -1) {
          DEBUG(dbgs()
                << "Failed to tell OProfile about unload of native function at "
                << (void*)Addr << "\n");
          continue;
        }
      }
    }
  }

  DebugObjects.erase(Obj.getData().data());
}

}  // anonymous namespace.

namespace llvm37 {
JITEventListener *JITEventListener::createOProfileJITEventListener() {
  return new OProfileJITEventListener(llvm37::make_unique<OProfileWrapper>());
}

} // namespace llvm37

