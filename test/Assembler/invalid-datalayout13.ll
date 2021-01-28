; RUN: not llvm37-as < %s 2>&1 | FileCheck %s
target datalayout = ":32"
; CHECK: Expected token before separator in datalayout string
