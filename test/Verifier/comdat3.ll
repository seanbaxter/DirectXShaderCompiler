; This used to be invalid, but now it's valid.  Ensure the verifier
; doesn't reject it.
; RUN: llvm37-as %s -o /dev/null

$v = comdat largest
