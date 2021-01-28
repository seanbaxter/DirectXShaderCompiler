//==- IntrinsicsXCore.td - XCore intrinsics                 -*- tablegen -*-==//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines all of the XCore-specific intrinsics.
//
//===----------------------------------------------------------------------===//

let TargetPrefix = "xcore" in {  // All intrinsics start with "llvm.xcore.".
  // Miscellaneous instructions.
  def int_xcore_bitrev : Intrinsic<[llvm37_i32_ty],[llvm37_i32_ty],[IntrNoMem]>,
                         GCCBuiltin<"__builtin_bitrev">;
  def int_xcore_crc8 : Intrinsic<[llvm37_i32_ty, llvm37_i32_ty],
                                 [llvm37_i32_ty,llvm37_i32_ty,llvm37_i32_ty],
                                 [IntrNoMem]>;
  def int_xcore_crc32 : Intrinsic<[llvm37_i32_ty],
                                  [llvm37_i32_ty,llvm37_i32_ty,llvm37_i32_ty],
                                  [IntrNoMem]>;
  def int_xcore_sext : Intrinsic<[llvm37_i32_ty], [llvm37_i32_ty, llvm37_i32_ty],
                                 [IntrNoMem]>;
  def int_xcore_zext : Intrinsic<[llvm37_i32_ty], [llvm37_i32_ty, llvm37_i32_ty],
                                 [IntrNoMem]>;
  def int_xcore_getid : Intrinsic<[llvm37_i32_ty],[],[IntrNoMem]>,
                        GCCBuiltin<"__builtin_getid">;
  def int_xcore_getps : Intrinsic<[llvm37_i32_ty],[llvm37_i32_ty]>,
                        GCCBuiltin<"__builtin_getps">;
  def int_xcore_setps : Intrinsic<[],[llvm37_i32_ty, llvm37_i32_ty]>,
                        GCCBuiltin<"__builtin_setps">;
  def int_xcore_geted : Intrinsic<[llvm37_i32_ty],[]>;
  def int_xcore_getet : Intrinsic<[llvm37_i32_ty],[]>;
  def int_xcore_setsr : Intrinsic<[],[llvm37_i32_ty]>;
  def int_xcore_clrsr : Intrinsic<[],[llvm37_i32_ty]>;

  // Resource instructions.
  def int_xcore_getr : Intrinsic<[llvm37_anyptr_ty],[llvm37_i32_ty]>;
  def int_xcore_freer : Intrinsic<[],[llvm37_anyptr_ty],
                                   [NoCapture<0>]>;
  def int_xcore_in : Intrinsic<[llvm37_i32_ty],[llvm37_anyptr_ty],[NoCapture<0>]>;
  def int_xcore_int : Intrinsic<[llvm37_i32_ty],[llvm37_anyptr_ty],
                                [NoCapture<0>]>;
  def int_xcore_inct : Intrinsic<[llvm37_i32_ty],[llvm37_anyptr_ty],
                                 [NoCapture<0>]>;
  def int_xcore_out : Intrinsic<[],[llvm37_anyptr_ty, llvm37_i32_ty],
                                [NoCapture<0>]>;
  def int_xcore_outt : Intrinsic<[],[llvm37_anyptr_ty, llvm37_i32_ty],
                                 [NoCapture<0>]>;
  def int_xcore_outct : Intrinsic<[],[llvm37_anyptr_ty, llvm37_i32_ty],
                                  [NoCapture<0>]>;
  def int_xcore_chkct : Intrinsic<[],[llvm37_anyptr_ty, llvm37_i32_ty],
                                  [NoCapture<0>]>;
  def int_xcore_testct : Intrinsic<[llvm37_i32_ty],[llvm37_anyptr_ty],
                                   [NoCapture<0>]>;
  def int_xcore_testwct : Intrinsic<[llvm37_i32_ty],[llvm37_anyptr_ty],
                                    [NoCapture<0>]>;
  def int_xcore_setd : Intrinsic<[],[llvm37_anyptr_ty, llvm37_i32_ty],
                                  [NoCapture<0>]>;
  def int_xcore_setc : Intrinsic<[],[llvm37_anyptr_ty, llvm37_i32_ty],
                                  [NoCapture<0>]>;
  def int_xcore_inshr : Intrinsic<[llvm37_i32_ty],[llvm37_anyptr_ty, llvm37_i32_ty],
                                  [NoCapture<0>]>;
  def int_xcore_outshr : Intrinsic<[llvm37_i32_ty],[llvm37_anyptr_ty, llvm37_i32_ty],
                                  [NoCapture<0>]>;
  def int_xcore_setpt : Intrinsic<[],[llvm37_anyptr_ty, llvm37_i32_ty],
                                  [NoCapture<0>]>;
  def int_xcore_clrpt : Intrinsic<[],[llvm37_anyptr_ty],
                                  [NoCapture<0>]>;
  def int_xcore_getts : Intrinsic<[llvm37_i32_ty],[llvm37_anyptr_ty],
                                  [NoCapture<0>]>;
  def int_xcore_syncr : Intrinsic<[],[llvm37_anyptr_ty],
                                  [NoCapture<0>]>;
  def int_xcore_settw : Intrinsic<[],[llvm37_anyptr_ty, llvm37_i32_ty],
                                  [NoCapture<0>]>;
  def int_xcore_setv : Intrinsic<[],[llvm37_anyptr_ty, llvm37_ptr_ty],
                                 [NoCapture<0>]>;
  def int_xcore_setev : Intrinsic<[],[llvm37_anyptr_ty, llvm37_ptr_ty],
                                  [NoCapture<0>]>;
  def int_xcore_eeu : Intrinsic<[],[llvm37_anyptr_ty], [NoCapture<0>]>;
  def int_xcore_edu : Intrinsic<[],[llvm37_anyptr_ty], [NoCapture<0>]>;
  def int_xcore_setclk : Intrinsic<[],[llvm37_anyptr_ty, llvm37_anyptr_ty],
                                   [NoCapture<0>, NoCapture<1>]>;
  def int_xcore_setrdy : Intrinsic<[],[llvm37_anyptr_ty, llvm37_anyptr_ty],
                                   [NoCapture<0>, NoCapture<1>]>;
  def int_xcore_setpsc : Intrinsic<[],[llvm37_anyptr_ty, llvm37_i32_ty],
                                   [NoCapture<0>]>;
  def int_xcore_peek : Intrinsic<[llvm37_i32_ty],[llvm37_anyptr_ty],
                                 [NoCapture<0>]>;
  def int_xcore_endin : Intrinsic<[llvm37_i32_ty],[llvm37_anyptr_ty],
                                 [NoCapture<0>]>;

  // Intrinsics for events.
  def int_xcore_waitevent : Intrinsic<[llvm37_ptr_ty],[], [IntrReadMem]>;

  // If any of the resources owned by the thread are ready this returns the
  // vector of one of the ready resources. If no resources owned by the thread
  // are ready then the operand passed to the intrinsic is returned.
  def int_xcore_checkevent : Intrinsic<[llvm37_ptr_ty],[llvm37_ptr_ty]>;

  def int_xcore_clre : Intrinsic<[],[],[]>;

  // Intrinsics for threads.
  def int_xcore_getst : Intrinsic <[llvm37_anyptr_ty],[llvm37_anyptr_ty],
                                   [NoCapture<0>]>;
  def int_xcore_msync : Intrinsic <[],[llvm37_anyptr_ty], [NoCapture<0>]>;
  def int_xcore_ssync : Intrinsic <[],[]>;
  def int_xcore_mjoin : Intrinsic <[],[llvm37_anyptr_ty], [NoCapture<0>]>;
  def int_xcore_initsp : Intrinsic <[],[llvm37_anyptr_ty, llvm37_ptr_ty],
                                    [NoCapture<0>]>;
  def int_xcore_initpc : Intrinsic <[],[llvm37_anyptr_ty, llvm37_ptr_ty],
                                    [NoCapture<0>]>;
  def int_xcore_initlr : Intrinsic <[],[llvm37_anyptr_ty, llvm37_ptr_ty],
                                    [NoCapture<0>]>;
  def int_xcore_initcp : Intrinsic <[],[llvm37_anyptr_ty, llvm37_ptr_ty],
                                    [NoCapture<0>]>;
  def int_xcore_initdp : Intrinsic <[],[llvm37_anyptr_ty, llvm37_ptr_ty],
                                    [NoCapture<0>]>;
}
