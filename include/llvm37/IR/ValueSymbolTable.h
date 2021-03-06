//===-- llvm37/ValueSymbolTable.h - Implement a Value Symtab ------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the name/Value symbol table for LLVM37.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_IR_VALUESYMBOLTABLE_H
#define LLVM37_IR_VALUESYMBOLTABLE_H

#include "llvm37/ADT/StringMap.h"
#include "llvm37/IR/Value.h"
#include "llvm37/Support/DataTypes.h"

namespace llvm37 {
  template<typename ValueSubClass, typename ItemParentClass>
        class SymbolTableListTraits;
  class BasicBlock;
  class Function;
  class NamedMDNode;
  class Module;
  class StringRef;

/// This class provides a symbol table of name/value pairs. It is essentially
/// a std::map<std::string,Value*> but has a controlled interface provided by
/// LLVM37 as well as ensuring uniqueness of names.
///
class ValueSymbolTable {
  friend class Value;
  friend class SymbolTableListTraits<Argument, Function>;
  friend class SymbolTableListTraits<BasicBlock, Function>;
  friend class SymbolTableListTraits<Instruction, BasicBlock>;
  friend class SymbolTableListTraits<Function, Module>;
  friend class SymbolTableListTraits<GlobalVariable, Module>;
  friend class SymbolTableListTraits<GlobalAlias, Module>;
/// @name Types
/// @{
public:
  /// @brief A mapping of names to values.
  typedef StringMap<Value*> ValueMap;

  /// @brief An iterator over a ValueMap.
  typedef ValueMap::iterator iterator;

  /// @brief A const_iterator over a ValueMap.
  typedef ValueMap::const_iterator const_iterator;

/// @}
/// @name Constructors
/// @{
public:

  ValueSymbolTable() : vmap(0), LastUnique(0) {}
  ~ValueSymbolTable();

/// @}
/// @name Accessors
/// @{
public:

  /// This method finds the value with the given \p Name in the
  /// the symbol table. 
  /// @returns the value associated with the \p Name
  /// @brief Lookup a named Value.
  Value *lookup(StringRef Name) const { return vmap.lookup(Name); }

  /// @returns true iff the symbol table is empty
  /// @brief Determine if the symbol table is empty
  inline bool empty() const { return vmap.empty(); }

  /// @brief The number of name/type pairs is returned.
  inline unsigned size() const { return unsigned(vmap.size()); }

  /// This function can be used from the debugger to display the
  /// content of the symbol table while debugging.
  /// @brief Print out symbol table on stderr
  void dump() const;

/// @}
/// @name Iteration
/// @{
public:
  /// @brief Get an iterator that from the beginning of the symbol table.
  inline iterator begin() { return vmap.begin(); }

  /// @brief Get a const_iterator that from the beginning of the symbol table.
  inline const_iterator begin() const { return vmap.begin(); }

  /// @brief Get an iterator to the end of the symbol table.
  inline iterator end() { return vmap.end(); }

  /// @brief Get a const_iterator to the end of the symbol table.
  inline const_iterator end() const { return vmap.end(); }
  
/// @}
/// @name Mutators
/// @{
private:
  /// This method adds the provided value \p N to the symbol table.  The Value
  /// must have a name which is used to place the value in the symbol table. 
  /// If the inserted name conflicts, this renames the value.
  /// @brief Add a named value to the symbol table
  void reinsertValue(Value *V);
    
  /// createValueName - This method attempts to create a value name and insert
  /// it into the symbol table with the specified name.  If it conflicts, it
  /// auto-renames the name and returns that instead.
  ValueName *createValueName(StringRef Name, Value *V);
  
  /// This method removes a value from the symbol table.  It leaves the
  /// ValueName attached to the value, but it is no longer inserted in the
  /// symtab.
  void removeValueName(ValueName *V);
  
/// @}
/// @name Internal Data
/// @{
private:
  ValueMap vmap;                    ///< The map that holds the symbol table.
  mutable uint32_t LastUnique; ///< Counter for tracking unique names

/// @}
};

} // End llvm37 namespace

#endif
