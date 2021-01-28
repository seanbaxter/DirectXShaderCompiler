; RUN: llvm37-as < %s | llvm37-dis
; RUN: verify-uselistorder %s


define void @foo() {
  bitcast i32* null to i32*
  ret void
}
