//===- DiagTool.h - Classes for defining diagtool tools -------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the boilerplate for defining diagtool tools.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_TOOLS_DIAGTOOL_DIAGTOOL_H
#define LLVM37_CLANG_TOOLS_DIAGTOOL_DIAGTOOL_H

#include "llvm37/ADT/StringRef.h"
#include "llvm37/Support/ManagedStatic.h"
#include "llvm37/Support/raw_ostream.h"
#include <string>


namespace diagtool {

class DiagTool {
  const std::string cmd;
  const std::string description;
public:
  DiagTool(llvm37::StringRef toolCmd, llvm37::StringRef toolDesc);
  virtual ~DiagTool();
  
  llvm37::StringRef getName() const { return cmd; }  
  llvm37::StringRef getDescription() const { return description; }  

  virtual int run(unsigned argc, char *argv[], llvm37::raw_ostream &out) = 0;
};
  
class DiagTools {
  void *tools;
public:
  DiagTools();
  ~DiagTools();
  
  DiagTool *getTool(llvm37::StringRef toolCmd);
  void registerTool(DiagTool *tool);  
  void printCommands(llvm37::raw_ostream &out);  
};

extern llvm37::ManagedStatic<DiagTools> diagTools;

template <typename DIAGTOOL>
class RegisterDiagTool {
public:
  RegisterDiagTool() { diagTools->registerTool(new DIAGTOOL()); }
};

} // end diagtool namespace

#define DEF_DIAGTOOL(NAME, DESC, CLSNAME)\
namespace {\
class CLSNAME : public diagtool::DiagTool {\
public:\
  CLSNAME() : DiagTool(NAME, DESC) {}\
  virtual ~CLSNAME() {}\
  int run(unsigned argc, char *argv[], llvm37::raw_ostream &out) override;\
};\
diagtool::RegisterDiagTool<CLSNAME> Register##CLSNAME;\
}

#endif
