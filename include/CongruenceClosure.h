#ifndef _CONG_CLOSURE__
#define _CONG_CLOSURE__
#define DEBUG_DESTRUCTORS_CC false

#include <iostream>
#include <list>
#include <utility>
#include "Z3Subterms.h"
#include "SignatureTable.h"

class CongruenceClosure {

  friend class Hornsat;

protected:
  Z3Subterms const & subterms;
  UnionFindExplain & ufe;
  SignatureTable     sig_table;

 public:
  CongruenceClosure(
      Z3Subterms const &, 
      UnionFindExplain &);

  virtual ~CongruenceClosure();

  UnionFindExplain & getUnionFindExplain() const;

  friend std::ostream & operator << (std::ostream &, CongruenceClosure const &);
};

#endif
