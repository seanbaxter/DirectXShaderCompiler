; RUN: not llvm37-as < %s 2>&1 | FileCheck %s

; CHECK: clause argument must be a constant

define void @test(i32 %in) personality void()* null {
  landingpad {} filter i32 %in
}
