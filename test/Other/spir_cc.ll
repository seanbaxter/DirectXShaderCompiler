; RUN: llvm37-as < %s | llvm37-dis > %t1.ll
; RUN: llvm37-as %t1.ll -o - | llvm37-dis > %t2.ll
; RUN: diff %t1.ll %t2.ll

define spir_func void @foo() {
        ret void
}

define spir_kernel void @bar() {
        call spir_func void @foo( )
        call spir_kernel void @bar( )
        ret void
}
