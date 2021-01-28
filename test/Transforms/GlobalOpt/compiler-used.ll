; RUN: opt < %s -globalopt -S | FileCheck %s

; Test that when all members of llvm37.compiler.used are found to be redundant
; we delete it instead of crashing.

define void @foo() {
  ret void
}

@llvm37.used = appending global [1 x i8*] [i8* bitcast (void ()* @foo to i8*)], section "llvm.metadata"

@llvm37.compiler.used = appending global [1 x i8*] [i8* bitcast (void ()* @foo to i8*)], section "llvm.metadata"

; CHECK-NOT: @llvm37.compiler.used
; CHECK: @llvm37.used = appending global [1 x i8*] [i8* bitcast (void ()* @foo to i8*)], section "llvm.metadata"
; CHECK-NOT: @llvm37.compiler.used
