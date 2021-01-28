///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilExportMap.h                                                           //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// dxilutil::ExportMap for handling -exports option.                         //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

// TODO: Refactor to separate name export verification part from
// llvm37/Function part so first part may be have shared use without llvm37

#pragma once
#include <vector>
#include <set>
#include <unordered_set>
#include <string>
#include "llvm37/ADT/StringRef.h"
#include "llvm37/ADT/StringSet.h"
#include "llvm37/ADT/SmallVector.h"
#include "llvm37/ADT/MapVector.h"

namespace llvm37 {
class Function;
class raw_ostream;
}

namespace hlsl {
namespace dxilutil {
  class ExportMap {
  public:
    typedef std::unordered_set<std::string> StringStore;
    typedef std::set<llvm37::StringRef> NameSet;
    typedef llvm37::MapVector< llvm37::Function*, NameSet > RenameMap;
    typedef llvm37::StringMap< llvm37::StringSet<> > ExportMapByString;
    typedef ExportMapByString::iterator iterator;
    typedef ExportMapByString::const_iterator const_iterator;

    ExportMap() {}
    void clear();
    bool empty() const;

    // Iterate export map by string name
    iterator begin() { return m_ExportMap.begin(); }
    const_iterator begin() const { return m_ExportMap.begin(); }
    iterator end() { return m_ExportMap.end(); }
    const_iterator end() const { return m_ExportMap.end(); }

    // Initialize export map from option strings
    bool ParseExports(const std::vector<std::string> &exportOpts, llvm37::raw_ostream &errors);
    // Add one export to the export map
    void Add(llvm37::StringRef exportName, llvm37::StringRef internalName = llvm37::StringRef());
    // Return true if export is present, or m_ExportMap is empty
    bool IsExported(llvm37::StringRef original) const;

    // Retrieve export entry by name.  If Name is mangled, it will fallback to
    // search for unmangled version if exact match fails.
    // If result == end(), no matching export was found.
    ExportMapByString::const_iterator GetExportsByName(llvm37::StringRef Name) const;

    // Call before processing functions for renaming and cloning validation
    void BeginProcessing();

    // Called for each function to be processed
    // In order to avoid intermediate name collisions during renaming,
    //  if collisionAvoidanceRenaming is true:
    //    non-exported functions will be renamed internal.<name>
    //    functions exported with a different name will be renamed temp.<name>
    // returns true if function is exported
    bool ProcessFunction(llvm37::Function *F, bool collisionAvoidanceRenaming);

    // Add function to exports without checking export map or renaming
    //  (useful for patch constant functions used by exported HS)
    void RegisterExportedFunction(llvm37::Function *F);

    // Called to mark an internal name as used (remove from unused set)
    void UseExport(llvm37::StringRef internalName);
    // Called to add an exported (full) name (for collision detection)
    void ExportName(llvm37::StringRef exportName);

    // Called after functions are processed.
    // Returns true if no name collisions or unused exports are present.
    bool EndProcessing() const;
    const NameSet& GetNameCollisions() const { return m_NameCollisions; }
    const NameSet& GetUnusedExports() const { return m_UnusedExports; }

    // GetRenames gets the map of mangled renames by function pointer
    const RenameMap &GetFunctionRenames() const { return m_RenameMap; }

  private:
    // {"internalname": ("export1", "export2", ...), ...}
    ExportMapByString m_ExportMap;
    StringStore m_StringStorage;
    llvm37::StringRef StoreString(llvm37::StringRef str);

    // Renaming/Validation state
    RenameMap m_RenameMap;
    NameSet m_ExportNames;
    NameSet m_NameCollisions;
    NameSet m_UnusedExports;
  };
}

}
