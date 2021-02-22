#ifndef _CONG_CLOSURE_E__
#define _CONG_CLOSURE_E__
#define DEBUG_SANITY_CHECK  0
#define DEBUG_MERGE         0
#define DEBUG_PROPAGATE     0
#define DEBUG_PROPAGATE_AUX 0
#define DEBUG_TEST_EXPLAIN  0
#define DEBUG_CONSTRUCT_CCE 0

#include "CongruenceClosure.h"
#include "FactoryCurryNodes.h"

class LookupTable {
  std::unordered_map<std::size_t, const EquationCurryNodes*> sig_table;
  //std::hash<EqClass> EqClass_hasher;

  public:
  LookupTable() {}
  ~LookupTable(){
#if DEBUG_DESTRUCTORS_CC
    std::cout << "Done ~LookupTable" << std::endl;
#endif
  }
  //std::size_t hash_combine(EqClass a1, EqClass a2){
    //std::size_t seed = EqClass_hasher(a1);
    //seed ^= EqClass_hasher(a2) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    //return seed;
  //}
  std::size_t hash_combine(EqClass a1, EqClass a2){
    return std::hash<unsigned long long>()(((unsigned long long) a1) ^ (((unsigned long long) a2) << 32));
  }
  void enter(EqClass a1, EqClass a2, const EquationCurryNodes * ecn){
    auto index = hash_combine(a1, a2);
    sig_table[index] = ecn;
    return;
  }
  void erase(EqClass a1, EqClass a2){
    sig_table.erase(hash_combine(a1, a2));
  }
  const EquationCurryNodes * query(EqClass a1, EqClass a2){
    auto r = sig_table.find(hash_combine(a1, a2));
    if(r == sig_table.end())
      return nullptr;
    return r->second;
  }
  friend std::ostream & operator << (std::ostream & os, const LookupTable & lt){
    for(auto x : lt.sig_table)
      os << *(x.second) << std::endl;
    os << "Size of lookup table: " << lt.sig_table.size();
    return os;
  }
};

typedef std::vector<std::list<const EquationCurryNodes *> > UseList;

class Hornsat;

class CongruenceClosureExplain : public CongruenceClosure {

  Hornsat * hsat;

  PendingElements pending_elements;
  PendingPointers equations_to_merge;
  PendingPointers pending_to_propagate;

  FactoryCurryNodes const & factory_curry_nodes;

  LookupTable lookup_table;
  UseList     use_list;

  void pushPending(PendingPointers &, const PendingElement &);
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
  std::ostream &  z3Explanation(std::ostream &, const z3::expr &, const z3::expr &);

  friend std::ostream & operator << (std::ostream &, const CongruenceClosureExplain &);
};

#endif
