// RUN: llvm37-tblgen %s

def {
  bit A = 1;
  int B = A;
}
