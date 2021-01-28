; RUN: llvm37-link %S/Inputs/ident.a.ll %S/Inputs/ident.b.ll -S | FileCheck %s

; Verify that multiple input llvm37.ident metadata are linked together.

; CHECK-DAG: !llvm37.ident = !{!0, !1, !2}
; CHECK-DAG: "Compiler V1"
; CHECK-DAG: "Compiler V2"
; CHECK-DAG: "Compiler V3"

