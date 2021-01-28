; RUN: not llvm37-as < %s 2>&1 | FileCheck %s
target datalayout = "^"
; CHECK: Unknown specifier in datalayout string
