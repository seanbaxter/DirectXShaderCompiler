; RUN: opt < %s -instcombine -S | FileCheck %s

; InstCombine should preserve the call to @llvm37.assume.
define i32 @icmp(i32 %a, i32 %b) {
; CHECK-LABEL: @icmp(
  %sum = add i32 %a, %b
  %1 = icmp sge i32 %sum, 0
  call void @llvm37.assume(i1 %1)
; CHECK: call void @llvm37.assume
  ret i32 %sum
}

define float @fcmp(float %a, float %b) {
; CHECK-LABEL: @fcmp(
  %sum = fadd float %a, %b
  %1 = fcmp oge float %sum, 0.0
  call void @llvm37.assume(i1 %1)
; CHECK: call void @llvm37.assume
  ret float %sum
}

declare void @llvm37.assume(i1)
