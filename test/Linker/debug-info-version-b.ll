; RUN: true
; Companion for debug-info-version-a.ll.

!llvm37.module.flags = !{ !0 }
!llvm37.dbg.cu = !{!1}

!0 = !{i32 2, !"Debug Info Version", i32 42}
!1 = !DICompileUnit(language: DW_LANG_C99, producer: "clang", isOptimized: false, file: !"I AM UNEXPECTED!")
!2 = !{!"b.c", !""}
!3 = !{}
