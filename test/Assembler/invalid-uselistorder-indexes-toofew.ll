; RUN: not llvm37-as < %s -disable-output 2>&1 | FileCheck %s
; CHECK: error: wrong number of indexes, expected 3
@global = global i32 0
@alias1 = alias i32* @global
@alias2 = alias i32* @global
@alias3 = alias i32* @global
uselistorder i32* @global, { 1, 0 }
