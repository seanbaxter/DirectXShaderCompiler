//===---------- RPCChannel.h - LLVM37 out-of-process JIT execution ----------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Definition of the RemoteTargetExternal class which executes JITed code in a
// separate process from where it was built.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_TOOLS_LLI_RPCCHANNEL_H
#define LLVM37_TOOLS_LLI_RPCCHANNEL_H

#include <stdlib.h>
#include <string>

namespace llvm37 {

class RPCChannel {
public:
  std::string ChildName;

  RPCChannel() {}
  ~RPCChannel();

  /// Start the remote process.
  ///
  /// @returns True on success. On failure, ErrorMsg is updated with
  ///          descriptive text of the encountered error.
  bool createServer();

  bool createClient();

  // This will get filled in as a point to an OS-specific structure.
  void *ConnectionData;

  bool WriteBytes(const void *Data, size_t Size);
  bool ReadBytes(void *Data, size_t Size);

  void Wait();
};

} // end namespace llvm37

#endif
