; RUN: not llvm37-lto %S/Inputs/invalid.ll.bc 2>&1 | FileCheck %s


; CHECK: llvm37-lto{{.*}}: error loading file '{{.*}}/Inputs/invalid.ll.bc': Unknown attribute kind (48)
