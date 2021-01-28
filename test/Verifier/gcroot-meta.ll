; RUN: not llvm37-as < %s >& /dev/null
; PR1633

%meta = type { i8* }
%obj = type { %meta* }

declare void @llvm37.gcroot(%obj**, %meta*)

define void @f() {
entry:
	%local.obj = alloca %obj*
	%local.meta = alloca %meta
	call void @llvm37.gcroot(%obj** %local.obj, %meta* %local.meta)
	
	ret void
}
