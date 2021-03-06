; Test whether negative values > 64 bits retain their negativeness.
; RUN: llvm37-as < %s | llvm37-dis | FileCheck %s
; RUN: verify-uselistorder %s

define i65 @testConsts(i65 %N) {
; CHECK: add i65 %N, -1
  %a = add i65 %N, -1
  ret i65 %a
}
