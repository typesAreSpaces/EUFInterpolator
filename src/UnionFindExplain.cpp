#include "UnionFindExplain.h"

UnionFindExplain::UnionFindExplain() :
  UnionFind(0) 
{ 
};

UnionFindExplain::UnionFindExplain(unsigned size) :
  UnionFind(size), proof_forest(size, 0), labels(size, nullptr) 
{
  for(unsigned i = 0; i < size; i++)
    proof_forest[i] = i;
}

UnionFindExplain::UnionFindExplain(const UnionFindExplain & other) :
  UnionFind(other), proof_forest(other.proof_forest), labels(other.labels) 
{
}

UnionFindExplain::~UnionFindExplain(){
#if DEBUG_DESTRUCTOR_UFE
  std::cout << "Done ~UnionFindExplain" << std::endl;
#endif
};

void UnionFindExplain::unionReverseEdges(UnionFind::EqClass target, UnionFind::EqClass source){
  assert(target < size && source < size);
  if(find(target) == find(source))
    return;
  // ---------------------------------------------------------------------
  // Reverse the edges between source
  // and its representative
  std::list<ExplainEquation> stack;
  UnionFind::EqClass aux_source = source;
  while(aux_source != proof_forest[aux_source]) {
    stack.emplace_back(aux_source, proof_forest[aux_source]);
    aux_source = proof_forest[aux_source];
  }
  while(!stack.empty()) {
    const auto element = stack.back();
    stack.pop_back();
    proof_forest[element.target] = element.source;
    std::swap(labels[element.target], labels[element.source]);
    if(stack.empty())
      proof_forest[element.source] = element.source;
  }
  // ---------------------------------------------------------------------
  proof_forest[source] = target;  
}

unsigned UnionFindExplain::depth(UnionFind::EqClass x){
  UnionFind::EqClass aux = x;
  int depth = 0;
  while(aux != proof_forest[aux]){
    depth++;
    aux = proof_forest[aux];
  }
  return depth;
}

UnionFind::EqClass UnionFindExplain::commonAncestorHelper(UnionFind::EqClass aux_x, UnionFind::EqClass aux_y, unsigned depth_diff){
  assert(find(aux_x) == find(aux_y));
  while(depth_diff--)
    aux_x = proof_forest[aux_x];
  while(aux_x != aux_y){
    aux_x = proof_forest[aux_x];
    aux_y = proof_forest[aux_y];
  }
  return aux_x;
}

UnionFind::EqClass UnionFindExplain::commonAncestor(UnionFind::EqClass x, UnionFind::EqClass y) {
  if(find(x) == find(y)){
    unsigned depth_x = depth(x), depth_y = depth(y);
    if(depth_x >= depth_y)
      return commonAncestorHelper(x, y, depth_x - depth_y);
    return commonAncestorHelper(y, x, depth_y - depth_x);
  }
  throw "The nodes are not in the same equivalence class";
}

void UnionFindExplain::explainAlongPath(UnionFind::EqClass node, UnionFind::EqClass representative, ExplainEquations & explanations) {
  while(node != representative){
    explanations.emplace_back(proof_forest[node], node);
    node = proof_forest[node];
  }
  return;
} 

UnionFind::EqClass UnionFindExplain::parentProofForest(UnionFind::EqClass x){
  assert(x < size);
  return proof_forest[x];
}

UnionFindExplain::ExplainEquations UnionFindExplain::explain(UnionFind::EqClass x, UnionFind::EqClass y){
  ExplainEquations explanations;
  UnionFind::EqClass common_ancestor_x_y;
  try {
    common_ancestor_x_y = commonAncestor(x, y);
  }
  catch(...){
    return explanations;
  } 
  explainAlongPath(x, common_ancestor_x_y, explanations);
  explainAlongPath(y, common_ancestor_x_y, explanations);
  return explanations;
}

// The first argument becomes the new
// representative, always.
void UnionFindExplain::combine(UnionFind::EqClass target, UnionFind::EqClass source, const CurryNode::PendingElement * pe){
  if(find(target) == find(source))
    return;
  unionReverseEdges(target, source);
  UnionFind::combine(target, source); 
  labels[source] = pe;
  return;
}

void UnionFindExplain::merge(UnionFind::EqClass target, UnionFind::EqClass source, const CurryNode::PendingElement * pe){
  if(find(target) == find(source))
    return;
  if(rank[find(target)] >= rank[find(source)])
    unionReverseEdges(target, source);
  else
    unionReverseEdges(source, target);
  UnionFind::merge(target, source);
  labels[source] = pe;
  return;
}

std::ostream & UnionFindExplain::giveExplanation(std::ostream & os, UnionFind::EqClass x, UnionFind::EqClass y){
  os << "Explain " << x << ", " << y << std::endl;
  auto explanation = explain(x, y);
  if(explanation.size() == 0)
    return (os << x << " and " << y << " belong to different equivalent classes" << std::endl);
  for(auto z : explanation) 
    os << z << std::endl;
  return os;
}

void UnionFindExplain::resize(unsigned sz){
  representative.resize(sz);
  rank.resize(sz);
  proof_forest.resize(sz);
  labels.resize(sz);

  for(unsigned i = size; i < sz; i++){
    representative[i] = i;
    rank[i] = 1;
    proof_forest[i] = i;
    labels[i] = nullptr;
  }

  size = sz;
}

const CurryNode::PendingElement * UnionFindExplain::getLabel(
    UnionFind::EqClass x){
  return labels[x];
}

std::ostream & operator << (std::ostream & os, UnionFindExplain & uf){
  unsigned num_changes = 0;
  for(unsigned i = 0; i < uf.representative.size(); ++i){
    if(i != uf.find(i)) {
      num_changes++;
      os << "(Different)";
    }
    else
      os << "(Same)";
    os << "ID: " << i
      << " Forest: " << uf.proof_forest[i]
      << " Representative " << uf.find(i)
      << std::endl;
  }
  os << "Size " << uf.size << std::endl;
  os << "Num changes: " << num_changes;
  return os;
}
