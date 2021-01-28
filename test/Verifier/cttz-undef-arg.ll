; RUN: not llvm37-as < %s -o /dev/null 2>&1 | FileCheck %s

declare i32 @llvm37.ctlz.i32(i32, i1)
declare i32 @llvm37.cttz.i32(i32, i1)

define void @f(i32 %x, i1 %is_not_zero) {
entry:
; CHECK: is_zero_undef argument of bit counting intrinsics must be a constant int
; CHECK-NEXT: @llvm37.ctlz.i32
  call i32 @llvm37.ctlz.i32(i32 %x, i1 %is_not_zero)

; CHECK: is_zero_undef argument of bit counting intrinsics must be a constant int
; CHECK-NEXT: @llvm37.cttz.i32
  call i32 @llvm37.cttz.i32(i32 %x, i1 %is_not_zero)
  ret void
}
