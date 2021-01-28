; RUN: llvm37-as < %s | llvm37-dis
; RUN: verify-uselistorder %s

define i32 @test() {
        ret i32 ashr (i32 ptrtoint (i32 ()* @test to i32), i32 2)
}
