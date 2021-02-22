#ifndef _CONG_CLOSURENO__
#define _CONG_CLOSURENO__

#include "CongruenceClosure.h"

class CongruenceClosureNO : public CongruenceClosure {
  friend class Hornsat;
  PredList & pred_list;
 public:
  CongruenceClosureNO(const Z3Subterms &, PredList &, UnionFindExplain &);
  void combine(unsigned, unsigned);
  bool areCongruent(unsigned, unsigned);
  ~CongruenceClosureNO();
  friend std::ostream & operator << (std::ostream &, const CongruenceClosureNO &);
};


#endif
