/*===-- calc.c - tool for testing libLLVM37 and llvm37-c API ------------------===*\
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This file implements the --calc command in llvm37-c-test. --calc reads lines *|
|* from stdin, parses them as a name and an expression in reverse polish      *|
|* notation and prints a module with a function with the expression.          *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#include "llvm37-c-test.h"
#include "llvm37-c/Core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef LLVM37ValueRef (*binop_func_t)(LLVMBuilderRef, LLVM37ValueRef LHS,
                                     LLVM37ValueRef RHS, const char *Name);

static LLVM37Opcode op_to_opcode(char op) {
  switch (op) {
  case '+': return LLVM37Add;
  case '-': return LLVM37Sub;
  case '*': return LLVM37Mul;
  case '/': return LLVM37SDiv;
  case '&': return LLVM37And;
  case '|': return LLVM37Or;
  case '^': return LLVM37Xor;
  }
  assert(0 && "unknown operation");
  return 0;
}

#define MAX_DEPTH 32

static LLVM37ValueRef build_from_tokens(char **tokens, int ntokens,
                                      LLVMBuilderRef builder,
                                      LLVM37ValueRef param) {
  LLVM37ValueRef stack[MAX_DEPTH];
  int depth = 0;
  int i;

  for (i = 0; i < ntokens; i++) {
    char tok = tokens[i][0];
    switch (tok) {
    case '+':
    case '-':
    case '*':
    case '/':
    case '&':
    case '|':
    case '^':
      if (depth < 2) {
        printf("stack underflow\n");
        return NULL;
      }

      stack[depth - 2] = LLVMBuildBinOp(builder, op_to_opcode(tok),
                                        stack[depth - 1], stack[depth - 2], "");
      depth--;

      break;

    case '@': {
      LLVM37ValueRef off;

      if (depth < 1) {
        printf("stack underflow\n");
        return NULL;
      }

      off = LLVMBuildGEP(builder, param, &stack[depth - 1], 1, "");
      stack[depth - 1] = LLVMBuildLoad(builder, off, "");

      break;
    }

    default: {
      char *end;
      long val = strtol(tokens[i], &end, 0);
      if (end[0] != '\0') {
        printf("error parsing number\n");
        return NULL;
      }

      if (depth >= MAX_DEPTH) {
        printf("stack overflow\n");
        return NULL;
      }

      stack[depth++] = LLVM37ConstInt(LLVM37Int64Type(), val, 1);
      break;
    }
    }
  }

  if (depth < 1) {
    printf("stack underflow at return\n");
    return NULL;
  }

  LLVMBuildRet(builder, stack[depth - 1]);

  return stack[depth - 1];
}

static void handle_line(char **tokens, int ntokens) {
  char *name = tokens[0];
  LLVM37ValueRef param;
  LLVM37ValueRef res;

  LLVM37ModuleRef M = LLVM37ModuleCreateWithName(name);

  LLVM37TypeRef I64ty = LLVM37Int64Type();
  LLVM37TypeRef I64Ptrty = LLVM37PointerType(I64ty, 0);
  LLVM37TypeRef Fty = LLVM37FunctionType(I64ty, &I64Ptrty, 1, 0);

  LLVM37ValueRef F = LLVM37AddFunction(M, name, Fty);
  LLVMBuilderRef builder = LLVM37CreateBuilder();
  LLVM37PositionBuilderAtEnd(builder, LLVM37AppendBasicBlock(F, "entry"));

  LLVM37GetParams(F, &param);
  LLVM37SetValueName(param, "in");

  res = build_from_tokens(tokens + 1, ntokens - 1, builder, param);
  if (res) {
    char *irstr = LLVM37PrintModuleToString(M);
    puts(irstr);
    LLVM37DisposeMessage(irstr);
  }

  LLVM37DisposeBuilder(builder);

  LLVM37DisposeModule(M);
}

int calc(void) {

  tokenize_stdin(handle_line);

  return 0;
}
