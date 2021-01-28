; RUN: opt < %s -S | FileCheck %s

!llvm37.module.flags = !{!0}
!0 = !{i32 1, !"Debug Info Version", !""}
; CHECK: !{i32 1, !"Debug Info Version", !""}
