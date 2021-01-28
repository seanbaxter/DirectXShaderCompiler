; This is not an assembly file, this is just to run the test.
; The test verifies that llvm37-ar produces a binary output.

;RUN: llvm37-ar p %p/Inputs/GNU.a very_long_bytecode_file_name.bc | cmp -s %p/Inputs/very_long_bytecode_file_name.bc -
