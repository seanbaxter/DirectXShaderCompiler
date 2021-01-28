; RUN: echo "@G = weak global {{{{double}}}} zeroinitializer " | \
; RUN:   llvm37-as > %t.out2.bc
; RUN: llvm37-as < %s > %t.out1.bc
; RUN: llvm37-link %t.out1.bc %t.out2.bc -S | FileCheck %s
; CHECK-NOT: }

; When linked, the global above should be eliminated, being merged with the 
; global below.

@G = global double 1.0
