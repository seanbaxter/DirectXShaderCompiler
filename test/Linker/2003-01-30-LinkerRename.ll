; RUN: llvm37-as %S/Inputs/2003-01-30-LinkerRename.ll -o %t.1.bc
; RUN: llvm37-as %s -o %t.2.bc
; RUN: llvm37-link %t.1.bc %t.2.bc -S | FileCheck %s

; CHECK: @bar = global i32 ()* @foo2

; CHECK:      define internal i32 @foo2() {
; CHECK-NEXT:   ret i32 7
; CHECK-NEXT: }

; CHECK:      define i32 @foo() {
; CHECK-NEXT:   ret i32 0
; CHECK-NEXT: }

define i32 @foo() {
  ret i32 0
}
