; RUN: echo "%%T = type opaque" | llvm37-as > %t.2.bc
; RUN: llvm37-as < %s > %t.1.bc
; RUN: llvm37-link %t.1.bc %t.2.bc

%T = type opaque
@a = constant { %T* } zeroinitializer		; <{ %T* }*> [#uses=0]

