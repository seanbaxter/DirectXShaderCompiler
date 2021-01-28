; Test linking two functions with different prototypes and two globals 
; in different modules.
; RUN: llvm37-as %s -o %t.foo1.bc
; RUN: echo | llvm37-as -o %t.foo2.bc
; RUN: llvm37-link %t.foo2.bc %t.foo1.bc -S | FileCheck %s
; RUN: llvm37-link %t.foo1.bc %t.foo2.bc -S | FileCheck %s
; CHECK: addrspace(2)
; rdar://6038021

@G = addrspace(2) global i32 256 
