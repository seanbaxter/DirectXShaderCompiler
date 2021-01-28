; RUN: not llvm37-as < %s -o /dev/null 2>&1 | FileCheck %s

!llvm37.module.flags = !{!0}
!0 = !{i32 1, null, null}

; CHECK: invalid ID operand in module flag (expected metadata string)
