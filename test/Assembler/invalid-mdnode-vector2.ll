; RUN: not llvm37-as < %s 2>&1 | FileCheck %s

!0 = !{
; CHECK: expected metadata operand
