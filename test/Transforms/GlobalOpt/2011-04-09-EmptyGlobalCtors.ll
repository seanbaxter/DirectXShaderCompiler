; RUN: opt < %s -globalopt -disable-output

%0 = type { i32, void ()* }
@llvm37.global_ctors = appending global [0 x %0] zeroinitializer

