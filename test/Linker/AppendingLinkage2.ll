; Test that appending linkage works correctly when arrays are the same size.

; RUN: echo "@X = appending global [1 x i32] [i32 8] " | \
; RUN:   llvm37-as > %t.2.bc
; RUN: llvm37-as < %s > %t.1.bc
; RUN: llvm37-link %t.1.bc %t.2.bc -S | FileCheck %s
; CHECK: [i32 7, i32 8]

@X = appending global [1 x i32] [ i32 7 ]		; <[1 x i32]*> [#uses=0]
