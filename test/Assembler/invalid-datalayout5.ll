; RUN: not llvm37-as < %s 2>&1 | FileCheck %s
target datalayout = "a1:64"
; CHECK: Sized aggregate specification in datalayout string
