; RUN: not llvm37-as < %s 2>&1 | FileCheck %s
; CHECK: value doesn't match function result type 'void'

define void @foo() {
  ret i32 0
}
