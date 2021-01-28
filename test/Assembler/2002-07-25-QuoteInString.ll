; Test double quotes in strings work correctly!
; RUN: llvm37-as < %s | llvm37-dis | llvm37-as | llvm37-dis
;
; RUN: verify-uselistorder %s
@str = internal global [6 x i8] c"\22foo\22\00"         ; <[6 x i8]*> [#uses=0]

