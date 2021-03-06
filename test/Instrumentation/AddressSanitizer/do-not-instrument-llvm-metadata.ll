; This test checks that we are not instrumenting globals in llvm37.metadata
; and other llvm37 internal globals.
; RUN: opt < %s -asan -asan-module -S | FileCheck %s

target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str_noinst = private unnamed_addr constant [4 x i8] c"aaa\00", section "llvm.metadata"
@.str_noinst_prof = private unnamed_addr constant [4 x i8] c"aaa\00", section "__llvm37_prf_data"
@.str_inst = private unnamed_addr constant [4 x i8] c"aaa\00"

; CHECK-NOT: {{asan_gen.*str_noinst}}
; CHECK-NOT: {{asan_gen.*str_noinst_prof}}
; CHECK: {{asan_gen.*str_inst}}
; CHECK: @asan.module_ctor
