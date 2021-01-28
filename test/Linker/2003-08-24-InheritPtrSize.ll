; Linking a module with a specified pointer size to one without a 
; specified pointer size should not cause a warning!

; RUN: llvm37-as < %s > %t.out1.bc
; RUN: echo "" | llvm37-as > %t.out2.bc
; RUN: llvm37-link %t.out1.bc %t.out2.bc 2>&1 | FileCheck %s 
; CHECK-NOT: warning

target datalayout = "e-p:64:64"

