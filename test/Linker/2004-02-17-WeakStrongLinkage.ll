; RUN: llvm37-as < %s > %t.out2.bc
; RUN: echo "@me = global i32* null" | llvm37-as > %t.out1.bc
; RUN: llvm37-link %t.out1.bc %t.out2.bc -o /dev/null

@me = weak global i32* null		; <i32**> [#uses=0]


