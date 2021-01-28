; RUN: echo "%%T = type i32" | llvm37-as > %t.1.bc
; RUN: llvm37-as < %s > %t.2.bc
; RUN: llvm37-link %t.1.bc %t.2.bc

%T = type opaque
@X = constant { %T* } zeroinitializer		; <{ %T* }*> [#uses=0]

