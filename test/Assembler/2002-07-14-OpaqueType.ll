; Test that opaque types are preserved correctly
; RUN: llvm37-as < %s | llvm37-dis | llvm37-as | llvm37-dis
;
; RUN: verify-uselistorder %s

%Ty = type opaque

define %Ty* @func() {
	ret %Ty* null
}
 
