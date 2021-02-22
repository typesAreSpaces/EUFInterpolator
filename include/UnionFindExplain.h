#ifndef UNIONFINDEXPLAIN_H
#define UNIONFINDEXPLAIN_H
#define DEBUG_DESTRUCTOR_UFE 0
#define DEBUG_EXPLAIN_OP     0

#include <list>
#include <unordered_map>
#include <algorithm>
#include "UnionFind.h"
#include "CurryNode.h"

struct ExplainEquation {
  EqClass source, target;
  
  ExplainEquation(EqClass source, EqClass target) :
    source(source), target(target) {}

  friend std::ostream & operator << (std::ostream & os, const ExplainEquation & eq){
    os << eq.source << " := " << eq.target;
    return os;
  }
};

typedef std::list<ExplainEquation> ExplainEquations;
 
class UnionFindExplain :  public UnionFind {
  
  std::vector<EqClass>                proof_forest;
  std::vector<const PendingElement *> labels;

  void     unionReverseEdges(EqClass, EqClass);
  unsigned depth(EqClass);
  EqClass  commonAncestorHelper(EqClass, EqClass, unsigned);
  void     explainAlongPath(EqClass, EqClass, ExplainEquations &);
  
public:
  UnionFindExplain();
  UnionFindExplain(unsigned);
  UnionFindExplain(const UnionFindExplain &);
  ~UnionFindExplain();

  EqClass          parentProofForest(EqClass);
  ExplainEquations explain(EqClass, EqClass);
  void             combine(EqClass, EqClass, const PendingElement * = nullptr);
  void             merge(EqClass, EqClass, const PendingElement * = nullptr);
  EqClass          commonAncestor(EqClass, EqClass);

  std::ostream & giveExplanation(std::ostream &, EqClass, EqClass);
  void           resize(unsigned);
  const PendingElement * getLabel(EqClass);

  friend std::ostream & operator << (std::ostream &, UnionFindExplain &);
};

#endif
