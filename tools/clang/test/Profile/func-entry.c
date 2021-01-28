// Test that function entry counts are set correctly.

// RUN: llvm37-profdata merge %S/Inputs/func-entry.proftext -o %t.profdata
// RUN: %clang %s -o - -mllvm37 -disable-llvm37-optzns -emit-llvm37 -S -fprofile-instr-use=%t.profdata | FileCheck %s

void foo(void);

// CHECK: @foo() #0 !prof [[FOO:![0-9]+]]
void foo() { return; }

// CHECK: @main() #1 !prof [[MAIN:![0-9]+]]
int main() {
  int i;
  for (i = 0; i < 10000; i++) foo();
  return 0;
}

// CHECK: [[FOO]] = !{!"function_entry_count", i64 1000}
// CHECK: [[MAIN]] = !{!"function_entry_count", i64 1}
