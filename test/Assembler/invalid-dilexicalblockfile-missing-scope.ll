; RUN: not llvm37-as < %s -disable-output 2>&1 | FileCheck %s

; CHECK: [[@LINE+1]]:42: error: missing required field 'scope'
!0 = !DILexicalBlockFile(discriminator: 0)
