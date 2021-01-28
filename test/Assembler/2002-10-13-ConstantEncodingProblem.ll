; RUN: llvm37-as < %s | llvm37-dis
; RUN: verify-uselistorder %s

%Domain = type { %Domain**, %Domain* }
@D = global %Domain zeroinitializer             ; <%Domain*> [#uses=0]

