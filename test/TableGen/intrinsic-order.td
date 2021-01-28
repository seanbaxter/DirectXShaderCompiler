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


// Make sure an intrinsic name that is a prefix of another is checked after the
// other.

// CHECK: if (NameR.startswith("oo.bar.")) return Intrinsic::foo_bar;
// CHECK: if (NameR.startswith("oo.")) return Intrinsic::foo;

def int_foo : Intrinsic<"llvm.foo", [llvm37_anyint_ty]>;
def int_foo_bar : Intrinsic<"llvm.foo.bar", [llvm37_anyint_ty]>;
