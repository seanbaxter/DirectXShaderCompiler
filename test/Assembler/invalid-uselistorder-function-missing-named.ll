; RUN: not llvm37-as < %s -disable-output 2>&1 | FileCheck %s
; CHECK: error: value has no uses
define void @foo() {
  unreachable
  uselistorder i32 %val, { 1, 0 }
}
