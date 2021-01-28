/*===-- include-all.c - tool for testing libLLVM37 and llvm37-c API -----------===*\
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This file doesn't have any actual code. It just make sure that all         *|
|* the llvm37-c include files are good and doesn't generate any warnings        *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

// FIXME: Autogenerate this list

#include "llvm37-c/Analysis.h"
#include "llvm37-c/BitReader.h"
#include "llvm37-c/BitWriter.h"
#include "llvm37-c/Core.h"
#include "llvm37-c/Disassembler.h"
#include "llvm37-c/ExecutionEngine.h"
#include "llvm37-c/Initialization.h"
#include "llvm37-c/LinkTimeOptimizer.h"
#include "llvm37-c/Linker.h"
#include "llvm37-c/Object.h"
#include "llvm37-c/Target.h"
#include "llvm37-c/TargetMachine.h"
#include "llvm37-c/Transforms/IPO.h"
#include "llvm37-c/Transforms/PassManagerBuilder.h"
#include "llvm37-c/Transforms/Scalar.h"
#include "llvm37-c/Transforms/Vectorize.h"
#include "llvm37-c/lto.h"
