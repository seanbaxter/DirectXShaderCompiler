; RUN: not llvm37-as < %s >& /dev/null
; PR1633

%meta = type { i8* }
%obj = type { %meta* }

declare %obj* @llvm37.gcread(%obj*, %obj*)

define %obj* @f() {
entry:
	%x = call %obj* @llvm37.gcread(%obj* null, %obj* null)
	ret %obj* %x
}
