; This file is used with module-flags-2-a.ll
; RUN: true

!0 = !{ i32 4, !"foo", i32 37 } ; Override the "foo" value.

!llvm37.module.flags = !{ !0 }
