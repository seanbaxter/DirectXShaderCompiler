//===-- CompileUtils.h - Utilities for compiling IR in the JIT --*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Contains utilities for compiling IR to object files.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_EXECUTIONENGINE_ORC_COMPILEUTILS_H
#define LLVM37_EXECUTIONENGINE_ORC_COMPILEUTILS_H

#include "llvm37/ExecutionEngine/ObjectMemoryBuffer.h"
#include "llvm37/IR/LegacyPassManager.h"
#include "llvm37/MC/MCContext.h"
#include "llvm37/Object/ObjectFile.h"
#include "llvm37/Target/TargetMachine.h"

namespace llvm37 {
namespace orc {

/// @brief Simple compile functor: Takes a single IR module and returns an
///        ObjectFile.
class SimpleCompiler {
public:
  /// @brief Construct a simple compile functor with the given target.
  SimpleCompiler(TargetMachine &TM) : TM(TM) {}

  /// @brief Compile a Module to an ObjectFile.
  object::OwningBinary<object::ObjectFile> operator()(Module &M) const {
    SmallVector<char, 0> ObjBufferSV;
    raw_svector_ostream ObjStream(ObjBufferSV);

    legacy::PassManager PM;
    MCContext *Ctx;
    if (TM.addPassesToEmitMC(PM, Ctx, ObjStream))
      llvm37_unreachable("Target does not support MC emission.");
    PM.run(M);
    ObjStream.flush();
    std::unique_ptr<MemoryBuffer> ObjBuffer(
        new ObjectMemoryBuffer(std::move(ObjBufferSV)));
    ErrorOr<std::unique_ptr<object::ObjectFile>> Obj =
        object::ObjectFile::createObjectFile(ObjBuffer->getMemBufferRef());
    // TODO: Actually report errors helpfully.
    typedef object::OwningBinary<object::ObjectFile> OwningObj;
    if (Obj)
      return OwningObj(std::move(*Obj), std::move(ObjBuffer));
    return OwningObj(nullptr, nullptr);
  }

private:
  TargetMachine &TM;
};

} // End namespace orc.
} // End namespace llvm37.

#endif // LLVM37_EXECUTIONENGINE_ORC_COMPILEUTILS_H
