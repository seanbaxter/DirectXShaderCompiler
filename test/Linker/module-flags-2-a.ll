; RUN: llvm37-link %s %p/module-flags-2-b.ll -S -o - | sort | FileCheck %s

; Test the 'override' behavior.

; CHECK: !0 = !{i32 4, !"foo", i32 37}
; CHECK: !llvm37.module.flags = !{!0}

!0 = !{ i32 1, !"foo", i32 927 }

!llvm37.module.flags = !{ !0 }
