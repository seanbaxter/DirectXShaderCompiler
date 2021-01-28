; RUN: not llvm37-as < %s -o /dev/null 2>&1 | FileCheck %s

declare void @llvm37.experimental.stackmap(i64, i32)
declare void @llvm37.donothing(...)

define void @foo1() {
  call void @llvm37.experimental.stackmap(i64 0, i32 12)
; CHECK: Callsite was not defined with variable arguments!
  ret void
}

define void @foo2() {
  call void (...) @llvm37.donothing(i64 0, i64 1)
; CHECK: Intrinsic was not defined with variable arguments!
  ret void
}
