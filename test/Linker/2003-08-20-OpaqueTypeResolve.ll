; RUN: llvm37-as < %s > %t.out1.bc
; RUN: echo "%M = type { i32, i32* } " | llvm37-as > %t.out2.bc
; RUN: llvm37-link %t.out1.bc %t.out2.bc

%M = type { i32, %N* }
%N = type opaque

;%X = global { int, %N* } { int 5, %N* null }
