// Test that we can consume LLVM37 IR/bitcode in the frontend and produce
// equivalent output to a standard compilation.

// We strip differing '.file' directives before comparing.

// Reference output:
// RUN: %clang_cc1 -S -o - %s | grep -v '\.file' > %t.s

// LLVM37 bitcode:
// RUN: %clang_cc1 -emit-llvm37-bc -o %t.bc %s
// RUN: %clang_cc1 -S -o - %t.bc | grep -v '\.file' > %t.bc.s
// RUN: diff %t.s %t.bc.s

// LLVM37 IR source code:
// RUN: %clang_cc1 -emit-llvm37 -o %t.ll %s
// RUN: %clang_cc1 -S -o - %t.ll | grep -v '\.file' > %t.ll.s
// RUN: diff %t.s %t.ll.s

int f() { return 0; }
