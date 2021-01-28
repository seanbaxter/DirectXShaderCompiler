; RUN: not llvm37-as < %s >& /dev/null

declare i32 @atoi(i8*) nounwind readonly
declare i32 @atoi(i8*)
