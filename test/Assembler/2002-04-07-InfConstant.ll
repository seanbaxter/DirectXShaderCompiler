; The output formater prints out 1.0e100 as Inf!
;
; RUN: llvm37-as < %s | llvm37-dis | llvm37-as | llvm37-dis | grep 0x7FF0000000000000
; RUN: verify-uselistorder %s

define float @test() {
        %tmp = fmul float 0x7FF0000000000000, 1.000000e+01               ; <float> [#uses=1]
        ret float %tmp
}

