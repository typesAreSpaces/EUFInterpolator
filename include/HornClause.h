#ifndef _HORN_CLAUSE_
#define _HORN_CLAUSE_
#define DEBUG_DESTRUCTOR_HC 0

#include <algorithm>
#include <list>
#include "z3++.h"
#include "UnionFindExplain.h"
#include "CongruenceClosureExplain.h"
#include "Z3Subterms.h"
#include "Util.h"

class HornClause {

  z3::context &   ctx;
  z3::expr_vector antecedent;
  z3::expr        consequent;
  bool            is_common_antecedent;
  unsigned        local_max_lit_id;
  bool            is_leader;

  void normalize(UnionFindExplain &);
  void normalize(CongruenceClosureExplain &);
  void orient();
  
public:
  HornClause(z3::context &, z3::expr_vector, z3::expr, UnionFindExplain &);
  HornClause(z3::context &, z3::expr_vector, z3::expr);
  ~HornClause();
  
  bool                    checkTriviality(UnionFindExplain &);
  const z3::expr_vector & getAntecedent()       const;
  const z3::expr &        getConsequent()       const;
  bool                    isCommonAntecedent()  const;
  bool                    isCommonConsequent()  const;
  bool                    isCommon()            const;
  unsigned                numAntecedent()       const;
  unsigned                getLocalMaxLitId()    const;
  z3::expr                ToZ3Expr()            const;
  bool                    isLeader()            const;
  void                    noLongerLeader();

  friend bool operator <(HornClause const &, HornClause const &);
  friend bool operator >(HornClause const &, HornClause const &);
  
  friend std::ostream & operator << (std::ostream &, HornClause const &);
};

#endif
