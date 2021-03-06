// RUN: grep -Ev "// *[A-Z-]+:" %s > %t.cpp
// RUN: clang-format -style=LLVM37 -i %t.cpp
// RUN: FileCheck -strict-whitespace -input-file=%t.cpp %s

// CHECK: {{^int\ \*i;}}
 int   *  i  ;
