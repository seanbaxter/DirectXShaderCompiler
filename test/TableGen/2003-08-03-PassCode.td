// RUN: llvm37-tblgen %s

class test<code C> {
  code Code = C;
}

def foo : test<[{ hello world! }]>;
