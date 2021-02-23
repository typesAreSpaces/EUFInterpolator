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

  std::size_t hash_combine(EqClass, EqClass);
  
  void enter(EqClass , EqClass, EquationCurryNodes const *);
  void erase(EqClass, EqClass);
  const EquationCurryNodes * query(EqClass, EqClass);
  
  friend std::ostream & operator << (std::ostream &, LookupTable const &);
  
};

#endif
