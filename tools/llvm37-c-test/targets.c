/*===-- targets.c - tool for testing libLLVM37 and llvm37-c API ---------------===*\
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This file implements the --targets command in llvm37-c-test.                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#include "llvm37-c/TargetMachine.h"
#include <stdio.h>

int targets_list(void) {
  LLVM37TargetRef t;
  LLVM37InitializeAllTargetInfos();
  LLVM37InitializeAllTargets();

  for (t = LLVM37GetFirstTarget(); t; t = LLVM37GetNextTarget(t)) {
    printf("%s", LLVM37GetTargetName(t));
    if (LLVM37TargetHasJIT(t))
      printf(" (+jit)");
    printf("\n - %s\n", LLVM37GetTargetDescription(t));
  }

  return 0;
}
