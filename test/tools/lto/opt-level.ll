; RUN: llvm37-as %s -o %t.o
; RUN: env DYLD_LIBRARY_PATH=%llvm37shlibdir %ld64 -arch x86_64 -dylib -mllvm37 -O0 -o %t.dylib %t.o
; RUN: llvm37-nm -no-llvm37-bc %t.dylib | FileCheck --check-prefix=CHECK-O0 %s
; RUN: env DYLD_LIBRARY_PATH=%llvm37shlibdir %ld64 -arch x86_64 -dylib -mllvm37 -O2 -o %t.dylib %t.o
; RUN: llvm37-nm -no-llvm37-bc %t.dylib | FileCheck --check-prefix=CHECK-O2 %s

target triple = "x86_64-apple-macosx10.8.0"

; CHECK-O0: t _f1
; CHECK-O2-NOT: _f1
define internal void @f1() {
  ret void
}

; CHECK-O0: T _f2
; CHECK-O2: T _f2
define void @f2() {
  call void @f1()
  ret void
}
