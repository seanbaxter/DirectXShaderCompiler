; RUN: not llvm37-as < %s -o /dev/null 2>&1 | FileCheck %s
; Verify that llvm37.ident is properly structured.
; llvm37.ident takes a list of metadata entries.
; Each metadata entry can contain one string only.

!llvm37.ident = !{!0}
!0 = !{null}
; CHECK: assembly parsed, but does not verify as correct!
; CHECK-NEXT: invalid value for llvm37.ident metadata entry operand(the operand should be a string)
