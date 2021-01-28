; RUN: llvm37-as < %s | llvm37-dis
; RUN: verify-uselistorder %s
; PR3372

@X = global i32* @0
global i32 4

