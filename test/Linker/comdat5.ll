; RUN: not llvm37-link %s %p/Inputs/comdat4.ll -S -o - 2>&1 | FileCheck %s
target datalayout = "e-m:w-p:32:32-i64:64-f80:32-n8:16:32-S32"
target triple = "i686-pc-windows-msvc"

$foo = comdat samesize
@foo = global i32 42, comdat($foo)
; CHECK: Linking COMDATs named 'foo': SameSize violated!
