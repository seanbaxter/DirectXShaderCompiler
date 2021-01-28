; RUN: not llvm37-as < %s 2>&1 | FileCheck %s
target datalayout = "f"
; CHECK: Missing alignment specification in datalayout string
