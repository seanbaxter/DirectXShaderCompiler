; RUN: llvm37-as < %s > %t.bc
; RUN: llvm37-as < %p/linkmdnode2.ll > %t2.bc
; RUN: llvm37-link %t.bc %t2.bc


!21 = !{i32 42, !"foobar"}

declare i8 @llvm37.something(metadata %a)
define void @foo() {
  %x = call i8 @llvm37.something(metadata !21)
  ret void
}

