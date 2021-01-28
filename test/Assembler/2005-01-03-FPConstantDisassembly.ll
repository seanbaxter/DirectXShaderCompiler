; RUN: llvm37-as < %s | llvm37-dis | FileCheck %s
; RUN: verify-uselistorder %s

define double @test() {
; CHECK: ret double 1.0
        ret double 1.0   ;; This should not require hex notation
}

