// RUN: llvm37-tblgen %s 

class x {
  string y = "missing terminating '\"' character";
}

