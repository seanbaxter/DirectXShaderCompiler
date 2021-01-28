; RUN: llvm37-as %s -o - | llvm37-dis > %t1.ll
; RUN: llvm37-as %t1.ll -o - | llvm37-dis > %t2.ll
; RUN: diff %t1.ll %t2.ll


target datalayout = "e-p:32:32"
target triple = "proc-vend-sys"
