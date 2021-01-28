; RUN: llvm37-as < %s | llvm37-dis | llvm37-as | llvm37-dis | FileCheck %s
; RUN: verify-uselistorder %s

declare void @llvm37.metadata(metadata)

define void @foo(i32 %arg) {
entry:
  %before = alloca i32
  call void @llvm37.metadata(metadata i32 %arg)
  call void @llvm37.metadata(metadata i32* %after)
  call void @llvm37.metadata(metadata i32* %before)
  %after = alloca i32
  ret void

; CHECK: %before = alloca i32
; CHECK: call void @llvm37.metadata(metadata i32 %arg)
; CHECK: call void @llvm37.metadata(metadata i32* %after)
; CHECK: call void @llvm37.metadata(metadata i32* %before)
; CHECK: %after = alloca i32
}
