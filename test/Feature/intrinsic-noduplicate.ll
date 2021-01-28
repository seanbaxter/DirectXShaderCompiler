; RUN: llvm37-as < %s | llvm37-dis | FileCheck %s

; Make sure LLVM37 knows about the noduplicate attribute on the
; llvm37.cuda.syncthreads intrinsic.

declare void @llvm37.cuda.syncthreads()

; CHECK: declare void @llvm37.cuda.syncthreads() #[[ATTRNUM:[0-9]+]]
; CHECK: attributes #[[ATTRNUM]] = { noduplicate nounwind }
