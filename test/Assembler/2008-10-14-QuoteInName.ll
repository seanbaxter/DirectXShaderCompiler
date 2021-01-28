; RUN: llvm37-as < %s | llvm37-dis | grep "quote"
; RUN: verify-uselistorder %s

@"a\22quote" = global i32 0
