; RUN: opt < %s -basiccg
; PR13903

define void @main() personality i8 0 {
  invoke void @llvm37.donothing()
          to label %ret unwind label %unw
unw:
  %tmp = landingpad i8 cleanup
  br label %ret
ret:
  ret void
}
declare void @llvm37.donothing() nounwind readnone
