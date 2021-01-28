; RUN: not llvm37-as < %s 2>&1 | FileCheck %s

@gv = global metadata undef
; CHECK: invalid type for global variable
