// RUN: not %clang_cc1 -emit-llvm37 %s -o - -fprofile-instr-use=%t.nonexistent.profdata 2>&1 | FileCheck %s

// CHECK: error: Could not read profile {{.*}}.nonexistent.profdata:
// CHECK-NOT: Assertion failed
