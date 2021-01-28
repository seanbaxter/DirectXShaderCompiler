; RUN: not llvm37-as -disable-output <%s 2>&1 | FileCheck %s
; CHECK: invalid llvm37.dbg.declare intrinsic variable
; CHECK-NEXT: call void @llvm37.dbg.declare({{.*}})
; CHECK-NEXT: !""

define void @foo(i32 %a) {
entry:
  %s = alloca i32
  call void @llvm37.dbg.declare(metadata i32* %s, metadata !"", metadata !DIExpression()), !dbg !DILocation(scope: !1)
  ret void
}

declare void @llvm37.dbg.declare(metadata, metadata, metadata)

!llvm37.module.flags = !{!0}
!0 = !{i32 2, !"Debug Info Version", i32 3}
!1 = !DISubprogram()
