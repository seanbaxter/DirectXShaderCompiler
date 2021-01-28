; This fails linking when it is linked with an empty file as the first object file

; RUN: llvm37-as > %t1.bc < /dev/null
; RUN: llvm37-as < %s  > %t2.bc
; RUN: llvm37-link %t1.bc %t2.bc

@work = global i32 (i32, i32)* @zip		; <i32 (i32, i32)**> [#uses=0]

declare i32 @zip(i32, i32)

