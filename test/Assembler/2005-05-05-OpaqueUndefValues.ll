; RUN: llvm37-as < %s | llvm37-dis | llvm37-as > /dev/null
; RUN: verify-uselistorder %s

%t = type opaque
@x = global %t undef
