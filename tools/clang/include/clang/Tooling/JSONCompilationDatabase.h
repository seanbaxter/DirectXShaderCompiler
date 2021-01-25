//===--- JSONCompilationDatabase.h - ----------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  The JSONCompilationDatabase finds compilation databases supplied as a file
//  'compile_commands.json'.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_TOOLING_JSONCOMPILATIONDATABASE_H
#define LLVM37_CLANG_TOOLING_JSONCOMPILATIONDATABASE_H

#include "clang/Basic/LLVM.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/FileMatchTrie.h"
#include "llvm37/ADT/StringMap.h"
#include "llvm37/ADT/StringRef.h"
#include "llvm37/Support/MemoryBuffer.h"
#include "llvm37/Support/SourceMgr.h"
#include "llvm37/Support/YAMLParser.h"
#include <memory>
#include <string>
#include <vector>

namespace clang {
namespace tooling {

/// \brief A JSON based compilation database.
///
/// JSON compilation database files must contain a list of JSON objects which
/// provide the command lines in the attributes 'directory', 'command' and
/// 'file':
/// [
///   { "directory": "<working directory of the compile>",
///     "command": "<compile command line>",
///     "file": "<path to source file>"
///   },
///   ...
/// ]
/// Each object entry defines one compile action. The specified file is
/// considered to be the main source file for the translation unit.
///
/// JSON compilation databases can for example be generated in CMake projects
/// by setting the flag -DCMAKE_EXPORT_COMPILE_COMMANDS.
class JSONCompilationDatabase : public CompilationDatabase {
public:
  /// \brief Loads a JSON compilation database from the specified file.
  ///
  /// Returns NULL and sets ErrorMessage if the database could not be
  /// loaded from the given file.
  static std::unique_ptr<JSONCompilationDatabase>
  loadFromFile(StringRef FilePath, std::string &ErrorMessage);

  /// \brief Loads a JSON compilation database from a data buffer.
  ///
  /// Returns NULL and sets ErrorMessage if the database could not be loaded.
  static std::unique_ptr<JSONCompilationDatabase>
  loadFromBuffer(StringRef DatabaseString, std::string &ErrorMessage);

  /// \brief Returns all compile comamnds in which the specified file was
  /// compiled.
  ///
  /// FIXME: Currently FilePath must be an absolute path inside the
  /// source directory which does not have symlinks resolved.
  std::vector<CompileCommand>
  getCompileCommands(StringRef FilePath) const override;

  /// \brief Returns the list of all files available in the compilation database.
  ///
  /// These are the 'file' entries of the JSON objects.
  std::vector<std::string> getAllFiles() const override;

  /// \brief Returns all compile commands for all the files in the compilation
  /// database.
  std::vector<CompileCommand> getAllCompileCommands() const override;

private:
  /// \brief Constructs a JSON compilation database on a memory buffer.
  JSONCompilationDatabase(std::unique_ptr<llvm37::MemoryBuffer> Database)
      : Database(std::move(Database)),
        YAMLStream(this->Database->getBuffer(), SM) {}

  /// \brief Parses the database file and creates the index.
  ///
  /// Returns whether parsing succeeded. Sets ErrorMessage if parsing
  /// failed.
  bool parse(std::string &ErrorMessage);

  // Tuple (directory, commandline) where 'commandline' pointing to the
  // corresponding nodes in the YAML stream.
  typedef std::pair<llvm37::yaml::ScalarNode*,
                    llvm37::yaml::ScalarNode*> CompileCommandRef;

  /// \brief Converts the given array of CompileCommandRefs to CompileCommands.
  void getCommands(ArrayRef<CompileCommandRef> CommandsRef,
                   std::vector<CompileCommand> &Commands) const;

  // Maps file paths to the compile command lines for that file.
  llvm37::StringMap< std::vector<CompileCommandRef> > IndexByFile;

  FileMatchTrie MatchTrie;

  std::unique_ptr<llvm37::MemoryBuffer> Database;
  llvm37::SourceMgr SM;
  llvm37::yaml::Stream YAMLStream;
};

} // end namespace tooling
} // end namespace clang

#endif
