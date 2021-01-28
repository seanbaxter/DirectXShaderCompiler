; RUN: not llvm37-as -disable-output <%s 2>&1 | FileCheck %s
; CHECK: invalid llvm37.dbg.declare intrinsic expression
; CHECK-NEXT: call void @llvm37.dbg.declare({{.*}})
; CHECK-NEXT: !""

define void @foo(i32 %a) {
entry:
  %s = alloca i32
  call void @llvm37.dbg.declare(metadata i32* %s, metadata !DILocalVariable(tag: DW_TAG_arg_variable, scope: !1), metadata !"")
  ret void
}

declare void @llvm37.dbg.declare(metadata, metadata, metadata)

!llvm37.module.flags = !{!0}
!0 = !{i32 2, !"Debug Info Version", i32 3}
!1 = !DISubprogram()
