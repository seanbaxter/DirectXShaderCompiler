; RUN: llvm37-as < %s | llvm37-dis > %t1.ll
; RUN: llvm37-as %t1.ll -o - | llvm37-dis > %t2.ll
; RUN: diff %t1.ll %t2.ll

@.str = private unnamed_addr constant [14 x i8] c"hello world!\0A\00", align 1

define void @foo() #0 {
entry:
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str, i32 0, i32 0))
  ret void
}

declare i32 @printf(i8*, ...)

attributes #0 = { nounwind ssp uwtable }
