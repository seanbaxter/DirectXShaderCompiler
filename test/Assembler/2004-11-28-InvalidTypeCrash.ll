; Test for PR463.  This program is erroneous, but should not crash llvm37-as.
; RUN: not llvm37-as %s -o /dev/null 2>&1 | FileCheck %s
; CHECK: use of undefined type named 'struct.none'

@.FOO  = internal global %struct.none zeroinitializer
