///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// HLOperationLowerExtension.h                                                //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// Functions to lower HL operations coming from HLSL extensions to DXIL      //
// operations.                                                               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "dxc/HLSL/HLSLExtensionsCodegenHelper.h"
#include "llvm37/ADT/StringRef.h"
#include <string>
#include <unordered_map>

namespace llvm37 {
  class Value;
  class CallInst;
  class Function;
  class StringRef;
  class Instruction;
}

namespace hlsl {
  class OP;

  struct HLResourceLookup 
  {
      // Lookup resource kind based on handle. Return true on success.
      virtual bool GetResourceKindName(llvm37::Value *HLHandle, const char **ppName) = 0;
      virtual ~HLResourceLookup() {}
  };

  // Lowers HLSL extensions from HL operation to DXIL operation.
  class ExtensionLowering {
  public:
    // Strategy used for lowering extensions.
    enum class Strategy {
      Unknown,        // Do not know how to lower. This is an error condition.
      NoTranslation,  // Propagate the call arguments as is down to dxil.
      Replicate,      // Scalarize the vector arguments and replicate the call.
      Pack,           // Convert the vector arguments into structs.
      Resource,       // Convert return value to resource return and explode vectors.
      Dxil,           // Convert call to a dxil intrinsic.
    };

    // Create the lowering using the given strategy and custom codegen helper.
    ExtensionLowering(llvm37::StringRef strategy, HLSLExtensionsCodegenHelper *helper, OP& hlslOp, HLResourceLookup &resourceHelper);
    ExtensionLowering(Strategy strategy, HLSLExtensionsCodegenHelper *helper, OP& hlslOp, HLResourceLookup &resourceHelper);

    // Translate the HL op call to a DXIL op call.
    // Returns a new value if translation was successful.
    // Returns nullptr if translation failed or made no changes.
    llvm37::Value *Translate(llvm37::CallInst *CI);
    
    // Translate the strategy string to an enum. The strategy string is
    // added as a custom attribute on the high level extension function.
    // It is translated as follows:
    //  "r" -> Replicate
    //  "n" -> NoTranslation
    //  "c" -> Custom
    static Strategy GetStrategy(llvm37::StringRef strategy);

    // Translate the strategy enum into a name. This is the inverse of the
    // GetStrategy() function.
    static llvm37::StringRef GetStrategyName(Strategy strategy);

    // Get the name that will be used for the extension function call after
    // lowering.
    std::string GetExtensionName(llvm37::CallInst *CI);

  private:
    Strategy m_strategy;
    HLSLExtensionsCodegenHelper *m_helper;
    OP &m_hlslOp;
    HLResourceLookup &m_hlResourceLookup;
    std::string m_extraStrategyInfo;

    llvm37::Value *Unknown(llvm37::CallInst *CI);
    llvm37::Value *NoTranslation(llvm37::CallInst *CI);
    llvm37::Value *Replicate(llvm37::CallInst *CI);
    llvm37::Value *Pack(llvm37::CallInst *CI);
    llvm37::Value *Resource(llvm37::CallInst *CI);
    llvm37::Value *Dxil(llvm37::CallInst *CI);
    llvm37::Value *CustomResource(llvm37::CallInst *CI);
  };
}