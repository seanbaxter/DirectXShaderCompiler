# RUN: llvm37-mc -triple i386-unknown-unknown %s | FileCheck %s

# CHECK-NOT: foo
.extern foo
