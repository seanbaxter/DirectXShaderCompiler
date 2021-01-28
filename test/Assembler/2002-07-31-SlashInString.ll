; RUN: llvm37-as < %s | llvm37-dis | llvm37-as 
; RUN: verify-uselistorder %s

; Make sure that \\ works in a string initializer
@Slashtest = internal global [8 x i8] c"\5Cbegin{\00"

