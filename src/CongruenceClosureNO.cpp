#include "CongruenceClosureNO.h"

CongruenceClosureNO::CongruenceClosureNO(const Z3Subterms & subterms,
					 PredList & pred_list, UnionFindExplain & uf) :
  CongruenceClosure(subterms, uf),
  pred_list(pred_list)
{
}

CongruenceClosureNO::~CongruenceClosureNO(){
#if DEBUG_DESTRUCTORS_CC
  std::cout << "Done ~CongruenceClosureNO" << std::endl;
#endif
}

void CongruenceClosureNO::combine(unsigned u, unsigned v){
  if(ufe.find(u) == ufe.find(v))
    return;
  auto p_u = pred_list[u], p_v = pred_list[v];
  ufe.combine(u, v);
  // The following assumes each elemen in pred_list is sorted,
  // and unique
  pred_list[ufe.find(u)].merge(pred_list[ufe.find(v)]); 
  for(auto x : p_u)
    for(auto y : p_v)
      if(ufe.find(x) != ufe.find(y) && areCongruent(x,y))
	combine(x, y);
  return;
}

bool CongruenceClosureNO::areCongruent(unsigned x, unsigned y){
  auto term_x = subterms[x], term_y = subterms[y];
  if(sig_table.hash_z3expr(term_x) != sig_table.hash_z3expr(term_y))
    return false;
  assert(term_x.num_args() == term_y.num_args());
  unsigned num_args = term_x.num_args();
  for(unsigned i = 0; i < num_args; i++)
    if(ufe.find(term_x.arg(i)) != ufe.find(term_y.arg(i)))
      return false;
  return true;
}

std::ostream & operator << (std::ostream & os, const CongruenceClosureNO & cc){
  return os;
}
