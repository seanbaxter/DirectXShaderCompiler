; RUN: llvm37-as < %s | llvm37-dis > %t1.ll
; RUN: llvm37-as %t1.ll -o - | llvm37-dis > %t2.ll
; RUN: diff %t1.ll %t2.ll

@H1     = global half 0x4010000000000000
@F1     = global float 0x4010000000000000
@D1     = global double 0x4010000000000000
