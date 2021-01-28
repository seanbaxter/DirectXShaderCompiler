; RUN: llvm37-as < %s | llvm37-dis > %t1.ll
; RUN: llvm37-as %t1.ll -o - | llvm37-dis > %t2.ll
; RUN: diff %t1.ll %t2.ll

declare i32 @bar(i32)

define i32 @foo(i32 %blah) {
        %xx = call i32 @bar( i32 %blah )                ; <i32> [#uses=1]
        ret i32 %xx
}

