; Test that returning a pointer to a function causes the disassembler to print 
; the right thing.
;
; RUN: llvm37-as < %s | llvm37-dis | llvm37-as
; RUN: verify-uselistorder %s

declare void (i32)* @foo()

define void @test() {
        call void (i32)* () @foo( )           ; <%ty*>:1 [#uses=0]
        ret void
}


