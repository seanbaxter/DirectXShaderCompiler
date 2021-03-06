//=== RegistryParser.h - Linker-supported plugin registries -----*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Defines a command-line parser for a registry.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_SUPPORT_REGISTRYPARSER_H
#define LLVM37_SUPPORT_REGISTRYPARSER_H

#include "llvm37/Support/CommandLine.h"
#include "llvm37/Support/Registry.h"

namespace llvm37 {

  /// A command-line parser for a registry. Use like such:
  ///
  ///   static cl::opt<Registry<Collector>::entry, false,
  ///                  RegistryParser<Collector> >
  ///   GCOpt("gc", cl::desc("Garbage collector to use."),
  ///               cl::value_desc());
  ///
  /// To make use of the value:
  ///
  ///   Collector *TheCollector = GCOpt->instantiate();
  ///
  template <typename T, typename U = RegistryTraits<T> >
  class RegistryParser :
  public cl::parser<const typename U::entry*>,
    public Registry<T, U>::listener {
    typedef U traits;
    typedef typename U::entry entry;
    typedef typename Registry<T, U>::listener listener;

  protected:
    void registered(const entry &E) {
      addLiteralOption(traits::nameof(E), &E, traits::descof(E));
    }

  public:
    void initialize(cl::Option &O) {
      listener::init();
      cl::parser<const typename U::entry*>::initialize(O);
    }
  };

}

#endif // LLVM37_SUPPORT_REGISTRYPARSER_H
