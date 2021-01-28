; RUN: opt -simplifycfg -S < %s | FileCheck %s

define void @test1() {
        call void @llvm37.assume(i1 0)
        ret void

; CHECK-LABEL: @test1
; CHECK-NOT: llvm37.assume
; CHECK: unreachable
}

define void @test2() {
        call void @llvm37.assume(i1 undef)
        ret void

; CHECK-LABEL: @test2
; CHECK-NOT: llvm37.assume
; CHECK: unreachable
}

declare void @llvm37.assume(i1) nounwind

