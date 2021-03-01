#ifndef _LOOKUP_TABLE_
#define _LOOKUP_TABLE_

#include "CongruenceClosure.h"
#include "FactoryCurryNodes.h"


class LookupTable {
  typedef unsigned long long ull;

  std::unordered_map<std::size_t, CurryNode::EquationCurryNodes const *> sig_table;

  public:
  LookupTable();
  ~LookupTable();

  std::size_t hash_combine(UnionFind::EqClass, UnionFind::EqClass);

  void enter(UnionFind::EqClass , UnionFind::EqClass, CurryNode::EquationCurryNodes const *);
  void erase(UnionFind::EqClass, UnionFind::EqClass);
  const CurryNode::EquationCurryNodes * query(UnionFind::EqClass, UnionFind::EqClass);

  friend std::ostream & operator << (std::ostream &, LookupTable const &);

};

#endif
