; RUN: not llvm37-as < %s -disable-output 2>&1 | FileCheck %s

; CHECK: [[@LINE+1]]:36: error: missing required field 'tag'
!25 = !DICompositeType(name: "Type")
