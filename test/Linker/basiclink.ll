; Test linking two functions with different prototypes and two globals 
; in different modules. This is for PR411
; RUN: llvm37-as %S/Inputs/basiclink.a.ll -o %t.foo.bc
; RUN: llvm37-as %S/Inputs/basiclink.b.ll -o %t.bar.bc
; RUN: llvm37-link %t.foo.bc %t.bar.bc -o %t.bc
; RUN: llvm37-link %t.bar.bc %t.foo.bc -o %t.bc
