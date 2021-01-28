//===- Intrinsics.td - Defines all LLVM37 intrinsics ---------*- tablegen -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines properties of all LLVM37 intrinsics.
//
//===----------------------------------------------------------------------===//

include "llvm37/CodeGen/ValueTypes.td"

//===----------------------------------------------------------------------===//
//  Properties we keep track of for intrinsics.
//===----------------------------------------------------------------------===//

class IntrinsicProperty;

// Intr*Mem - Memory properties.  An intrinsic is allowed to have at most one of
// these properties set.  They are listed from the most aggressive (best to use
// if correct) to the least aggressive.  If no property is set, the worst case
// is assumed (it may read and write any memory it can get access to and it may
// have other side effects).

// IntrNoMem - The intrinsic does not access memory or have any other side
// effects.  It may be CSE'd deleted if dead, etc.
def IntrNoMem : IntrinsicProperty;

// IntrReadArgMem - This intrinsic reads only from memory that one of its
// pointer-typed arguments points to, but may read an unspecified amount.
def IntrReadArgMem : IntrinsicProperty;

// IntrReadMem - This intrinsic reads from unspecified memory, so it cannot be
// moved across stores.  However, it can be reordered otherwise and can be
// deleted if dead.
def IntrReadMem : IntrinsicProperty;

// IntrReadWriteArgMem - This intrinsic reads and writes only from memory that
// one of its arguments points to, but may access an unspecified amount.  The
// reads and writes may be volatile, but except for this it has no other side
// effects.
def IntrReadWriteArgMem : IntrinsicProperty;

// Commutative - This intrinsic is commutative: X op Y == Y op X.
def Commutative : IntrinsicProperty;

// Throws - This intrinsic can throw.
def Throws : IntrinsicProperty;

// NoCapture - The specified argument pointer is not captured by the intrinsic.
class NoCapture<int argNo> : IntrinsicProperty {
  int ArgNo = argNo;
}

// ReadOnly - The specified argument pointer is not written to through the
// pointer by the intrinsic.
class ReadOnly<int argNo> : IntrinsicProperty {
  int ArgNo = argNo;
}

// ReadNone - The specified argument pointer is not dereferenced by the
// intrinsic.
class ReadNone<int argNo> : IntrinsicProperty {
  int ArgNo = argNo;
}

def IntrNoReturn : IntrinsicProperty;

// IntrNoduplicate - Calls to this intrinsic cannot be duplicated.
// Parallels the noduplicate attribute on LLVM37 IR functions.
def IntrNoDuplicate : IntrinsicProperty;

// IntrConvergent - Calls to this intrinsic are convergent and may only be
// moved to control equivalent blocks.
// Parallels the convergent attribute on LLVM37 IR functions.
def IntrConvergent : IntrinsicProperty;

//===----------------------------------------------------------------------===//
// Types used by intrinsics.
//===----------------------------------------------------------------------===//

class LLVM37Type<ValueType vt> {
  ValueType VT = vt;
}

class LLVM37QualPointerType<LLVM37Type elty, int addrspace>
  : LLVM37Type<iPTR>{
  LLVM37Type ElTy = elty;
  int AddrSpace = addrspace;
}

class LLVM37PointerType<LLVM37Type elty>
  : LLVM37QualPointerType<elty, 0>;

class LLVM37AnyPointerType<LLVM37Type elty>
  : LLVM37Type<iPTRAny>{
  LLVM37Type ElTy = elty;
}

// Match the type of another intrinsic parameter.  Number is an index into the
// list of overloaded types for the intrinsic, excluding all the fixed types.
// The Number value must refer to a previously listed type.  For example:
//   Intrinsic<[llvm37_i32_ty], [llvm37_i32_ty, llvm37_anyfloat_ty, LLVM37MatchType<0>]>
// has two overloaded types, the 2nd and 3rd arguments.  LLVM37MatchType<0>
// refers to the first overloaded type, which is the 2nd argument.
class LLVM37MatchType<int num>
  : LLVM37Type<OtherVT>{
  int Number = num;
}

// Match the type of another intrinsic parameter that is expected to be based on
// an integral type (i.e. either iN or <N x iM>), but change the scalar size to
// be twice as wide or half as wide as the other type.  This is only useful when
// the intrinsic is overloaded, so the matched type should be declared as iAny.
class LLVM37ExtendedType<int num> : LLVM37MatchType<num>;
class LLVM37TruncatedType<int num> : LLVM37MatchType<num>;
class LLVM37VectorSameWidth<int num, LLVM37Type elty>
  : LLVM37MatchType<num> {
  ValueType ElTy = elty.VT;
}
class LLVM37PointerTo<int num> : LLVM37MatchType<num>;
class LLVM37VectorOfPointersToElt<int num> : LLVM37MatchType<num>;

// Match the type of another intrinsic parameter that is expected to be a
// vector type, but change the element count to be half as many
class LLVM37HalfElementsVectorType<int num> : LLVM37MatchType<num>;

def llvm37_void_ty       : LLVM37Type<isVoid>;
def llvm37_any_ty        : LLVM37Type<Any>;
def llvm37_anyint_ty     : LLVM37Type<iAny>;
def llvm37_anyfloat_ty   : LLVM37Type<fAny>;
def llvm37_anyvector_ty  : LLVM37Type<vAny>;
def llvm37_i1_ty         : LLVM37Type<i1>;
def llvm37_i8_ty         : LLVM37Type<i8>;
def llvm37_i16_ty        : LLVM37Type<i16>;
def llvm37_i32_ty        : LLVM37Type<i32>;
def llvm37_i64_ty        : LLVM37Type<i64>;
def llvm37_half_ty       : LLVM37Type<f16>;
def llvm37_float_ty      : LLVM37Type<f32>;
def llvm37_double_ty     : LLVM37Type<f64>;
def llvm37_f80_ty        : LLVM37Type<f80>;
def llvm37_f128_ty       : LLVM37Type<f128>;
def llvm37_ppcf128_ty    : LLVM37Type<ppcf128>;
def llvm37_ptr_ty        : LLVM37PointerType<llvm37_i8_ty>;             // i8*
def llvm37_ptrptr_ty     : LLVM37PointerType<llvm37_ptr_ty>;            // i8**
def llvm37_anyptr_ty     : LLVM37AnyPointerType<llvm37_i8_ty>;          // (space)i8*
def llvm37_empty_ty      : LLVM37Type<OtherVT>;                       // { }
def llvm37_descriptor_ty : LLVM37PointerType<llvm37_empty_ty>;          // { }*
def llvm37_metadata_ty   : LLVM37Type<MetadataVT>;                    // !{...}

def llvm37_x86mmx_ty     : LLVM37Type<x86mmx>;
def llvm37_ptrx86mmx_ty  : LLVM37PointerType<llvm37_x86mmx_ty>;         // <1 x i64>*

def llvm37_v2i1_ty       : LLVM37Type<v2i1>;     //  2 x i1
def llvm37_v4i1_ty       : LLVM37Type<v4i1>;     //  4 x i1
def llvm37_v8i1_ty       : LLVM37Type<v8i1>;     //  8 x i1
def llvm37_v16i1_ty      : LLVM37Type<v16i1>;    // 16 x i1
def llvm37_v32i1_ty      : LLVM37Type<v32i1>;    // 32 x i1
def llvm37_v64i1_ty      : LLVM37Type<v64i1>;    // 64 x i1
def llvm37_v1i8_ty       : LLVM37Type<v1i8>;     //  1 x i8
def llvm37_v2i8_ty       : LLVM37Type<v2i8>;     //  2 x i8
def llvm37_v4i8_ty       : LLVM37Type<v4i8>;     //  4 x i8
def llvm37_v8i8_ty       : LLVM37Type<v8i8>;     //  8 x i8
def llvm37_v16i8_ty      : LLVM37Type<v16i8>;    // 16 x i8
def llvm37_v32i8_ty      : LLVM37Type<v32i8>;    // 32 x i8
def llvm37_v64i8_ty      : LLVM37Type<v64i8>;    // 64 x i8

def llvm37_v1i16_ty      : LLVM37Type<v1i16>;    //  1 x i16
def llvm37_v2i16_ty      : LLVM37Type<v2i16>;    //  2 x i16
def llvm37_v4i16_ty      : LLVM37Type<v4i16>;    //  4 x i16
def llvm37_v8i16_ty      : LLVM37Type<v8i16>;    //  8 x i16
def llvm37_v16i16_ty     : LLVM37Type<v16i16>;   // 16 x i16
def llvm37_v32i16_ty     : LLVM37Type<v32i16>;   // 32 x i16

def llvm37_v1i32_ty      : LLVM37Type<v1i32>;    //  1 x i32
def llvm37_v2i32_ty      : LLVM37Type<v2i32>;    //  2 x i32
def llvm37_v4i32_ty      : LLVM37Type<v4i32>;    //  4 x i32
def llvm37_v8i32_ty      : LLVM37Type<v8i32>;    //  8 x i32
def llvm37_v16i32_ty     : LLVM37Type<v16i32>;   // 16 x i32
def llvm37_v1i64_ty      : LLVM37Type<v1i64>;    //  1 x i64
def llvm37_v2i64_ty      : LLVM37Type<v2i64>;    //  2 x i64
def llvm37_v4i64_ty      : LLVM37Type<v4i64>;    //  4 x i64
def llvm37_v8i64_ty      : LLVM37Type<v8i64>;    //  8 x i64
def llvm37_v16i64_ty     : LLVM37Type<v16i64>;   // 16 x i64

def llvm37_v1i128_ty     : LLVM37Type<v1i128>;   //  1 x i128

def llvm37_v2f16_ty      : LLVM37Type<v2f16>;    //  2 x half (__fp16)
def llvm37_v4f16_ty      : LLVM37Type<v4f16>;    //  4 x half (__fp16)
def llvm37_v8f16_ty      : LLVM37Type<v8f16>;    //  8 x half (__fp16)
def llvm37_v1f32_ty      : LLVM37Type<v1f32>;    //  1 x float
def llvm37_v2f32_ty      : LLVM37Type<v2f32>;    //  2 x float
def llvm37_v4f32_ty      : LLVM37Type<v4f32>;    //  4 x float
def llvm37_v8f32_ty      : LLVM37Type<v8f32>;    //  8 x float
def llvm37_v16f32_ty     : LLVM37Type<v16f32>;   // 16 x float
def llvm37_v1f64_ty      : LLVM37Type<v1f64>;    //  1 x double
def llvm37_v2f64_ty      : LLVM37Type<v2f64>;    //  2 x double
def llvm37_v4f64_ty      : LLVM37Type<v4f64>;    //  4 x double
def llvm37_v8f64_ty      : LLVM37Type<v8f64>;    //  8 x double

def llvm37_vararg_ty     : LLVM37Type<isVoid>;   // this means vararg here


//===----------------------------------------------------------------------===//
// Intrinsic Definitions.
//===----------------------------------------------------------------------===//

// Intrinsic class - This is used to define one LLVM37 intrinsic.  The name of the
// intrinsic definition should start with "int_", then match the LLVM37 intrinsic
// name with the "llvm." prefix removed, and all "."s turned into "_"s.  For
// example, llvm37.bswap.i16 -> int_bswap_i16.
//
//  * RetTypes is a list containing the return types expected for the
//    intrinsic.
//  * ParamTypes is a list containing the parameter types expected for the
//    intrinsic.
//  * Properties can be set to describe the behavior of the intrinsic.
//
class SDPatternOperator;
class Intrinsic<list<LLVM37Type> ret_types,
                list<LLVM37Type> param_types = [],
                list<IntrinsicProperty> properties = [],
                string name = ""> : SDPatternOperator {
  string LLVM37Name = name;
  string TargetPrefix = "";   // Set to a prefix for target-specific intrinsics.
  list<LLVM37Type> RetTypes = ret_types;
  list<LLVM37Type> ParamTypes = param_types;
  list<IntrinsicProperty> Properties = properties;

  bit isTarget = 0;
}

/// GCCBuiltin - If this intrinsic exactly corresponds to a GCC builtin, this
/// specifies the name of the builtin.  This provides automatic CBE and CFE
/// support.
class GCCBuiltin<string name> {
  string GCCBuiltinName = name;
}

class MSBuiltin<string name> {
  string MSBuiltinName = name;
}


//===--------------- Variable Argument Handling Intrinsics ----------------===//
//

def int_vastart : Intrinsic<[], [llvm37_ptr_ty], [], "llvm.va_start">;
def int_vacopy  : Intrinsic<[], [llvm37_ptr_ty, llvm37_ptr_ty], [],
                            "llvm.va_copy">;
def int_vaend   : Intrinsic<[], [llvm37_ptr_ty], [], "llvm.va_end">;

//===------------------- Garbage Collection Intrinsics --------------------===//
//
def int_gcroot  : Intrinsic<[],
                            [llvm37_ptrptr_ty, llvm37_ptr_ty]>;
def int_gcread  : Intrinsic<[llvm37_ptr_ty],
                            [llvm37_ptr_ty, llvm37_ptrptr_ty],
                            [IntrReadArgMem]>;
def int_gcwrite : Intrinsic<[],
                            [llvm37_ptr_ty, llvm37_ptr_ty, llvm37_ptrptr_ty],
                            [IntrReadWriteArgMem, NoCapture<1>, NoCapture<2>]>;

//===--------------------- Code Generator Intrinsics ----------------------===//
//
def int_returnaddress : Intrinsic<[llvm37_ptr_ty], [llvm37_i32_ty], [IntrNoMem]>;
def int_frameaddress  : Intrinsic<[llvm37_ptr_ty], [llvm37_i32_ty], [IntrNoMem]>;
def int_read_register  : Intrinsic<[llvm37_anyint_ty], [llvm37_metadata_ty],
                                   [IntrReadMem], "llvm.read_register">;
def int_write_register : Intrinsic<[], [llvm37_metadata_ty, llvm37_anyint_ty],
                                   [], "llvm.write_register">;

// Gets the address of the local variable area. This is typically a copy of the
// stack, frame, or base pointer depending on the type of prologue.
def int_localaddress : Intrinsic<[llvm37_ptr_ty], [], [IntrNoMem]>;

// Escapes local variables to allow access from other functions.
def int_localescape : Intrinsic<[], [llvm37_vararg_ty]>;

// Given a function and the localaddress of a parent frame, returns a pointer
// to an escaped allocation indicated by the index.
def int_localrecover : Intrinsic<[llvm37_ptr_ty],
                                 [llvm37_ptr_ty, llvm37_ptr_ty, llvm37_i32_ty],
                                 [IntrNoMem]>;
// Note: we treat stacksave/stackrestore as writemem because we don't otherwise
// model their dependencies on allocas.
def int_stacksave     : Intrinsic<[llvm37_ptr_ty]>,
                        GCCBuiltin<"__builtin_stack_save">;
def int_stackrestore  : Intrinsic<[], [llvm37_ptr_ty]>,
                        GCCBuiltin<"__builtin_stack_restore">;

// IntrReadWriteArgMem is more pessimistic than strictly necessary for prefetch,
// however it does conveniently prevent the prefetch from being reordered
// with respect to nearby accesses to the same memory.
def int_prefetch      : Intrinsic<[],
                                  [llvm37_ptr_ty, llvm37_i32_ty, llvm37_i32_ty,
                                   llvm37_i32_ty],
                                  [IntrReadWriteArgMem, NoCapture<0>]>;
def int_pcmarker      : Intrinsic<[], [llvm37_i32_ty]>;

def int_readcyclecounter : Intrinsic<[llvm37_i64_ty]>;

// The assume intrinsic is marked as arbitrarily writing so that proper
// control dependencies will be maintained.
def int_assume        : Intrinsic<[], [llvm37_i1_ty], []>;

// Stack Protector Intrinsic - The stackprotector intrinsic writes the stack
// guard to the correct place on the stack frame.
def int_stackprotector : Intrinsic<[], [llvm37_ptr_ty, llvm37_ptrptr_ty], []>;
def int_stackprotectorcheck : Intrinsic<[], [llvm37_ptrptr_ty],
                                        [IntrReadWriteArgMem]>;

// A counter increment for instrumentation based profiling.
def int_instrprof_increment : Intrinsic<[],
                                        [llvm37_ptr_ty, llvm37_i64_ty,
                                         llvm37_i32_ty, llvm37_i32_ty],
                                        []>;

//===------------------- Standard C Library Intrinsics --------------------===//
//

def int_memcpy  : Intrinsic<[],
                             [llvm37_anyptr_ty, llvm37_anyptr_ty, llvm37_anyint_ty,
                              llvm37_i32_ty, llvm37_i1_ty],
                            [IntrReadWriteArgMem, NoCapture<0>, NoCapture<1>,
                             ReadOnly<1>]>;
def int_memmove : Intrinsic<[],
                            [llvm37_anyptr_ty, llvm37_anyptr_ty, llvm37_anyint_ty,
                             llvm37_i32_ty, llvm37_i1_ty],
                            [IntrReadWriteArgMem, NoCapture<0>, NoCapture<1>,
                             ReadOnly<1>]>;
def int_memset  : Intrinsic<[],
                            [llvm37_anyptr_ty, llvm37_i8_ty, llvm37_anyint_ty,
                             llvm37_i32_ty, llvm37_i1_ty],
                            [IntrReadWriteArgMem, NoCapture<0>]>;

let Properties = [IntrNoMem] in {
  def int_fma  : Intrinsic<[llvm37_anyfloat_ty],
                           [LLVM37MatchType<0>, LLVM37MatchType<0>,
                            LLVM37MatchType<0>]>;
  def int_fmuladd : Intrinsic<[llvm37_anyfloat_ty],
                              [LLVM37MatchType<0>, LLVM37MatchType<0>,
                               LLVM37MatchType<0>]>;

  // These functions do not read memory, but are sensitive to the
  // rounding mode. LLVM37 purposely does not model changes to the FP
  // environment so they can be treated as readnone.
  def int_sqrt : Intrinsic<[llvm37_anyfloat_ty], [LLVM37MatchType<0>]>;
  def int_powi : Intrinsic<[llvm37_anyfloat_ty], [LLVM37MatchType<0>, llvm37_i32_ty]>;
  def int_sin  : Intrinsic<[llvm37_anyfloat_ty], [LLVM37MatchType<0>]>;
  def int_cos  : Intrinsic<[llvm37_anyfloat_ty], [LLVM37MatchType<0>]>;
  def int_pow  : Intrinsic<[llvm37_anyfloat_ty],
                           [LLVM37MatchType<0>, LLVM37MatchType<0>]>;
  def int_log  : Intrinsic<[llvm37_anyfloat_ty], [LLVM37MatchType<0>]>;
  def int_log10: Intrinsic<[llvm37_anyfloat_ty], [LLVM37MatchType<0>]>;
  def int_log2 : Intrinsic<[llvm37_anyfloat_ty], [LLVM37MatchType<0>]>;
  def int_exp  : Intrinsic<[llvm37_anyfloat_ty], [LLVM37MatchType<0>]>;
  def int_exp2 : Intrinsic<[llvm37_anyfloat_ty], [LLVM37MatchType<0>]>;
  def int_fabs : Intrinsic<[llvm37_anyfloat_ty], [LLVM37MatchType<0>]>;
  def int_minnum : Intrinsic<[llvm37_anyfloat_ty], [LLVM37MatchType<0>, LLVM37MatchType<0>]>;
  def int_maxnum : Intrinsic<[llvm37_anyfloat_ty], [LLVM37MatchType<0>, LLVM37MatchType<0>]>;
  def int_copysign : Intrinsic<[llvm37_anyfloat_ty],
                               [LLVM37MatchType<0>, LLVM37MatchType<0>]>;
  def int_floor : Intrinsic<[llvm37_anyfloat_ty], [LLVM37MatchType<0>]>;
  def int_ceil  : Intrinsic<[llvm37_anyfloat_ty], [LLVM37MatchType<0>]>;
  def int_trunc : Intrinsic<[llvm37_anyfloat_ty], [LLVM37MatchType<0>]>;
  def int_rint  : Intrinsic<[llvm37_anyfloat_ty], [LLVM37MatchType<0>]>;
  def int_nearbyint : Intrinsic<[llvm37_anyfloat_ty], [LLVM37MatchType<0>]>;
  def int_round : Intrinsic<[llvm37_anyfloat_ty], [LLVM37MatchType<0>]>;
  def int_canonicalize : Intrinsic<[llvm37_anyfloat_ty], [LLVM37MatchType<0>],
                                   [IntrNoMem]>;
}

// NOTE: these are internal interfaces.
def int_setjmp     : Intrinsic<[llvm37_i32_ty],  [llvm37_ptr_ty]>;
def int_longjmp    : Intrinsic<[], [llvm37_ptr_ty, llvm37_i32_ty], [IntrNoReturn]>;
def int_sigsetjmp  : Intrinsic<[llvm37_i32_ty] , [llvm37_ptr_ty, llvm37_i32_ty]>;
def int_siglongjmp : Intrinsic<[], [llvm37_ptr_ty, llvm37_i32_ty], [IntrNoReturn]>;

// Internal interface for object size checking
def int_objectsize : Intrinsic<[llvm37_anyint_ty], [llvm37_anyptr_ty, llvm37_i1_ty],
                               [IntrNoMem]>,
                               GCCBuiltin<"__builtin_object_size">;

//===------------------------- Expect Intrinsics --------------------------===//
//
def int_expect : Intrinsic<[llvm37_anyint_ty], [LLVM37MatchType<0>,
                                              LLVM37MatchType<0>], [IntrNoMem]>;

//===-------------------- Bit Manipulation Intrinsics ---------------------===//
//

// None of these intrinsics accesses memory at all.
let Properties = [IntrNoMem] in {
  def int_bswap: Intrinsic<[llvm37_anyint_ty], [LLVM37MatchType<0>]>;
  def int_ctpop: Intrinsic<[llvm37_anyint_ty], [LLVM37MatchType<0>]>;
  def int_ctlz : Intrinsic<[llvm37_anyint_ty], [LLVM37MatchType<0>, llvm37_i1_ty]>;
  def int_cttz : Intrinsic<[llvm37_anyint_ty], [LLVM37MatchType<0>, llvm37_i1_ty]>;
}

//===------------------------ Debugger Intrinsics -------------------------===//
//

// None of these intrinsics accesses memory at all...but that doesn't mean the
// optimizers can change them aggressively.  Special handling needed in a few
// places.
let Properties = [IntrNoMem] in {
  def int_dbg_declare      : Intrinsic<[],
                                       [llvm37_metadata_ty,
                                       llvm37_metadata_ty,
                                       llvm37_metadata_ty]>;
  def int_dbg_value        : Intrinsic<[],
                                       [llvm37_metadata_ty, llvm37_i64_ty,
                                        llvm37_metadata_ty,
                                        llvm37_metadata_ty]>;
}

//===------------------ Exception Handling Intrinsics----------------------===//
//

// The result of eh.typeid.for depends on the enclosing function, but inside a
// given function it is 'const' and may be CSE'd etc.
def int_eh_typeid_for : Intrinsic<[llvm37_i32_ty], [llvm37_ptr_ty], [IntrNoMem]>;

def int_eh_return_i32 : Intrinsic<[], [llvm37_i32_ty, llvm37_ptr_ty]>;
def int_eh_return_i64 : Intrinsic<[], [llvm37_i64_ty, llvm37_ptr_ty]>;

// eh.begincatch takes a pointer returned by a landingpad instruction and
// copies the exception object into the memory pointed to by the second
// parameter. If the second parameter is null, no copy occurs.
def int_eh_begincatch : Intrinsic<[], [llvm37_ptr_ty, llvm37_ptr_ty],
                                  [NoCapture<0>, NoCapture<1>]>;
def int_eh_endcatch : Intrinsic<[], []>;

// Represents the list of actions to take when an exception is thrown.
def int_eh_actions : Intrinsic<[llvm37_ptr_ty], [llvm37_vararg_ty], []>;

def int_eh_exceptioncode : Intrinsic<[llvm37_i32_ty], [], [IntrReadMem]>;

// __builtin_unwind_init is an undocumented GCC intrinsic that causes all
// callee-saved registers to be saved and restored (regardless of whether they
// are used) in the calling function. It is used by libgcc_eh.
def int_eh_unwind_init: Intrinsic<[]>,
                        GCCBuiltin<"__builtin_unwind_init">;

def int_eh_dwarf_cfa  : Intrinsic<[llvm37_ptr_ty], [llvm37_i32_ty]>;

let Properties = [IntrNoMem] in {
  def int_eh_sjlj_lsda             : Intrinsic<[llvm37_ptr_ty]>;
  def int_eh_sjlj_callsite         : Intrinsic<[], [llvm37_i32_ty]>;
}
def int_eh_sjlj_functioncontext : Intrinsic<[], [llvm37_ptr_ty]>;
def int_eh_sjlj_setjmp          : Intrinsic<[llvm37_i32_ty], [llvm37_ptr_ty]>;
def int_eh_sjlj_longjmp         : Intrinsic<[], [llvm37_ptr_ty], [IntrNoReturn]>;

//===---------------- Generic Variable Attribute Intrinsics----------------===//
//
def int_var_annotation : Intrinsic<[],
                                   [llvm37_ptr_ty, llvm37_ptr_ty,
                                    llvm37_ptr_ty, llvm37_i32_ty],
                                   [], "llvm.var.annotation">;
def int_ptr_annotation : Intrinsic<[LLVM37AnyPointerType<llvm37_anyint_ty>],
                                   [LLVM37MatchType<0>, llvm37_ptr_ty, llvm37_ptr_ty,
                                    llvm37_i32_ty],
                                   [], "llvm.ptr.annotation">;
def int_annotation : Intrinsic<[llvm37_anyint_ty],
                               [LLVM37MatchType<0>, llvm37_ptr_ty,
                                llvm37_ptr_ty, llvm37_i32_ty],
                               [], "llvm.annotation">;

//===------------------------ Trampoline Intrinsics -----------------------===//
//
def int_init_trampoline : Intrinsic<[],
                                    [llvm37_ptr_ty, llvm37_ptr_ty, llvm37_ptr_ty],
                                    [IntrReadWriteArgMem, NoCapture<0>]>,
                                   GCCBuiltin<"__builtin_init_trampoline">;

def int_adjust_trampoline : Intrinsic<[llvm37_ptr_ty], [llvm37_ptr_ty],
                                      [IntrReadArgMem]>,
                                     GCCBuiltin<"__builtin_adjust_trampoline">;

//===------------------------ Overflow Intrinsics -------------------------===//
//

// Expose the carry flag from add operations on two integrals.
def int_sadd_with_overflow : Intrinsic<[llvm37_anyint_ty, llvm37_i1_ty],
                                       [LLVM37MatchType<0>, LLVM37MatchType<0>],
                                       [IntrNoMem]>;
def int_uadd_with_overflow : Intrinsic<[llvm37_anyint_ty, llvm37_i1_ty],
                                       [LLVM37MatchType<0>, LLVM37MatchType<0>],
                                       [IntrNoMem]>;

def int_ssub_with_overflow : Intrinsic<[llvm37_anyint_ty, llvm37_i1_ty],
                                       [LLVM37MatchType<0>, LLVM37MatchType<0>],
                                       [IntrNoMem]>;
def int_usub_with_overflow : Intrinsic<[llvm37_anyint_ty, llvm37_i1_ty],
                                       [LLVM37MatchType<0>, LLVM37MatchType<0>],
                                       [IntrNoMem]>;

def int_smul_with_overflow : Intrinsic<[llvm37_anyint_ty, llvm37_i1_ty],
                                       [LLVM37MatchType<0>, LLVM37MatchType<0>],
                                       [IntrNoMem]>;
def int_umul_with_overflow : Intrinsic<[llvm37_anyint_ty, llvm37_i1_ty],
                                       [LLVM37MatchType<0>, LLVM37MatchType<0>],
                                       [IntrNoMem]>;

//===------------------------- Memory Use Markers -------------------------===//
//
def int_lifetime_start  : Intrinsic<[],
                                    [llvm37_i64_ty, llvm37_ptr_ty],
                                    [IntrReadWriteArgMem, NoCapture<1>]>;
def int_lifetime_end    : Intrinsic<[],
                                    [llvm37_i64_ty, llvm37_ptr_ty],
                                    [IntrReadWriteArgMem, NoCapture<1>]>;
def int_invariant_start : Intrinsic<[llvm37_descriptor_ty],
                                    [llvm37_i64_ty, llvm37_ptr_ty],
                                    [IntrReadWriteArgMem, NoCapture<1>]>;
def int_invariant_end   : Intrinsic<[],
                                    [llvm37_descriptor_ty, llvm37_i64_ty,
                                     llvm37_ptr_ty],
                                    [IntrReadWriteArgMem, NoCapture<2>]>;

//===------------------------ Stackmap Intrinsics -------------------------===//
//
def int_experimental_stackmap : Intrinsic<[],
                                  [llvm37_i64_ty, llvm37_i32_ty, llvm37_vararg_ty],
                                  [Throws]>;
def int_experimental_patchpoint_void : Intrinsic<[],
                                                 [llvm37_i64_ty, llvm37_i32_ty,
                                                  llvm37_ptr_ty, llvm37_i32_ty,
                                                  llvm37_vararg_ty],
                                                  [Throws]>;
def int_experimental_patchpoint_i64 : Intrinsic<[llvm37_i64_ty],
                                                [llvm37_i64_ty, llvm37_i32_ty,
                                                 llvm37_ptr_ty, llvm37_i32_ty,
                                                 llvm37_vararg_ty],
                                                 [Throws]>;


//===------------------------ Garbage Collection Intrinsics ---------------===//
// These are documented in docs/Statepoint.rst

def int_experimental_gc_statepoint : Intrinsic<[llvm37_i32_ty],
                               [llvm37_i64_ty, llvm37_i32_ty,
                                llvm37_anyptr_ty, llvm37_i32_ty,
                                llvm37_i32_ty, llvm37_vararg_ty],
                                [Throws]>;

def int_experimental_gc_result   : Intrinsic<[llvm37_any_ty], [llvm37_i32_ty]>;
def int_experimental_gc_relocate : Intrinsic<[llvm37_anyptr_ty],
                                [llvm37_i32_ty, llvm37_i32_ty, llvm37_i32_ty]>;

// Deprecated: will be removed in a couple of weeks
def int_experimental_gc_result_int : Intrinsic<[llvm37_anyint_ty], [llvm37_i32_ty]>;
def int_experimental_gc_result_float : Intrinsic<[llvm37_anyfloat_ty],
                                                 [llvm37_i32_ty]>;
def int_experimental_gc_result_ptr : Intrinsic<[llvm37_anyptr_ty], [llvm37_i32_ty]>;

//===-------------------------- Other Intrinsics --------------------------===//
//
def int_flt_rounds : Intrinsic<[llvm37_i32_ty]>,
                     GCCBuiltin<"__builtin_flt_rounds">;
def int_trap : Intrinsic<[], [], [IntrNoReturn]>,
               GCCBuiltin<"__builtin_trap">;
def int_debugtrap : Intrinsic<[]>,
                    GCCBuiltin<"__builtin_debugtrap">;

// NOP: calls/invokes to this intrinsic are removed by codegen
def int_donothing : Intrinsic<[], [], [IntrNoMem]>;

// Intrisics to support half precision floating point format
let Properties = [IntrNoMem] in {
def int_convert_to_fp16   : Intrinsic<[llvm37_i16_ty], [llvm37_anyfloat_ty]>;
def int_convert_from_fp16 : Intrinsic<[llvm37_anyfloat_ty], [llvm37_i16_ty]>;
}

// These convert intrinsics are to support various conversions between
// various types with rounding and saturation. NOTE: avoid using these
// intrinsics as they might be removed sometime in the future and
// most targets don't support them.
def int_convertff  : Intrinsic<[llvm37_anyfloat_ty],
                               [llvm37_anyfloat_ty, llvm37_i32_ty, llvm37_i32_ty]>;
def int_convertfsi : Intrinsic<[llvm37_anyfloat_ty],
                               [llvm37_anyint_ty, llvm37_i32_ty, llvm37_i32_ty]>;
def int_convertfui : Intrinsic<[llvm37_anyfloat_ty],
                               [llvm37_anyint_ty, llvm37_i32_ty, llvm37_i32_ty]>;
def int_convertsif : Intrinsic<[llvm37_anyint_ty],
                               [llvm37_anyfloat_ty, llvm37_i32_ty, llvm37_i32_ty]>;
def int_convertuif : Intrinsic<[llvm37_anyint_ty],
                               [llvm37_anyfloat_ty, llvm37_i32_ty, llvm37_i32_ty]>;
def int_convertss  : Intrinsic<[llvm37_anyint_ty],
                               [llvm37_anyint_ty, llvm37_i32_ty, llvm37_i32_ty]>;
def int_convertsu  : Intrinsic<[llvm37_anyint_ty],
                               [llvm37_anyint_ty, llvm37_i32_ty, llvm37_i32_ty]>;
def int_convertus  : Intrinsic<[llvm37_anyint_ty],
                               [llvm37_anyint_ty, llvm37_i32_ty, llvm37_i32_ty]>;
def int_convertuu  : Intrinsic<[llvm37_anyint_ty],
                               [llvm37_anyint_ty, llvm37_i32_ty, llvm37_i32_ty]>;

// Clear cache intrinsic, default to ignore (ie. emit nothing)
// maps to void __clear_cache() on supporting platforms
def int_clear_cache : Intrinsic<[], [llvm37_ptr_ty, llvm37_ptr_ty],
                                [], "llvm.clear_cache">;

//===-------------------------- Masked Intrinsics -------------------------===//
//
def int_masked_store : Intrinsic<[], [llvm37_anyvector_ty, LLVM37PointerTo<0>,
                                      llvm37_i32_ty,
                                      LLVM37VectorSameWidth<0, llvm37_i1_ty>],
                                 [IntrReadWriteArgMem]>;

def int_masked_load  : Intrinsic<[llvm37_anyvector_ty],
                                 [LLVM37PointerTo<0>, llvm37_i32_ty,
                                  LLVM37VectorSameWidth<0, llvm37_i1_ty>, LLVM37MatchType<0>],
                                 [IntrReadArgMem]>;

def int_masked_gather: Intrinsic<[llvm37_anyvector_ty],
                                 [LLVM37VectorOfPointersToElt<0>, llvm37_i32_ty,
                                  LLVM37VectorSameWidth<0, llvm37_i1_ty>,
                                  LLVM37MatchType<0>],
                                 [IntrReadArgMem]>;

def int_masked_scatter: Intrinsic<[],
                                  [llvm37_anyvector_ty,
                                   LLVM37VectorOfPointersToElt<0>, llvm37_i32_ty,
                                   LLVM37VectorSameWidth<0, llvm37_i1_ty>],
                                  [IntrReadWriteArgMem]>;

// Intrinsics to support bit sets.
def int_bitset_test : Intrinsic<[llvm37_i1_ty], [llvm37_ptr_ty, llvm37_metadata_ty],
                                [IntrNoMem]>;

//===----------------------------------------------------------------------===//
// Target-specific intrinsics
//===----------------------------------------------------------------------===//

// HLSL Change Starts - remove platform intrinsics
// include "llvm37/IR/IntrinsicsPowerPC.td"
// include "llvm37/IR/IntrinsicsX86.td"
// include "llvm37/IR/IntrinsicsARM.td"
// include "llvm37/IR/IntrinsicsAArch64.td"
// include "llvm37/IR/IntrinsicsXCore.td"
// include "llvm37/IR/IntrinsicsHexagon.td"
// include "llvm37/IR/IntrinsicsNVVM.td"
// include "llvm37/IR/IntrinsicsMips.td"
// include "llvm37/IR/IntrinsicsAMDGPU.td"
// include "llvm37/IR/IntrinsicsBPF.td"
// include "llvm37/IR/IntrinsicsSystemZ.td"
// include "llvm37/IR/IntrinsicsWebAssembly.td"
// include "llvm37/IR/IntrinsicsHLSL.td"
// HLSL Change Ends - remove platform intrinsics
