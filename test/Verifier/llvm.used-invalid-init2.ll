; RUN: not llvm37-as < %s -o /dev/null 2>&1 | FileCheck %s

@a = global i8 42
@llvm37.used = appending global [2 x i8*] [i8* @a, i8* null], section "llvm.metadata"

; CHECK: invalid llvm37.used member
; CHECK-NEXT: i8* null
