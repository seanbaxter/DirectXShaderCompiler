; RUN: llvm37-as < %s >%t1
; RUN: llvm37-lto -o %t2 %t1

module asm ".align 4"
