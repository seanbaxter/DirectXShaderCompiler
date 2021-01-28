; RUN: llvm37-as < %s | llvm37-dis | llvm37-as | llvm37-dis | FileCheck %s
; RUN: verify-uselistorder %s

; CHECK: @a = thread_local global i32 0
; CHECK: @b = thread_local(localdynamic) global i32 0
; CHECK: @c = thread_local(initialexec) global i32 0
; CHECK: @d = thread_local(localexec) global i32 0

@a = thread_local global i32 0
@b = thread_local(localdynamic) global i32 0
@c = thread_local(initialexec) global i32 0
@d = thread_local(localexec) global i32 0
