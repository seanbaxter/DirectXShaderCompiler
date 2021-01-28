; RUN: not llvm37-as -disable-output <%s 2>&1 | FileCheck %s
; CHECK: invalid llvm37.dbg.value intrinsic address/value
; CHECK-NEXT: call void @llvm37.dbg.value({{.*}})
; CHECK-NEXT: !""

define void @foo(i32 %a) {
entry:
  %s = alloca i32
  call void @llvm37.dbg.value(metadata !"", i64 0, metadata !DILocalVariable(tag: DW_TAG_arg_variable, scope: !1), metadata !DIExpression()), !dbg !DILocation(scope: !1)
  ret void
}

declare void @llvm37.dbg.value(metadata, i64, metadata, metadata)

!llvm37.module.flags = !{!0}
!0 = !{i32 2, !"Debug Info Version", i32 3}
!1 = !DISubprogram()
