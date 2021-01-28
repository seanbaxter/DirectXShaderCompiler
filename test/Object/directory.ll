;RUN: rm -f %T/test.a
;RUN: not llvm37-ar r %T/test.a . 2>&1 | FileCheck %s
;CHECK: .: {{I|i}}s a directory

;RUN: rm -f %T/test.a
;RUN: touch %T/a-very-long-file-name
;RUN: llvm37-ar r %T/test.a %s %T/a-very-long-file-name
;RUN: llvm37-ar r %T/test.a %T/a-very-long-file-name
;RUN: llvm37-ar t %T/test.a | FileCheck -check-prefix=MEMBERS %s
;MEMBERS-NOT: /
;MEMBERS: directory.ll
;MEMBERS: a-very-long-file-name
;MEMBERS-NOT: a-very-long-file-name
