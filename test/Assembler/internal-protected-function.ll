; RUN: not llvm37-as < %s -o /dev/null 2>&1 | FileCheck %s

define internal protected void @function() {
; CHECK: symbol with local linkage must have default visibility
entry:
  ret void
}
