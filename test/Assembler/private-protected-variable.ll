; RUN: not llvm37-as < %s -o /dev/null 2>&1 | FileCheck %s

@var = private protected global i32 0
; CHECK: symbol with local linkage must have default visibility
