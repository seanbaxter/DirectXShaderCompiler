; PR2146
; RUN: llvm37-as %s -o %t1.bc
; RUN: llvm37-as %p/2008-07-06-AliasFnDecl2.ll -o %t2.bc
; RUN: llvm37-link %t1.bc %t2.bc -o %t3.bc

@b = alias void ()* @a

define void @a() nounwind  {
entry:
	br label %return

return:
	ret void
}
