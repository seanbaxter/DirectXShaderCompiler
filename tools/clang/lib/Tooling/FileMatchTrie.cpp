//===--- FileMatchTrie.cpp - ----------------------------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file contains the implementation of a FileMatchTrie.
//
//===----------------------------------------------------------------------===//

#include "clang/Tooling/FileMatchTrie.h"
#include "llvm37/ADT/StringMap.h"
#include "llvm37/Support/FileSystem.h"
#include "llvm37/Support/Path.h"
#include "llvm37/Support/raw_ostream.h"
#include <sstream>
using namespace clang;
using namespace tooling;

namespace {
/// \brief Default \c PathComparator using \c llvm37::sys::fs::equivalent().
struct DefaultPathComparator : public PathComparator {
  bool equivalent(StringRef FileA, StringRef FileB) const override {
    return FileA == FileB || llvm37::sys::fs::equivalent(FileA, FileB);
  }
};
}

namespace clang {
namespace tooling {
/// \brief A node of the \c FileMatchTrie.
///
/// Each node has storage for up to one path and a map mapping a path segment to
/// child nodes. The trie starts with an empty root node.
class FileMatchTrieNode {
public:
  /// \brief Inserts 'NewPath' into this trie. \c ConsumedLength denotes
  /// the number of \c NewPath's trailing characters already consumed during
  /// recursion.
  ///
  /// An insert of a path
  /// 'p'starts at the root node and does the following:
  /// - If the node is empty, insert 'p' into its storage and abort.
  /// - If the node has a path 'p2' but no children, take the last path segment
  ///   's' of 'p2', put a new child into the map at 's' an insert the rest of
  ///   'p2' there.
  /// - Insert a new child for the last segment of 'p' and insert the rest of
  ///   'p' there.
  ///
  /// An insert operation is linear in the number of a path's segments.
  void insert(StringRef NewPath, unsigned ConsumedLength = 0) {
    // We cannot put relative paths into the FileMatchTrie as then a path can be
    // a postfix of another path, violating a core assumption of the trie.
    if (llvm37::sys::path::is_relative(NewPath))
      return;
    if (Path.empty()) {
      // This is an empty leaf. Store NewPath and return.
      Path = NewPath;
      return;
    }
    if (Children.empty()) {
      // This is a leaf, ignore duplicate entry if 'Path' equals 'NewPath'.
      if (NewPath == Path)
          return;
      // Make this a node and create a child-leaf with 'Path'.
      StringRef Element(llvm37::sys::path::filename(
          StringRef(Path).drop_back(ConsumedLength)));
      Children[Element].Path = Path;
    }
    StringRef Element(llvm37::sys::path::filename(
          StringRef(NewPath).drop_back(ConsumedLength)));
    Children[Element].insert(NewPath, ConsumedLength + Element.size() + 1);
  }

  /// \brief Tries to find the node under this \c FileMatchTrieNode that best
  /// matches 'FileName'.
  ///
  /// If multiple paths fit 'FileName' equally well, \c IsAmbiguous is set to
  /// \c true and an empty string is returned. If no path fits 'FileName', an
  /// empty string is returned. \c ConsumedLength denotes the number of
  /// \c Filename's trailing characters already consumed during recursion.
  ///
  /// To find the best matching node for a given path 'p', the
  /// \c findEquivalent() function is called recursively for each path segment
  /// (back to fron) of 'p' until a node 'n' is reached that does not ..
  /// - .. have children. In this case it is checked
  ///   whether the stored path is equivalent to 'p'. If yes, the best match is
  ///   found. Otherwise continue with the parent node as if this node did not
  ///   exist.
  /// - .. a child matching the next path segment. In this case, all children of
  ///   'n' are an equally good match for 'p'. All children are of 'n' are found
  ///   recursively and their equivalence to 'p' is determined. If none are
  ///   equivalent, continue with the parent node as if 'n' didn't exist. If one
  ///   is equivalent, the best match is found. Otherwise, report and ambigiuity
  ///   error.
  StringRef findEquivalent(const PathComparator& Comparator,
                           StringRef FileName,
                           bool &IsAmbiguous,
                           unsigned ConsumedLength = 0) const {
    if (Children.empty()) {
      if (Comparator.equivalent(StringRef(Path), FileName))
        return StringRef(Path);
      return StringRef();
    }
    StringRef Element(llvm37::sys::path::filename(FileName.drop_back(
        ConsumedLength)));
    llvm37::StringMap<FileMatchTrieNode>::const_iterator MatchingChild =
        Children.find(Element);
    if (MatchingChild != Children.end()) {
      StringRef Result = MatchingChild->getValue().findEquivalent(
          Comparator, FileName, IsAmbiguous,
          ConsumedLength + Element.size() + 1);
      if (!Result.empty() || IsAmbiguous)
        return Result;
    }
    std::vector<StringRef> AllChildren;
    getAll(AllChildren, MatchingChild);
    StringRef Result;
    for (unsigned i = 0; i < AllChildren.size(); i++) {
      if (Comparator.equivalent(AllChildren[i], FileName)) {
        if (Result.empty()) {
          Result = AllChildren[i];
        } else {
          IsAmbiguous = true;
          return StringRef();
        }
      }
    }
    return Result;
  }

private:
  /// \brief Gets all paths under this FileMatchTrieNode.
  void getAll(std::vector<StringRef> &Results,
              llvm37::StringMap<FileMatchTrieNode>::const_iterator Except) const {
    if (Path.empty())
      return;
    if (Children.empty()) {
      Results.push_back(StringRef(Path));
      return;
    }
    for (llvm37::StringMap<FileMatchTrieNode>::const_iterator
         It = Children.begin(), E = Children.end();
         It != E; ++It) {
      if (It == Except)
        continue;
      It->getValue().getAll(Results, Children.end());
    }
  }

  // The stored absolute path in this node. Only valid for leaf nodes, i.e.
  // nodes where Children.empty().
  std::string Path;

  // The children of this node stored in a map based on the next path segment.
  llvm37::StringMap<FileMatchTrieNode> Children;
};
} // end namespace tooling
} // end namespace clang

FileMatchTrie::FileMatchTrie()
  : Root(new FileMatchTrieNode), Comparator(new DefaultPathComparator()) {}

FileMatchTrie::FileMatchTrie(PathComparator *Comparator)
  : Root(new FileMatchTrieNode), Comparator(Comparator) {}

FileMatchTrie::~FileMatchTrie() {
  delete Root;
}

void FileMatchTrie::insert(StringRef NewPath) {
  Root->insert(NewPath);
}

StringRef FileMatchTrie::findEquivalent(StringRef FileName,
                                        raw_ostream &Error) const {
  if (llvm37::sys::path::is_relative(FileName)) {
    Error << "Cannot resolve relative paths";
    return StringRef();
  }
  bool IsAmbiguous = false;
  StringRef Result = Root->findEquivalent(*Comparator, FileName, IsAmbiguous);
  if (IsAmbiguous)
    Error << "Path is ambiguous";
  return Result;
}
