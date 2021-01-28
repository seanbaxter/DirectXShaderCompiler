; RUN: llvm37-as < %s > %t.bc
; RUN: llvm37-as < %p/linknamedmdnode2.ll > %t2.bc
; RUN: llvm37-link %t.bc %t2.bc -S | FileCheck %s
; CHECK: !llvm37.stuff = !{!0, !1}

!0 = !{i32 42}
!llvm37.stuff = !{!0}
