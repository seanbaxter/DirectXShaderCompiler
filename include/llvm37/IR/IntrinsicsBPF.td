//===- IntrinsicsBPF.td - Defines BPF intrinsics -----------*- tablegen -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines all of the BPF-specific intrinsics.
//
//===----------------------------------------------------------------------===//

let TargetPrefix = "bpf" in {  // All intrinsics start with "llvm.bpf."
  def int_bpf_load_byte : GCCBuiltin<"__builtin_bpf_load_byte">,
              Intrinsic<[llvm37_i64_ty], [llvm37_ptr_ty, llvm37_i64_ty], [IntrReadMem]>;
  def int_bpf_load_half : GCCBuiltin<"__builtin_bpf_load_half">,
              Intrinsic<[llvm37_i64_ty], [llvm37_ptr_ty, llvm37_i64_ty], [IntrReadMem]>;
  def int_bpf_load_word : GCCBuiltin<"__builtin_bpf_load_word">,
              Intrinsic<[llvm37_i64_ty], [llvm37_ptr_ty, llvm37_i64_ty], [IntrReadMem]>;
  def int_bpf_pseudo : GCCBuiltin<"__builtin_bpf_pseudo">,
              Intrinsic<[llvm37_i64_ty], [llvm37_i64_ty, llvm37_i64_ty]>;
}
