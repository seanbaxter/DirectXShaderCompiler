// %RUN: %clang_cc1 -std=c++11 -emit-llvm37 %s -o %t

enum E : bool { A };
template <E>
struct S {
  struct Inner {
    Inner() {}
  };
};

template class S<A>;
