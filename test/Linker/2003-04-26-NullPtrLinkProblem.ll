; This one fails because the LLVM37 runtime is allowing two null pointers of
; the same type to be created!

; RUN: echo "%%T = type i32" | llvm37-as > %t.2.bc
; RUN: llvm37-as %s -o %t.1.bc
; RUN: llvm37-link %t.1.bc %t.2.bc

%T = type opaque

declare %T* @create()

define void @test() {
	%X = call %T* @create( )		; <%T*> [#uses=1]
	%v = icmp eq %T* %X, null		; <i1> [#uses=0]
	ret void
}

