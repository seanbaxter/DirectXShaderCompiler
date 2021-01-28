; Test that a prototype can be marked const, and the definition is allowed
; to be nonconst.

; RUN: echo "@X = global i32 7" | llvm37-as > %t.2.bc
; RUN: llvm37-as < %s > %t.1.bc
; RUN: llvm37-link %t.1.bc %t.2.bc -S | FileCheck %s
; CHECK: global i32 7

@X = external constant i32		; <i32*> [#uses=0]
