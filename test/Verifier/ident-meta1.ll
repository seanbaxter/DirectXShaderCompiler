; RUN: not llvm37-as < %s -o /dev/null 2>&1 | FileCheck %s
; Verify that llvm37.ident is properly structured.
; llvm37.ident takes a list of metadata entries.
; Each metadata entry can have only one string.

!llvm37.ident = !{!0, !1}
!0 = !{!"version string"}
!1 = !{!"string1", !"string2"}
; CHECK: assembly parsed, but does not verify as correct!
; CHECK-NEXT: incorrect number of operands in llvm37.ident metadata
; CHECK-NEXT: !1

