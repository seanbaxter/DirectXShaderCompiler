; RUN: not llvm37-as < %s > /dev/null 2> %t
; RUN: FileCheck %s --input-file=%t
; CHECK: basic block pointers are invalid

define i32 @main() {
         %foo  = call i8* %llvm37.stacksave()
         %foop = bitcast i8* %foo to label*
         %nret = load label, label* %foop
         br label %nret
}
