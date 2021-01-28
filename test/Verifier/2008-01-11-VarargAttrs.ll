; RUN: not llvm37-as < %s >& /dev/null

	%struct = type {  }

declare void @foo(...)

define void @bar() {
	call void (...) @foo(%struct* sret null )
	ret void
}
