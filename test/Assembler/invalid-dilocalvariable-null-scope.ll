; RUN: not llvm37-as < %s -disable-output 2>&1 | FileCheck %s

; CHECK: <stdin>:[[@LINE+1]]:30: error: 'scope' cannot be null
!0 = !DILocalVariable(scope: null)
