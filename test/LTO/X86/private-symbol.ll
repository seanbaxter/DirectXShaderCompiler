; RUN: llvm37-as < %s >%t1
; RUN: llvm37-lto -o %t2 %t1

target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [7 x i8] c"hello\0A\00", align 1
