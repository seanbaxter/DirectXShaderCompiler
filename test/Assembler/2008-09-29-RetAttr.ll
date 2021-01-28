; Test return attributes
; RUN: llvm37-as < %s | llvm37-dis | grep "define inreg i32"
; RUN: llvm37-as < %s | llvm37-dis | grep "call inreg i32"
; RUN: verify-uselistorder %s

define inreg i32 @fn1() {
  ret i32 0
}

define void @fn2() {
  %t = call inreg i32 @fn1()
  ret void
}

