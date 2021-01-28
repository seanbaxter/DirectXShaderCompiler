/*===-- main.c - tool for testing libLLVM37 and llvm37-c API ------------------===*\
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* Main file for llvm37-c-tests. "Parses" arguments and dispatches.             *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#include "llvm37-c-test.h"
#include "llvm37-c/BitReader.h"
#include "llvm37-c/Core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_usage(void) {
  fprintf(stderr, "llvm37-c-test command\n\n");
  fprintf(stderr, " Commands:\n");
  fprintf(stderr, "  * --module-dump\n");
  fprintf(stderr, "    Read bytecode from stdin - print disassembly\n\n");
  fprintf(stderr, "  * --module-list-functions\n");
  fprintf(stderr,
          "    Read bytecode from stdin - list summary of functions\n\n");
  fprintf(stderr, "  * --module-list-globals\n");
  fprintf(stderr, "    Read bytecode from stdin - list summary of globals\n\n");
  fprintf(stderr, "  * --targets-list\n");
  fprintf(stderr, "    List available targets\n\n");
  fprintf(stderr, "  * --object-list-sections\n");
  fprintf(stderr, "    Read object file form stdin - list sections\n\n");
  fprintf(stderr, "  * --object-list-symbols\n");
  fprintf(stderr,
          "    Read object file form stdin - list symbols (like nm)\n\n");
  fprintf(stderr, "  * --disassemble\n");
  fprintf(stderr, "    Read lines of triple, hex ascii machine code from stdin "
                  "- print disassembly\n\n");
  fprintf(stderr, "  * --calc\n");
  fprintf(
      stderr,
      "    Read lines of name, rpn from stdin - print generated module\n\n");
}

// HLSL Change: changed calling convention to __cdecl
int __cdecl main(int argc, char **argv) {
  LLVM37PassRegistryRef pr = LLVM37GetGlobalPassRegistry();

  LLVM37InitializeCore(pr);

  if (argc == 2 && !strcmp(argv[1], "--module-dump")) {
    return module_dump();
  } else if (argc == 2 && !strcmp(argv[1], "--module-list-functions")) {
    return module_list_functions();
  } else if (argc == 2 && !strcmp(argv[1], "--module-list-globals")) {
    return module_list_globals();
  } else if (argc == 2 && !strcmp(argv[1], "--targets-list")) {
    return targets_list();
  } else if (argc == 2 && !strcmp(argv[1], "--object-list-sections")) {
    return object_list_sections();
  } else if (argc == 2 && !strcmp(argv[1], "--object-list-symbols")) {
    return object_list_symbols();
  } else if (argc == 2 && !strcmp(argv[1], "--disassemble")) {
    return disassemble();
  } else if (argc == 2 && !strcmp(argv[1], "--calc")) {
    return calc();
  } else if (argc == 2 && !strcmp(argv[1], "--add-named-metadata-operand")) {
    return add_named_metadata_operand();
  } else if (argc == 2 && !strcmp(argv[1], "--set-metadata")) {
    return set_metadata();
  } else {
    print_usage();
  }

  return 1;
}
