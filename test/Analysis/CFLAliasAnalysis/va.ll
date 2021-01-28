; RUN: opt < %s -cfl-aa -aa-eval -print-may-aliases -disable-output 2>&1 | FileCheck %s

; CHECK-LABEL: Function: test1
; CHECK: 0 no alias responses

define i32 @test1(i32 %X, ...) {
  ; Initialize variable argument processing
  %ap = alloca i8*
  %ap2 = bitcast i8** %ap to i8*
  call void @llvm37.va_start(i8* %ap2)

  ; Read a single integer argument
  %tmp = va_arg i8** %ap, i32

  ; Demonstrate usage of llvm37.va_copy and llvm37.va_end
  %aq = alloca i8*
  %aq2 = bitcast i8** %aq to i8*
  call void @llvm37.va_copy(i8* %aq2, i8* %ap2)
  call void @llvm37.va_end(i8* %aq2)

  ; Stop processing of arguments.
  call void @llvm37.va_end(i8* %ap2)
  ret i32 %tmp
}

declare void @llvm37.va_start(i8*)
declare void @llvm37.va_copy(i8*, i8*)
declare void @llvm37.va_end(i8*)

