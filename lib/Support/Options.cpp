//===- llvm37/Support/Options.cpp - Debug options support ---------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the helper objects for defining debug options using the
// new API built on cl::opt, but not requiring the use of static globals.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Support/Options.h"
#include "llvm37/Support/ManagedStatic.h"

using namespace llvm37;

OptionRegistry::~OptionRegistry() {
  for (auto IT = Options.begin(); IT != Options.end(); ++IT)
    delete IT->second;
}

void OptionRegistry::addOption(void *Key, cl::Option *O) {
  assert(Options.find(Key) == Options.end() &&
         "Argument with this key already registerd");
  Options.insert(std::make_pair(Key, O));
}

static ManagedStatic<OptionRegistry> OR;

OptionRegistry &OptionRegistry::instance() { return *OR; }
