; RUN: llvm37-as %s -o - | llvm37-dis > %t1.ll
; RUN: llvm37-as %t1.ll -o - | llvm37-dis > %t2.ll
; RUN: diff %t1.ll %t2.ll


@X = global i31 undef


declare i32 @"atoi"(i8 *)

define i63 @test() {
	ret i63 undef
}

define i31 @test2() {
	%X = add i31 undef, 1
	ret i31 %X
}
