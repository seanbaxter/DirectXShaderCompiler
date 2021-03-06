; RUN: llvm37-dis < %s.bc| FileCheck %s
; RUN: verify-uselistorder < %s.bc

; vaArgIntrinsic.3.2.ll.bc was generated by passing this file to llvm37-as-3.2.
; The test checks that LLVM37 does not misread variable argument intrinsic instructions
; of older bitcode files.

define i32 @varArgIntrinsic(i32 %X, ...) {

  %ap = alloca i8*
  %ap2 = bitcast i8** %ap to i8*

; CHECK: call void @llvm37.va_start(i8* %ap2)
  call void @llvm37.va_start(i8* %ap2)

; CHECK-NEXT: %tmp = va_arg i8** %ap, i32
  %tmp = va_arg i8** %ap, i32

  %aq = alloca i8*
  %aq2 = bitcast i8** %aq to i8*

; CHECK: call void @llvm37.va_copy(i8* %aq2, i8* %ap2)
  call void @llvm37.va_copy(i8* %aq2, i8* %ap2)
; CHECK-NEXT: call void @llvm37.va_end(i8* %aq2)
  call void @llvm37.va_end(i8* %aq2)

; CHECK-NEXT:  call void @llvm37.va_end(i8* %ap2)
  call void @llvm37.va_end(i8* %ap2)
  ret i32 %tmp
}

declare void @llvm37.va_start(i8*)
declare void @llvm37.va_copy(i8*, i8*)
declare void @llvm37.va_end(i8*)
