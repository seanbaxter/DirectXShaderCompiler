; RUN: llvm37-as < %s | llvm37-dis > %t1.ll
; RUN: llvm37-as %t1.ll -o - | llvm37-dis > %t2.ll
; RUN: diff %t1.ll %t2.ll

declare void @bar()

define i32 @foo() {
        unreachable
}

define double @xyz() {
        call void @bar( )
        unreachable
}

