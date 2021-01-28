// RUN: llvm37-tblgen -I %p %s

def BeforeInclude;

include "Include.inc"

def AfterInclude;

