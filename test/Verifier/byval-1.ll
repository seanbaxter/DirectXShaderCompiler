; RUN: not llvm37-as < %s >& /dev/null
declare void @h(i32 byval %num)
