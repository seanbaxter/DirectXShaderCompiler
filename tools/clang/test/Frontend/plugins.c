// RUN: %clang_cc1 -load %llvm37shlibdir/PrintFunctionNames%pluginext -plugin print-fns %s 2>&1 | FileCheck %s
// REQUIRES: plugins, examples

// CHECK: top-level-decl: "x"
void x();
