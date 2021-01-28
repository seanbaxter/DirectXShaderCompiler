; RUN: llvm37-as %S/Inputs/linkage.a.ll -o %t.1.bc
; RUN: llvm37-as %S/Inputs/linkage.b.ll -o %t.2.bc
; RUN: llvm37-link %t.1.bc  %t.2.bc
