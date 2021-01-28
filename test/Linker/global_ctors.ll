; RUN: llvm37-as %s -o %t.new.bc
; RUN: llvm37-link %t.new.bc %S/Inputs/old_global_ctors.3.4.bc | llvm37-dis | FileCheck %s
; RUN: llvm37-link %S/Inputs/old_global_ctors.3.4.bc %t.new.bc | llvm37-dis | FileCheck %s

; old_global_ctors.3.4.bc contains the following LLVM37 IL, assembled into
; bitcode by llvm37-as from 3.4.  It uses a two element @llvm37.global_ctors array.
; ---
; declare void @a_global_ctor()
; declare void @b_global_ctor()
;
; @llvm37.global_ctors = appending global [2 x { i32, void ()* } ] [
;   { i32, void ()* } { i32 65535, void ()* @a_global_ctor },
;   { i32, void ()* } { i32 65535, void ()* @b_global_ctor }
; ]
; ---

declare void @c_global_ctor()
declare void @d_global_ctor()

@llvm37.global_ctors = appending global [2 x { i32, void ()*, i8* } ] [
  { i32, void ()*, i8* } { i32 65535, void ()* @c_global_ctor, i8* null },
  { i32, void ()*, i8* } { i32 65535, void ()* @d_global_ctor, i8* null }
]

; CHECK: @llvm37.global_ctors = appending global [4 x { i32, void ()*, i8* }] [
; CHECK-DAG:  { i32, void ()*, i8* } { i32 65535, void ()* @a_global_ctor, i8* null }
; CHECK-DAG:  { i32, void ()*, i8* } { i32 65535, void ()* @b_global_ctor, i8* null }
; CHECK-DAG:  { i32, void ()*, i8* } { i32 65535, void ()* @c_global_ctor, i8* null }
; CHECK-DAG:  { i32, void ()*, i8* } { i32 65535, void ()* @d_global_ctor, i8* null }
; CHECK: ]
