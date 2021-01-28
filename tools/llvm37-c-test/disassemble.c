/*===-- disassemble.c - tool for testing libLLVM37 and llvm37-c API -----------===*\
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This file implements the --disassemble command in llvm37-c-test.             *|
|* --disassemble reads lines from stdin, parses them as a triple and hex      *|
|*  machine code, and prints disassembly of the machine code.                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#include "llvm37-c-test.h"
#include "llvm37-c/Disassembler.h"
#include "llvm37-c/Target.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void pprint(int pos, unsigned char *buf, int len, const char *disasm) {
  int i;
  printf("%04x:  ", pos);
  for (i = 0; i < 8; i++) {
    if (i < len) {
      printf("%02x ", buf[i]);
    } else {
      printf("   ");
    }
  }

  printf("   %s\n", disasm);
}

static void do_disassemble(const char *triple, const char *features,
                           unsigned char *buf, int siz) {
  LLVM37DisasmContextRef D = LLVM37CreateDisasmCPUFeatures(triple, "", features,
                                                       NULL, 0, NULL, NULL);
  char outline[1024];
  int pos;

  if (!D) {
    printf("ERROR: Couldn't create disassembler for triple %s\n", triple);
    return;
  }

  pos = 0;
  while (pos < siz) {
    size_t l = LLVM37DisasmInstruction(D, buf + pos, siz - pos, 0, outline,
                                     sizeof(outline));
    if (!l) {
      pprint(pos, buf + pos, 1, "\t???");
      pos++;
    } else {
      pprint(pos, buf + pos, l, outline);
      pos += l;
    }
  }

  LLVM37DisasmDispose(D);
}

static void handle_line(char **tokens, int ntokens) {
  unsigned char disbuf[128];
  size_t disbuflen = 0;
  const char *triple = tokens[0];
  const char *features = tokens[1];
  int i;

  printf("triple: %s, features: %s\n", triple, features);
  if (!strcmp(features, "NULL"))
    features = "";

  for (i = 2; i < ntokens; i++) {
    disbuf[disbuflen++] = strtol(tokens[i], NULL, 16);
    if (disbuflen >= sizeof(disbuf)) {
      fprintf(stderr, "Warning: Too long line, truncating\n");
      break;
    }
  }
  do_disassemble(triple, features, disbuf, disbuflen);
}

int disassemble(void) {
  LLVM37InitializeAllTargetInfos();
  LLVM37InitializeAllTargetMCs();
  LLVM37InitializeAllDisassemblers();

  tokenize_stdin(handle_line);

  return 0;
}
