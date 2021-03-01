#ifndef _CONG_CLOSURE__
#define _CONG_CLOSURE__
#define DEBUG_DESTRUCTORS_CC false

#include <iostream>
#include <list>
#include <utility>
#include <unordered_map>
#include "Z3Subterms.h"
#include "UnionFindExplain.h"
#include "Util.h"

class CongruenceClosure {

  friend class Hornsat;

  protected:
  Z3Subterms const & subterms;
  UnionFindExplain & ufe;
  // This structure is not used
  // in the CongruenceClosureExplain
  // class
  //SignatureTable     sig_table;

  public:
  CongruenceClosure(
      Z3Subterms const &, 
      UnionFindExplain &);

  virtual ~CongruenceClosure();

  friend std::ostream & operator << (std::ostream &, CongruenceClosure const &);
};

#endif
