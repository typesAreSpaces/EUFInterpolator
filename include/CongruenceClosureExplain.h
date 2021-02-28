#ifndef _CONG_CLOSURE_E__
#define _CONG_CLOSURE_E__
#define DEBUG_SANITY_CHECK  0
#define DEBUG_MERGE         0
#define DEBUG_PROPAGATE     0
#define DEBUG_PROPAGATE_AUX 0
#define DEBUG_TEST_EXPLAIN  0
#define DEBUG_CONSTRUCT_CCE 0

#include "LookupTable.h"

class Hornsat;

class CongruenceClosureExplain : public CongruenceClosure {

  typedef std::vector<std::list<EquationCurryNodes const *> > UseList;

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
  void propagateAux(CurryNode const &, CurryNode const &, 
      UnionFind::EqClass, UnionFind::EqClass, PendingElement const &);

  UnionFind::EqClass highestNode(UnionFind::EqClass, UnionFind &);
  UnionFind::EqClass nearestCommonAncestor(UnionFind::EqClass, UnionFind::EqClass, UnionFind &);
  PendingPointers    explain(UnionFind::EqClass, UnionFind::EqClass);
  void               explainAlongPath(UnionFind::EqClass, UnionFind::EqClass, 
      UnionFind &, UnionFindExplain::ExplainEquations &, PendingPointers &);
  std::ostream &     giveExplanation(std::ostream &, UnionFind::EqClass, UnionFind::EqClass);

  public:
  CongruenceClosureExplain(Hornsat *, CongruenceClosureExplain const &, UnionFindExplain &);
  CongruenceClosureExplain(Z3Subterms const &, UnionFindExplain &, FactoryCurryNodes &, IdsToMerge const &);
  ~CongruenceClosureExplain();

  bool areSameClass(UnionFind::EqClass, UnionFind::EqClass);
  bool areSameClass(z3::expr const &, z3::expr const &);

  UnionFind::EqClass  constantId(UnionFind::EqClass);
  UnionFind::EqClass  find(UnionFind::EqClass);
  z3::expr z3Repr(z3::expr const &);

  void merge(UnionFind::EqClass, UnionFind::EqClass);
  void merge(z3::expr const &, z3::expr const &);

  PendingPointers explain(z3::expr const &, z3::expr const &);
  std::ostream &  giveExplanation(std::ostream &, z3::expr const &, z3::expr const &);

  z3::expr_vector z3Explain(z3::expr const &, z3::expr const &);
  std::ostream &  z3Explanation(std::ostream &, z3::expr const &, z3::expr const &);

  friend std::ostream & operator << (std::ostream &, CongruenceClosureExplain const &);
};

#endif
