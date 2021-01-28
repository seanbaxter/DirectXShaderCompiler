; RUN: llvm37-as < %s | llvm37-dis | grep "global i32 0"
; PR1215

@G = global i32 sdiv (i32 0, i32 -1)

