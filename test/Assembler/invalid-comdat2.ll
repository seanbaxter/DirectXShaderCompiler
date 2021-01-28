; RUN: not llvm37-as < %s 2>&1 | FileCheck %s

$v = comdat any
$v = comdat any
; CHECK: redefinition of comdat '$v'
