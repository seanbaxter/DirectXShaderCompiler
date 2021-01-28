/*===-- object.c - tool for testing libLLVM37 and llvm37-c API ----------------===*\
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This file implements the --add-named-metadata-operand and --set-metadata   *|
|* commands in llvm37-c-test.                                                   *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#include "llvm37-c-test.h"
#include "llvm37-c/Core.h"

int add_named_metadata_operand(void) {
  LLVM37ModuleRef m = LLVM37ModuleCreateWithName("Mod");
  LLVM37ValueRef values[] = { LLVM37ConstInt(LLVM37Int32Type(), 0, 0) };

  // This used to trigger an assertion
  LLVM37AddNamedMetadataOperand(m, "name", LLVM37MDNode(values, 1));

  LLVM37DisposeModule(m);

  return 0;
}

int set_metadata(void) {
  LLVMBuilderRef b = LLVM37CreateBuilder();
  LLVM37ValueRef values[] = { LLVM37ConstInt(LLVM37Int32Type(), 0, 0) };

  // This used to trigger an assertion
  LLVM37SetMetadata(
      LLVMBuildRetVoid(b),
      LLVM37GetMDKindID("kind", 4),
      LLVM37MDNode(values, 1));

  LLVM37DisposeBuilder(b);

  return 0;
}
