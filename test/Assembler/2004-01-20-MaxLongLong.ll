; RUN: llvm37-as < %s | llvm37-dis | grep 9223372036854775808
; RUN: verify-uselistorder %s

global i64 -9223372036854775808

