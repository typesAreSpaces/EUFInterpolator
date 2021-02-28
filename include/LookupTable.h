#ifndef _LOOKUP_TABLE_
#define _LOOKUP_TABLE_

#include "CongruenceClosure.h"
#include "FactoryCurryNodes.h"

typedef unsigned long long ull;

class LookupTable {
  std::unordered_map<std::size_t, EquationCurryNodes const *> sig_table;

  public:
  LookupTable();
  ~LookupTable();

  std::size_t hash_combine(UnionFind::EqClass, UnionFind::EqClass);
  
  void enter(UnionFind::EqClass , UnionFind::EqClass, EquationCurryNodes const *);
  void erase(UnionFind::EqClass, UnionFind::EqClass);
  const EquationCurryNodes * query(UnionFind::EqClass, UnionFind::EqClass);
  
  friend std::ostream & operator << (std::ostream &, LookupTable const &);
  
};

#endif
