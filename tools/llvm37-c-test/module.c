/*===-- module.c - tool for testing libLLVM37 and llvm37-c API ----------------===*\
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This file implements the --module-dump, --module-list-functions and        *|
|* --module-list-globals commands in llvm37-c-test.                             *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#include "llvm37-c-test.h"
#include "llvm37-c/BitReader.h"
#include "llvm37-c/Core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static LLVM37ModuleRef load_module(void) {
  LLVM37MemoryBufferRef MB;
  LLVM37ModuleRef M;
  char *msg = NULL;

  if (LLVM37CreateMemoryBufferWithSTDIN(&MB, &msg)) {
    fprintf(stderr, "Error reading file: %s\n", msg);
    exit(1);
  }

  if (LLVM37ParseBitcode(MB, &M, &msg)) {
    fprintf(stderr, "Error parsing bitcode: %s\n", msg);
    LLVM37DisposeMemoryBuffer(MB);
    exit(1);
  }

  LLVM37DisposeMemoryBuffer(MB);
  return M;
}

int module_dump(void) {
  LLVM37ModuleRef M = load_module();

  char *irstr = LLVM37PrintModuleToString(M);
  puts(irstr);
  LLVM37DisposeMessage(irstr);

  LLVM37DisposeModule(M);

  return 0;
}

int module_list_functions(void) {
  LLVM37ModuleRef M = load_module();
  LLVM37ValueRef f;

  f = LLVM37GetFirstFunction(M);
  while (f) {
    if (LLVM37IsDeclaration(f)) {
      printf("FunctionDeclaration: %s\n", LLVM37GetValueName(f));
    } else {
      LLVM37BasicBlockRef bb;
      LLVM37ValueRef isn;
      unsigned nisn = 0;
      unsigned nbb = 0;

      printf("FunctionDefinition: %s [#bb=%u]\n", LLVM37GetValueName(f),
             LLVM37CountBasicBlocks(f));

      for (bb = LLVM37GetFirstBasicBlock(f); bb;
           bb = LLVM37GetNextBasicBlock(bb)) {
        nbb++;
        for (isn = LLVM37GetFirstInstruction(bb); isn;
             isn = LLVM37GetNextInstruction(isn)) {
          nisn++;
          if (LLVM37IsACallInst(isn)) {
            LLVM37ValueRef callee =
                LLVM37GetOperand(isn, LLVM37GetNumOperands(isn) - 1);
            printf(" calls: %s\n", LLVM37GetValueName(callee));
          }
        }
      }
      printf(" #isn: %u\n", nisn);
      printf(" #bb: %u\n\n", nbb);
    }
    f = LLVM37GetNextFunction(f);
  }

  LLVM37DisposeModule(M);

  return 0;
}

int module_list_globals(void) {
  LLVM37ModuleRef M = load_module();
  LLVM37ValueRef g;

  g = LLVM37GetFirstGlobal(M);
  while (g) {
    LLVM37TypeRef T = LLVM37TypeOf(g);
    char *s = LLVM37PrintTypeToString(T);

    printf("Global%s: %s %s\n",
           LLVM37IsDeclaration(g) ? "Declaration" : "Definition",
           LLVM37GetValueName(g), s);

    LLVM37DisposeMessage(s);

    g = LLVM37GetNextGlobal(g);
  }

  LLVM37DisposeModule(M);

  return 0;
}
