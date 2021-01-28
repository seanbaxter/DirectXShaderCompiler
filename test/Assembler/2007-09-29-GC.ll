; RUN: llvm37-as < %s | llvm37-dis | FileCheck %s
; RUN: verify-uselistorder %s


; CHECK: define void @f() gc "shadowstack"
; CHECK: define void @g() gc "java"

define void @f() gc "shadowstack" {
entry:
	ret void
}

define void @g() gc "java" {
entry:
	ret void
}
