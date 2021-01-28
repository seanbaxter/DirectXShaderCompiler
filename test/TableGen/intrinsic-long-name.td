// RUN: llvm37-tblgen -gen-intrinsic %s | FileCheck %s
// XFAIL: vg_leak

class IntrinsicProperty;

class ValueType<int size, int value> {
  string Namespace = "MVT";
  int Size = size;
  int Value = value;
}

class LLVM37Type<ValueType vt> {
  ValueType VT = vt;
}

class Intrinsic<string name, list<LLVM37Type> param_types = []> {
  string LLVM37Name = name;
  bit isTarget = 0;
  string TargetPrefix = "";
  list<LLVM37Type> RetTypes = [];
  list<LLVM37Type> ParamTypes = param_types;
  list<IntrinsicProperty> Properties = [];
}

def iAny : ValueType<0, 254>;
def llvm37_anyint_ty : LLVM37Type<iAny>;

// Make sure we generate the long name without crashing
// CHECK: this_is_a_really_long_intrinsic_name_but_we_should_still_not_crash   // llvm37.this.is.a.really.long.intrinsic.name.but.we.should.still.not.crash
def int_foo : Intrinsic<"llvm.foo", [llvm37_anyint_ty]>;
def int_this_is_a_really_long_intrinsic_name_but_we_should_still_not_crash : Intrinsic<"llvm.this.is.a.really.long.intrinsic.name.but.we.should.still.not.crash", [llvm37_anyint_ty]>;

