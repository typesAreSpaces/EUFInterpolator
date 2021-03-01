#ifndef _SIG_TABLE_
#define _SIG_TABLE_ 

#include <iostream>
#include <unordered_map>
#include "UnionFindExplain.h"
#include "Util.h"

class SignatureTable {
  std::unordered_map<std::size_t, unsigned> sig_table;
  UnionFindExplain &                        uf;
  
  std::size_t hash_z3expr(z3::expr const &);

public:
  SignatureTable(UnionFindExplain &);
  ~SignatureTable();
  
  void enter(z3::expr const &); 
  void erase(z3::expr const &);
  unsigned query(z3::expr const &); 

  friend std::ostream & operator << (std::ostream & os, SignatureTable const &);
};

#endif
