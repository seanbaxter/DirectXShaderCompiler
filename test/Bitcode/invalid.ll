; RUN:  not llvm37-dis < %s.bc 2>&1 | FileCheck %s

; CHECK: llvm37-dis{{(\.EXE|\.exe)?}}: error: Unknown attribute kind (48)

; invalid.ll.bc has an invalid attribute number.
; The test checks that LLVM37 reports the error and doesn't access freed memory
; in doing so.
