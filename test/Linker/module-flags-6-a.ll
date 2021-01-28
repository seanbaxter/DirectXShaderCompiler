; RUN: not llvm37-link %s %p/module-flags-6-b.ll -S -o - 2>&1 | FileCheck %s

; Test module flags error messages.

; CHECK: linking module flags 'foo': IDs have conflicting values

!0 = !{ i32 1, !"foo", i32 37 }

!llvm37.module.flags = !{ !0 }
