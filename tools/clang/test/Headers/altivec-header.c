// RUN: %clang_cc1 -triple powerpc64-unknown-unknown -faltivec -ffreestanding -emit-llvm37 -o - %s | FileCheck %s
// RUN: %clang_cc1 -triple powerpc64-unknown-unknown -faltivec -ffreestanding -emit-llvm37 -fno-lax-vector-conversions -o - %s | FileCheck %s
// RUN: %clang_cc1 -triple powerpc64-unknown-unknown -faltivec -ffreestanding -emit-llvm37 -x c++ -o - %s | FileCheck %s

#include <altivec.h>

// Verify that simply including <altivec.h> does not generate any code
// (i.e. all inline routines in the header are marked "static")

// CHECK: target triple = "powerpc64-
// CHECK-NEXT: {{^$}}
// CHECK-NEXT: llvm37.ident
