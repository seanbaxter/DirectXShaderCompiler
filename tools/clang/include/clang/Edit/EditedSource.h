//===----- EditedSource.h - Collection of source edits ----------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_EDIT_EDITEDSOURCE_H
#define LLVM37_CLANG_EDIT_EDITEDSOURCE_H

#include "clang/Edit/FileOffset.h"
#include "llvm37/ADT/DenseMap.h"
#include "llvm37/ADT/StringRef.h"
#include "llvm37/Support/Allocator.h"
#include <map>

namespace clang {
  class LangOptions;
  class PPConditionalDirectiveRecord;

namespace edit {
  class Commit;
  class EditsReceiver;

class EditedSource {
  const SourceManager &SourceMgr;
  const LangOptions &LangOpts;
  const PPConditionalDirectiveRecord *PPRec;

  struct FileEdit {
    StringRef Text;
    unsigned RemoveLen;

    FileEdit() : RemoveLen(0) {}
  };

  typedef std::map<FileOffset, FileEdit> FileEditsTy;
  FileEditsTy FileEdits;

  llvm37::DenseMap<unsigned, SourceLocation> ExpansionToArgMap;

  llvm37::BumpPtrAllocator StrAlloc;

public:
  EditedSource(const SourceManager &SM, const LangOptions &LangOpts,
               const PPConditionalDirectiveRecord *PPRec = nullptr)
    : SourceMgr(SM), LangOpts(LangOpts), PPRec(PPRec),
      StrAlloc() { }

  const SourceManager &getSourceManager() const { return SourceMgr; }
  const LangOptions &getLangOpts() const { return LangOpts; }
  const PPConditionalDirectiveRecord *getPPCondDirectiveRecord() const {
    return PPRec;
  }

  bool canInsertInOffset(SourceLocation OrigLoc, FileOffset Offs);

  bool commit(const Commit &commit);
  
  void applyRewrites(EditsReceiver &receiver);
  void clearRewrites();

  StringRef copyString(StringRef str) {
    char *buf = StrAlloc.Allocate<char>(str.size());
    std::memcpy(buf, str.data(), str.size());
    return StringRef(buf, str.size());
  }
  StringRef copyString(const Twine &twine);

private:
  bool commitInsert(SourceLocation OrigLoc, FileOffset Offs, StringRef text,
                    bool beforePreviousInsertions);
  bool commitInsertFromRange(SourceLocation OrigLoc, FileOffset Offs,
                             FileOffset InsertFromRangeOffs, unsigned Len,
                             bool beforePreviousInsertions);
  void commitRemove(SourceLocation OrigLoc, FileOffset BeginOffs, unsigned Len);

  StringRef getSourceText(FileOffset BeginOffs, FileOffset EndOffs,
                          bool &Invalid);
  FileEditsTy::iterator getActionForOffset(FileOffset Offs);
};

}

} // end namespace clang

#endif
