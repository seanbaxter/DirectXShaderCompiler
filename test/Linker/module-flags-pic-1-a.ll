; RUN: llvm37-link %s %p/Inputs/module-flags-pic-1-b.ll -S -o - | FileCheck %s

; test linking modules with specified and default PIC levels

!0 = !{ i32 1, !"PIC Level", i32 1 }

!llvm37.module.flags = !{!0}
; CHECK: !llvm37.module.flags = !{!0}
; CHECK: !0 = !{i32 1, !"PIC Level", i32 1}
