/*===-- object.c - tool for testing libLLVM37 and llvm37-c API ----------------===*\
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This file implements the --object-list-sections and --object-list-symbols  *|
|* commands in llvm37-c-test.                                                   *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#include "llvm37-c-test.h"
#include "llvm37-c/Object.h"
#include <stdio.h>
#include <stdlib.h>

int object_list_sections(void) {
  LLVM37MemoryBufferRef MB;
  LLVM37ObjectFileRef O;
  LLVM37SectionIteratorRef sect;
  char *msg = NULL;

  if (LLVM37CreateMemoryBufferWithSTDIN(&MB, &msg)) {
    fprintf(stderr, "Error reading file: %s\n", msg);
    exit(1);
  }

  O = LLVM37CreateObjectFile(MB);
  if (!O) {
    fprintf(stderr, "Error reading object\n");
    exit(1);
  }

  sect = LLVM37GetSections(O);
  while (!LLVM37IsSectionIteratorAtEnd(O, sect)) {
    printf("'%s': @0x%08" PRIx64 " +%" PRIu64 "\n", LLVM37GetSectionName(sect),
           LLVM37GetSectionAddress(sect), LLVM37GetSectionSize(sect));

    LLVM37MoveToNextSection(sect);
  }

  LLVM37DisposeSectionIterator(sect);

  LLVM37DisposeObjectFile(O);

  return 0;
}

int object_list_symbols(void) {
  LLVM37MemoryBufferRef MB;
  LLVM37ObjectFileRef O;
  LLVM37SectionIteratorRef sect;
  LLVM37SymbolIteratorRef sym;
  char *msg = NULL;

  if (LLVM37CreateMemoryBufferWithSTDIN(&MB, &msg)) {
    fprintf(stderr, "Error reading file: %s\n", msg);
    exit(1);
  }

  O = LLVM37CreateObjectFile(MB);
  if (!O) {
    fprintf(stderr, "Error reading object\n");
    exit(1);
  }

  sect = LLVM37GetSections(O);
  sym = LLVM37GetSymbols(O);
  while (!LLVM37IsSymbolIteratorAtEnd(O, sym)) {

    LLVM37MoveToContainingSection(sect, sym);
    printf("%s @0x%08" PRIx64 " +%" PRIu64 " (%s)\n", LLVM37GetSymbolName(sym),
           LLVM37GetSymbolAddress(sym), LLVM37GetSymbolSize(sym),
           LLVM37GetSectionName(sect));

    LLVM37MoveToNextSymbol(sym);
  }

  LLVM37DisposeSymbolIterator(sym);

  LLVM37DisposeObjectFile(O);

  return 0;
}
