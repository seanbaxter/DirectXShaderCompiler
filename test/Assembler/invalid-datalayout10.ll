; RUN: not llvm37-as < %s 2>&1 | FileCheck %s
target datalayout = "m"
; CHECK: Expected mangling specifier in datalayout string
