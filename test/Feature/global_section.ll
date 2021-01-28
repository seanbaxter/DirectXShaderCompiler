; RUN: llvm37-as < %s | llvm37-dis > %t1.ll
; RUN: llvm37-as %t1.ll -o - | llvm37-dis > %t2.ll
; RUN: diff %t1.ll %t2.ll

@X = global i32 4, section "foo", align 16              ; <i32*> [#uses=0]

define void @test() section "bar" {
        ret void
}

