; RUN: not llvm37-as < %s -o /dev/null 2>&1 | FileCheck %s

@llvm37.compiler.used = appending global [1 x i32] [i32 0], section "llvm.metadata"

; CHECK:       wrong type for intrinsic global variable
; CHECK-NEXT: [1 x i32]* @llvm37.compiler.used
