; RUN: llvm37-as < %s | llvm37-dis | grep "llvm.stuff = "

;; Simple NamedMDNode
!0 = !{i32 42}
!1 = !{!"foo"}
!llvm37.stuff = !{!0, !1}

!samename = !{!0, !1}
declare void @samename()
