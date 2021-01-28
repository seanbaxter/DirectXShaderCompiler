; RUN: llvm37-as < %s | llvm37-dis | llvm37-as | llvm37-dis | FileCheck %s
; RUN: verify-uselistorder %s

; Don't crash on null operands.  When we add a verify check for this, also
; require non-null in the assembler and rework this test to check for that ala
; test/Assembler/invalid-mdcompileunit-null-file.ll.
!named = !{!0}
!0 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null)

; CHECK: !named = !{!0}
; CHECK: !0 = !DIDerivedType({{.*}}baseType: null{{.*}})
