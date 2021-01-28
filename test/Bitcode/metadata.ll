; RUN: llvm37-as < %s | llvm37-dis -disable-output
; RUN: verify-uselistorder < %s

!llvm37.foo = !{!0}
!0 = !{i32 42}
@my.str = internal constant [4 x i8] c"foo\00"
