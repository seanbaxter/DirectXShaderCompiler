//===-- llvm37/Support/PluginLoader.h - Plugin Loader for Tools ---*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// A tool can #include this file to get a -load option that allows the user to
// load arbitrary shared objects into the tool's address space.  Note that this
// header can only be included by a program ONCE, so it should never to used by
// library authors.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_SUPPORT_PLUGINLOADER_H
#define LLVM37_SUPPORT_PLUGINLOADER_H

#include "llvm37/Support/CommandLine.h"

#if 0 // HLSL Change Starts - no support for plug-in loader

namespace llvm37 {
  struct PluginLoader {
    void operator=(const std::string &Filename);
    static unsigned getNumPlugins();
    static std::string& getPlugin(unsigned num);
  };

#ifndef DONT_GET_PLUGIN_LOADER_OPTION
  // This causes operator= above to be invoked for every -load option.
  static cl::opt<PluginLoader, false, cl::parser<std::string> >
    LoadOpt("load", cl::ZeroOrMore, cl::value_desc("pluginfilename"),
            cl::desc("Load the specified plugin"));
#endif
}

#endif // HLSL Change Ends - no support for plug-in loader

#endif
