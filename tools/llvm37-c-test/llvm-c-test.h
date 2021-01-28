/*===-- llvm37-c-test.h - tool for testing libLLVM37 and llvm37-c API -----------===*\
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* Header file for llvm37-c-test                                                *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/
#define LLVM37_C_TEST_H

// helpers.c
void tokenize_stdin(void (*cb)(char **tokens, int ntokens));

// module.c
int module_dump(void);
int module_list_functions(void);
int module_list_globals(void);

// calc.c
int calc(void);

// disassemble.c
int disassemble(void);

// metadata.c
int add_named_metadata_operand(void);
int set_metadata(void);

// object.c
int object_list_sections(void);
int object_list_symbols(void);

// targets.c
int targets_list(void);

#endif

