; RUN: llvm37-link %s -override %S/Inputs/override.ll -S | FileCheck %s
; RUN: llvm37-link -override %S/Inputs/override.ll %s -S | FileCheck %s


; CHECK-LABEL: define i32 @foo
; CHECK-NEXT: entry:
; CHECK-NEXT: ret i32 4
define i32 @foo(i32 %i) {
entry:
  %add = add nsw i32 %i, %i
  ret i32 %add
}

; Function Attrs: nounwind ssp uwtable
define i32 @main(i32 %argc, i8** %argv) {
entry:
  %a = call i32 @foo(i32 2)
  ret i32 %a
}
