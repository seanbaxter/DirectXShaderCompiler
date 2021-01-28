; RUN: llvm37-as < %s | llvm37-dis | llvm37-as | llvm37-dis | FileCheck %s
; RUN: verify-uselistorder %s

; CHECK: @G = externally_initialized global i32 0

@G = externally_initialized global i32 0
