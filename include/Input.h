#ifndef _INPUT_
#define _INPUT_

#include <map>
#include "Hornsat.h"
#include "CongruenceClosureExplain.h"
#include "CurryNode.h"
#include "Rename.h"

struct Input {

  typedef std::map<std::string, std::vector<unsigned> > FSymPositions;
  
  unsigned original_num_terms;
  
  z3::context &   ctx;
  Z3Subterms      subterms;
  z3::expr        contradiction;
  z3::expr_vector disequalities;
  
  FSymPositions    fsym_positions;
  UnionFindExplain ufe;
  HornClauses      horn_clauses;

  CurryNode::IdsToMerge ids_to_merge;

  CurryNode::CurryDeclarations curry_decl;  
  FactoryCurryNodes            factory_curry_nodes;
  CongruenceClosureExplain     cce;

  unsigned maxIdFromAssertions(z3::expr_vector const &);
  void     init(z3::expr_vector const &);
  void     initFormula(z3::expr const &);
  void     disequalitiesToHCS();

  Input(z3::expr_vector const &);
  ~Input();

  z3::expr z3_repr(z3::expr const &);
  friend std::ostream & operator << (std::ostream &, Input const &);
};

#endif
