#ifndef _CONG_CLOSURE_E__
#define _CONG_CLOSURE_E__
#define DEBUG_SANITY_CHECK  0
#define DEBUG_MERGE         0
#define DEBUG_PROPAGATE     0
#define DEBUG_PROPAGATE_AUX 0
#define DEBUG_TEST_EXPLAIN  0
#define DEBUG_CONSTRUCT_CCE 0

//#include "LookupTable.h"
#include "CongruenceClosure.h"
#include "FactoryCurryNodes.h"

class Hornsat;

class CongruenceClosureExplain : public CongruenceClosure {

  class LookupTable {
    typedef unsigned long long ull;

    std::unordered_map<
      std::size_t, 
      CurryNode::EquationCurryNodes const *> sig_table;

    public:
    LookupTable();
    ~LookupTable();

    std::size_t hash_combine(UnionFind::EqClass, UnionFind::EqClass);

    void enter(UnionFind::EqClass , UnionFind::EqClass, CurryNode::EquationCurryNodes const *);
    void erase(UnionFind::EqClass, UnionFind::EqClass);
    const CurryNode::EquationCurryNodes * query(UnionFind::EqClass, UnionFind::EqClass);

  };

  typedef std::vector<std::list<CurryNode::EquationCurryNodes const *> > UseList;

  Hornsat * hsat;

  CurryNode::CurryNode::PendingElements pending_elements;
  CurryNode::PendingPointers equations_to_merge;
  CurryNode::PendingPointers pending_to_propagate;

  FactoryCurryNodes const & factory_curry_nodes;

  LookupTable lookup_table;
  UseList     use_list;

  void pushPending(CurryNode::PendingPointers &, CurryNode::PendingElement const &);
  void merge();
  void merge(CurryNode::EquationCurryNodes const &);
  void propagate();
  void propagateAux(CurryNode const &, CurryNode const &, 
      UnionFind::EqClass, UnionFind::EqClass, CurryNode::PendingElement const &);

  UnionFind::EqClass highestNode(UnionFind::EqClass, UnionFind &);
  UnionFind::EqClass nearestCommonAncestor(UnionFind::EqClass, UnionFind::EqClass, UnionFind &);
  CurryNode::PendingPointers    explain(UnionFind::EqClass, UnionFind::EqClass);
  void               explainAlongPath(UnionFind::EqClass, UnionFind::EqClass, 
      UnionFind &, UnionFindExplain::ExplainEquations &, CurryNode::PendingPointers &);
  std::ostream &     giveExplanation(std::ostream &, UnionFind::EqClass, UnionFind::EqClass);

  public:
  CongruenceClosureExplain(Hornsat *, CongruenceClosureExplain const &, UnionFindExplain &);
  CongruenceClosureExplain(Z3Subterms const &, UnionFindExplain &, FactoryCurryNodes &, CurryNode::IdsToMerge const &);
  ~CongruenceClosureExplain();

  bool areSameClass(UnionFind::EqClass, UnionFind::EqClass);
  bool areSameClass(z3::expr const &, z3::expr const &);

  UnionFind::EqClass constantId(UnionFind::EqClass);
  UnionFind::EqClass find(UnionFind::EqClass);
  z3::expr           z3Repr(z3::expr const &);

  void merge(UnionFind::EqClass, UnionFind::EqClass);
  void merge(z3::expr const &, z3::expr const &);

  CurryNode::PendingPointers explain(z3::expr const &, z3::expr const &);
  std::ostream &  giveExplanation(std::ostream &, z3::expr const &, z3::expr const &);

  z3::expr_vector z3Explain(z3::expr const &, z3::expr const &);
  std::ostream &  z3Explanation(std::ostream &, z3::expr const &, z3::expr const &);

  friend std::ostream & operator << (std::ostream &, CongruenceClosureExplain const &);
};

#endif
