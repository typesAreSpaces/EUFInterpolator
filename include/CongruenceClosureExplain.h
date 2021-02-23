#ifndef _CONG_CLOSURE_E__
#define _CONG_CLOSURE_E__
#define DEBUG_SANITY_CHECK  0
#define DEBUG_MERGE         0
#define DEBUG_PROPAGATE     0
#define DEBUG_PROPAGATE_AUX 0
#define DEBUG_TEST_EXPLAIN  0
#define DEBUG_CONSTRUCT_CCE 0

#include "CongruenceClosure.h"
#include "LookupTable.h"

typedef std::vector<std::list<EquationCurryNodes const *> > UseList;

class Hornsat;

class CongruenceClosureExplain : public CongruenceClosure {

  Hornsat * hsat;

  PendingElements pending_elements;
  PendingPointers equations_to_merge;
  PendingPointers pending_to_propagate;

  FactoryCurryNodes const & factory_curry_nodes;

  LookupTable lookup_table;
  UseList     use_list;

  void pushPending(PendingPointers &, PendingElement const &);
  void merge();
  void merge(EquationCurryNodes const &);
  void propagate();
  void propagateAux(CurryNode const &, CurryNode const &, EqClass, EqClass, PendingElement const &);

  EqClass         highestNode(EqClass, UnionFind &);
  EqClass         nearestCommonAncestor(EqClass, EqClass, UnionFind &);
  PendingPointers explain(EqClass, EqClass);
  void            explainAlongPath(EqClass, EqClass, UnionFind &, ExplainEquations &, PendingPointers &);
  std::ostream &  giveExplanation(std::ostream &, EqClass, EqClass);

  public:
  CongruenceClosureExplain(Hornsat *, CongruenceClosureExplain const &, UnionFindExplain &);
  CongruenceClosureExplain(Z3Subterms const &, UnionFindExplain &, FactoryCurryNodes &, IdsToMerge const &);
  ~CongruenceClosureExplain();

  bool areSameClass(EqClass, EqClass);
  bool areSameClass(z3::expr const &, z3::expr const &);
  
  EqClass  constantId(EqClass);
  EqClass  find(EqClass);
  z3::expr z3Repr(z3::expr const &);

  void merge(EqClass, EqClass);
  void merge(z3::expr const &, z3::expr const &);

  PendingPointers explain(z3::expr const &, z3::expr const &);
  std::ostream &  giveExplanation(std::ostream &, z3::expr const &, z3::expr const &);

  z3::expr_vector z3Explain(z3::expr const &, z3::expr const &);
  std::ostream &  z3Explanation(std::ostream &, z3::expr const &, z3::expr const &);

  friend std::ostream & operator << (std::ostream &, CongruenceClosureExplain const &);
};

#endif
