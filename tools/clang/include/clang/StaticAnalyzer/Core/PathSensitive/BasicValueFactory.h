//=== BasicValueFactory.h - Basic values for Path Sens analysis --*- C++ -*---//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines BasicValueFactory, a class that manages the lifetime
//  of APSInt objects and symbolic constraints used by ExprEngine
//  and related classes.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_BASICVALUEFACTORY_H
#define LLVM37_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_BASICVALUEFACTORY_H

#include "clang/AST/ASTContext.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/APSIntType.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SVals.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/StoreRef.h"

namespace clang {
namespace ento {

class CompoundValData : public llvm37::FoldingSetNode {
  QualType T;
  llvm37::ImmutableList<SVal> L;

public:
  CompoundValData(QualType t, llvm37::ImmutableList<SVal> l)
    : T(t), L(l) {}

  typedef llvm37::ImmutableList<SVal>::iterator iterator;
  iterator begin() const { return L.begin(); }
  iterator end() const { return L.end(); }

  static void Profile(llvm37::FoldingSetNodeID& ID, QualType T,
                      llvm37::ImmutableList<SVal> L);

  void Profile(llvm37::FoldingSetNodeID& ID) { Profile(ID, T, L); }
};

class LazyCompoundValData : public llvm37::FoldingSetNode {
  StoreRef store;
  const TypedValueRegion *region;
public:
  LazyCompoundValData(const StoreRef &st, const TypedValueRegion *r)
    : store(st), region(r) {}

  const void *getStore() const { return store.getStore(); }
  const TypedValueRegion *getRegion() const { return region; }

  static void Profile(llvm37::FoldingSetNodeID& ID,
                      const StoreRef &store,
                      const TypedValueRegion *region);

  void Profile(llvm37::FoldingSetNodeID& ID) { Profile(ID, store, region); }
};

class BasicValueFactory {
  typedef llvm37::FoldingSet<llvm37::FoldingSetNodeWrapper<llvm37::APSInt> >
          APSIntSetTy;

  ASTContext &Ctx;
  llvm37::BumpPtrAllocator& BPAlloc;

  APSIntSetTy   APSIntSet;
  void *        PersistentSVals;
  void *        PersistentSValPairs;

  llvm37::ImmutableList<SVal>::Factory SValListFactory;
  llvm37::FoldingSet<CompoundValData>  CompoundValDataSet;
  llvm37::FoldingSet<LazyCompoundValData> LazyCompoundValDataSet;

  // This is private because external clients should use the factory
  // method that takes a QualType.
  const llvm37::APSInt& getValue(uint64_t X, unsigned BitWidth, bool isUnsigned);

public:
  BasicValueFactory(ASTContext &ctx, llvm37::BumpPtrAllocator &Alloc)
    : Ctx(ctx), BPAlloc(Alloc), PersistentSVals(nullptr),
      PersistentSValPairs(nullptr), SValListFactory(Alloc) {}

  ~BasicValueFactory();

  ASTContext &getContext() const { return Ctx; }

  const llvm37::APSInt& getValue(const llvm37::APSInt& X);
  const llvm37::APSInt& getValue(const llvm37::APInt& X, bool isUnsigned);
  const llvm37::APSInt& getValue(uint64_t X, QualType T);

  /// Returns the type of the APSInt used to store values of the given QualType.
  APSIntType getAPSIntType(QualType T) const {
    assert(T->isIntegralOrEnumerationType() || Loc::isLocType(T));
    return APSIntType(Ctx.getTypeSize(T),
                      !T->isSignedIntegerOrEnumerationType());
  }

  /// Convert - Create a new persistent APSInt with the same value as 'From'
  ///  but with the bitwidth and signedness of 'To'.
  const llvm37::APSInt &Convert(const llvm37::APSInt& To,
                              const llvm37::APSInt& From) {
    APSIntType TargetType(To);
    if (TargetType == APSIntType(From))
      return From;

    return getValue(TargetType.convert(From));
  }
  
  const llvm37::APSInt &Convert(QualType T, const llvm37::APSInt &From) {
    APSIntType TargetType = getAPSIntType(T);
    if (TargetType == APSIntType(From))
      return From;
    
    return getValue(TargetType.convert(From));
  }

  const llvm37::APSInt& getIntValue(uint64_t X, bool isUnsigned) {
    QualType T = isUnsigned ? Ctx.UnsignedIntTy : Ctx.IntTy;
    return getValue(X, T);
  }

  inline const llvm37::APSInt& getMaxValue(const llvm37::APSInt &v) {
    return getValue(APSIntType(v).getMaxValue());
  }

  inline const llvm37::APSInt& getMinValue(const llvm37::APSInt &v) {
    return getValue(APSIntType(v).getMinValue());
  }

  inline const llvm37::APSInt& getMaxValue(QualType T) {
    return getValue(getAPSIntType(T).getMaxValue());
  }

  inline const llvm37::APSInt& getMinValue(QualType T) {
    return getValue(getAPSIntType(T).getMinValue());
  }

  inline const llvm37::APSInt& Add1(const llvm37::APSInt& V) {
    llvm37::APSInt X = V;
    ++X;
    return getValue(X);
  }

  inline const llvm37::APSInt& Sub1(const llvm37::APSInt& V) {
    llvm37::APSInt X = V;
    --X;
    return getValue(X);
  }

  inline const llvm37::APSInt& getZeroWithPtrWidth(bool isUnsigned = true) {
    return getValue(0, Ctx.getTypeSize(Ctx.VoidPtrTy), isUnsigned);
  }

  inline const llvm37::APSInt &getIntWithPtrWidth(uint64_t X, bool isUnsigned) {
    return getValue(X, Ctx.getTypeSize(Ctx.VoidPtrTy), isUnsigned);
  }

  inline const llvm37::APSInt& getTruthValue(bool b, QualType T) {
    return getValue(b ? 1 : 0, Ctx.getTypeSize(T), false);
  }

  inline const llvm37::APSInt& getTruthValue(bool b) {
    return getTruthValue(b, Ctx.getLogicalOperationType());
  }

  const CompoundValData *getCompoundValData(QualType T,
                                            llvm37::ImmutableList<SVal> Vals);

  const LazyCompoundValData *getLazyCompoundValData(const StoreRef &store,
                                            const TypedValueRegion *region);

  llvm37::ImmutableList<SVal> getEmptySValList() {
    return SValListFactory.getEmptyList();
  }

  llvm37::ImmutableList<SVal> consVals(SVal X, llvm37::ImmutableList<SVal> L) {
    return SValListFactory.add(X, L);
  }

  const llvm37::APSInt* evalAPSInt(BinaryOperator::Opcode Op,
                                     const llvm37::APSInt& V1,
                                     const llvm37::APSInt& V2);

  const std::pair<SVal, uintptr_t>&
  getPersistentSValWithData(const SVal& V, uintptr_t Data);

  const std::pair<SVal, SVal>&
  getPersistentSValPair(const SVal& V1, const SVal& V2);

  const SVal* getPersistentSVal(SVal X);
};

} // end GR namespace

} // end clang namespace

#endif
