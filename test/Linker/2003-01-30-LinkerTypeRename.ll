; This fails because the linker renames the non-opaque type not the opaque
; one...

; RUN: echo "%%Ty = type opaque @GV = external global %%Ty*" | llvm37-as > %t.1.bc
; RUN: llvm37-as < %s > %t.2.bc
; RUN: llvm37-link %t.1.bc %t.2.bc -S | FileCheck %s
; CHECK: = global %Ty

%Ty = type {i32}

@GV = global %Ty* null
