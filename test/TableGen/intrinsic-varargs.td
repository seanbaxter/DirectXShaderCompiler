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

// isVoid needs to match the definition in ValueTypes.td
def isVoid : ValueType<0, 57>;   // Produces no value
def llvm37_vararg_ty : LLVM37Type<isVoid>;   // this means vararg here

// CHECK: /* 0 */ 0, 28, 0,
def int_foo : Intrinsic<"llvm.foo", [llvm37_vararg_ty]>;
