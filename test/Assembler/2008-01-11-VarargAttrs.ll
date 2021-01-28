; RUN: llvm37-as < %s | llvm37-dis | grep byval
; RUN: verify-uselistorder %s

	%struct = type {  }

declare void @foo(...)

define void @bar() {
	call void (...) @foo(%struct* byval null )
	ret void
}
