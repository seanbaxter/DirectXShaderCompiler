; The linker should merge link-once globals into strong external globals,
; just like it does for weak symbols!

; RUN: echo "@X = global i32 7" | llvm37-as > %t.2.bc
; RUN: llvm37-as < %s > %t.1.bc
; RUN: llvm37-link %t.1.bc %t.2.bc

@X = linkonce global i32 7
