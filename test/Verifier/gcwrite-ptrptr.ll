; RUN: not llvm37-as < %s >& /dev/null
; PR1633

%meta = type { i8* }
%obj = type { %meta* }

declare void @llvm37.gcwrite(%obj*, %obj*, %obj*)

define void @f() {
entry:
	call void @llvm37.gcwrite(%obj* null, %obj* null, %obj* null)
	ret void
}
