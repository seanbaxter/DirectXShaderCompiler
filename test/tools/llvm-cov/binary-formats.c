// Checks for reading various formats.

// CHECK: 100| [[@LINE+1]]|int main
int main(int argc, const char *argv[]) {}

// RUN: llvm37-profdata merge %S/Inputs/binary-formats.proftext -o %t.profdata
// RUN: llvm37-cov show %S/Inputs/binary-formats.macho32l -instr-profile %t.profdata -filename-equivalence %s | FileCheck %s
// RUN: llvm37-cov show %S/Inputs/binary-formats.macho64l -instr-profile %t.profdata -filename-equivalence %s | FileCheck %s
// RUN: llvm37-cov show %S/Inputs/binary-formats.macho32b -instr-profile %t.profdata -filename-equivalence %s | FileCheck %s
