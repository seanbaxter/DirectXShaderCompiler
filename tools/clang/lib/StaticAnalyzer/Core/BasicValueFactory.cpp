//=== BasicValueFactory.cpp - Basic values for Path Sens analysis --*- C++ -*-//
//
//                     The LLVM Compiler Infrastructure
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

#include "clang/AST/ASTContext.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/BasicValueFactory.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/Store.h"

using namespace clang;
using namespace ento;

void CompoundValData::Profile(llvm37::FoldingSetNodeID& ID, QualType T,
                              llvm37::ImmutableList<SVal> L) {
  T.Profile(ID);
  ID.AddPointer(L.getInternalPointer());
}

void LazyCompoundValData::Profile(llvm37::FoldingSetNodeID& ID,
                                  const StoreRef &store,
                                  const TypedValueRegion *region) {
  ID.AddPointer(store.getStore());
  ID.AddPointer(region);
}

typedef std::pair<SVal, uintptr_t> SValData;
typedef std::pair<SVal, SVal> SValPair;

namespace llvm37 {
template<> struct FoldingSetTrait<SValData> {
  static inline void Profile(const SValData& X, llvm37::FoldingSetNodeID& ID) {
    X.first.Profile(ID);
    ID.AddPointer( (void*) X.second);
  }
};

template<> struct FoldingSetTrait<SValPair> {
  static inline void Profile(const SValPair& X, llvm37::FoldingSetNodeID& ID) {
    X.first.Profile(ID);
    X.second.Profile(ID);
  }
};
}

typedef llvm37::FoldingSet<llvm37::FoldingSetNodeWrapper<SValData> >
  PersistentSValsTy;

typedef llvm37::FoldingSet<llvm37::FoldingSetNodeWrapper<SValPair> >
  PersistentSValPairsTy;

BasicValueFactory::~BasicValueFactory() {
  // Note that the dstor for the contents of APSIntSet will never be called,
  // so we iterate over the set and invoke the dstor for each APSInt.  This
  // frees an aux. memory allocated to represent very large constants.
  for (APSIntSetTy::iterator I=APSIntSet.begin(), E=APSIntSet.end(); I!=E; ++I)
    I->getValue().~APSInt();

  delete (PersistentSValsTy*) PersistentSVals;
  delete (PersistentSValPairsTy*) PersistentSValPairs;
}

const llvm37::APSInt& BasicValueFactory::getValue(const llvm37::APSInt& X) {
  llvm37::FoldingSetNodeID ID;
  void *InsertPos;
  typedef llvm37::FoldingSetNodeWrapper<llvm37::APSInt> FoldNodeTy;

  X.Profile(ID);
  FoldNodeTy* P = APSIntSet.FindNodeOrInsertPos(ID, InsertPos);

  if (!P) {
    P = (FoldNodeTy*) BPAlloc.Allocate<FoldNodeTy>();
    new (P) FoldNodeTy(X);
    APSIntSet.InsertNode(P, InsertPos);
  }

  return *P;
}

const llvm37::APSInt& BasicValueFactory::getValue(const llvm37::APInt& X,
                                                bool isUnsigned) {
  llvm37::APSInt V(X, isUnsigned);
  return getValue(V);
}

const llvm37::APSInt& BasicValueFactory::getValue(uint64_t X, unsigned BitWidth,
                                           bool isUnsigned) {
  llvm37::APSInt V(BitWidth, isUnsigned);
  V = X;
  return getValue(V);
}

const llvm37::APSInt& BasicValueFactory::getValue(uint64_t X, QualType T) {

  return getValue(getAPSIntType(T).getValue(X));
}

const CompoundValData*
BasicValueFactory::getCompoundValData(QualType T,
                                      llvm37::ImmutableList<SVal> Vals) {

  llvm37::FoldingSetNodeID ID;
  CompoundValData::Profile(ID, T, Vals);
  void *InsertPos;

  CompoundValData* D = CompoundValDataSet.FindNodeOrInsertPos(ID, InsertPos);

  if (!D) {
    D = (CompoundValData*) BPAlloc.Allocate<CompoundValData>();
    new (D) CompoundValData(T, Vals);
    CompoundValDataSet.InsertNode(D, InsertPos);
  }

  return D;
}

const LazyCompoundValData*
BasicValueFactory::getLazyCompoundValData(const StoreRef &store,
                                          const TypedValueRegion *region) {
  llvm37::FoldingSetNodeID ID;
  LazyCompoundValData::Profile(ID, store, region);
  void *InsertPos;

  LazyCompoundValData *D =
    LazyCompoundValDataSet.FindNodeOrInsertPos(ID, InsertPos);

  if (!D) {
    D = (LazyCompoundValData*) BPAlloc.Allocate<LazyCompoundValData>();
    new (D) LazyCompoundValData(store, region);
    LazyCompoundValDataSet.InsertNode(D, InsertPos);
  }

  return D;
}

const llvm37::APSInt*
BasicValueFactory::evalAPSInt(BinaryOperator::Opcode Op,
                             const llvm37::APSInt& V1, const llvm37::APSInt& V2) {

  switch (Op) {
    default:
      assert (false && "Invalid Opcode.");

    case BO_Mul:
      return &getValue( V1 * V2 );

    case BO_Div:
      if (V2 == 0) // Avoid division by zero
        return nullptr;
      return &getValue( V1 / V2 );

    case BO_Rem:
      if (V2 == 0) // Avoid division by zero
        return nullptr;
      return &getValue( V1 % V2 );

    case BO_Add:
      return &getValue( V1 + V2 );

    case BO_Sub:
      return &getValue( V1 - V2 );

    case BO_Shl: {

      // FIXME: This logic should probably go higher up, where we can
      // test these conditions symbolically.

      // FIXME: Expand these checks to include all undefined behavior.

      if (V2.isSigned() && V2.isNegative())
        return nullptr;

      uint64_t Amt = V2.getZExtValue();

      if (Amt >= V1.getBitWidth())
        return nullptr;

      return &getValue( V1.operator<<( (unsigned) Amt ));
    }

    case BO_Shr: {

      // FIXME: This logic should probably go higher up, where we can
      // test these conditions symbolically.

      // FIXME: Expand these checks to include all undefined behavior.

      if (V2.isSigned() && V2.isNegative())
        return nullptr;

      uint64_t Amt = V2.getZExtValue();

      if (Amt >= V1.getBitWidth())
        return nullptr;

      return &getValue( V1.operator>>( (unsigned) Amt ));
    }

    case BO_LT:
      return &getTruthValue( V1 < V2 );

    case BO_GT:
      return &getTruthValue( V1 > V2 );

    case BO_LE:
      return &getTruthValue( V1 <= V2 );

    case BO_GE:
      return &getTruthValue( V1 >= V2 );

    case BO_EQ:
      return &getTruthValue( V1 == V2 );

    case BO_NE:
      return &getTruthValue( V1 != V2 );

      // Note: LAnd, LOr, Comma are handled specially by higher-level logic.

    case BO_And:
      return &getValue( V1 & V2 );

    case BO_Or:
      return &getValue( V1 | V2 );

    case BO_Xor:
      return &getValue( V1 ^ V2 );
  }
}


const std::pair<SVal, uintptr_t>&
BasicValueFactory::getPersistentSValWithData(const SVal& V, uintptr_t Data) {

  // Lazily create the folding set.
  if (!PersistentSVals) PersistentSVals = new PersistentSValsTy();

  llvm37::FoldingSetNodeID ID;
  void *InsertPos;
  V.Profile(ID);
  ID.AddPointer((void*) Data);

  PersistentSValsTy& Map = *((PersistentSValsTy*) PersistentSVals);

  typedef llvm37::FoldingSetNodeWrapper<SValData> FoldNodeTy;
  FoldNodeTy* P = Map.FindNodeOrInsertPos(ID, InsertPos);

  if (!P) {
    P = (FoldNodeTy*) BPAlloc.Allocate<FoldNodeTy>();
    new (P) FoldNodeTy(std::make_pair(V, Data));
    Map.InsertNode(P, InsertPos);
  }

  return P->getValue();
}

const std::pair<SVal, SVal>&
BasicValueFactory::getPersistentSValPair(const SVal& V1, const SVal& V2) {

  // Lazily create the folding set.
  if (!PersistentSValPairs) PersistentSValPairs = new PersistentSValPairsTy();

  llvm37::FoldingSetNodeID ID;
  void *InsertPos;
  V1.Profile(ID);
  V2.Profile(ID);

  PersistentSValPairsTy& Map = *((PersistentSValPairsTy*) PersistentSValPairs);

  typedef llvm37::FoldingSetNodeWrapper<SValPair> FoldNodeTy;
  FoldNodeTy* P = Map.FindNodeOrInsertPos(ID, InsertPos);

  if (!P) {
    P = (FoldNodeTy*) BPAlloc.Allocate<FoldNodeTy>();
    new (P) FoldNodeTy(std::make_pair(V1, V2));
    Map.InsertNode(P, InsertPos);
  }

  return P->getValue();
}

const SVal* BasicValueFactory::getPersistentSVal(SVal X) {
  return &getPersistentSValWithData(X, 0).first;
}
