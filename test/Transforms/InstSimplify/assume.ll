; RUN: opt -instsimplify -S < %s | FileCheck %s

define void @test1() {
  call void @llvm37.assume(i1 1)
  ret void

; CHECK-LABEL: @test1
; CHECK-NOT: llvm37.assume
; CHECK: ret void
}

declare void @llvm37.assume(i1) nounwind

