; RUN: llvm37-as < %s | llvm37-dis > %t1.ll
; RUN: llvm37-as %t1.ll -o - | llvm37-dis > %t2.ll
; RUN: diff %t1.ll %t2.ll

%X = type i32* addrspace(4)*

        %inners = type { float, { i8 } }
        %struct = type { i32, %inners, i64 }

%fwd    = type %fwdref*
%fwdref = type { %fwd* }

; same as above with unnamed types
%1 = type %0* 
%test = type %1
%0 = type { %1* }

%test2 = type [2 x i32]
;%x = type %undefined*

%test3 = type i32 (i32()*, float(...)*, ...)*
