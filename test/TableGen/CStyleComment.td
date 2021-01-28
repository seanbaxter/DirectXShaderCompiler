// Test that multiline, nested, comments work correctly.
//
// RUN: llvm37-tblgen < %s

/* Foo
  bar
  /* 
  blah
  */

  stuff
  */

def x;
