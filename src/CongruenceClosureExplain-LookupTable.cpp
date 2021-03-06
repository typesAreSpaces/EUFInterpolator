#include "CongruenceClosureExplain.h"
CongruenceClosureExplain::LookupTable::LookupTable()
{
}

CongruenceClosureExplain::LookupTable::~LookupTable(){
#if DEBUG_DESTRUCTORS_CC
  std::cout << "Done ~LookupTable" << std::endl;
#endif
}

std::size_t CongruenceClosureExplain::LookupTable::hash_combine(UnionFind::EqClass a1, UnionFind::EqClass a2){
  return std::hash<ull>()(((ull) a1) ^ (((ull) a2) << 32));
}

void CongruenceClosureExplain::LookupTable::enter(UnionFind::EqClass a1, UnionFind::EqClass a2, 
    CurryNode::EquationCurryNodes const * ecn){
  auto index = hash_combine(a1, a2);
  sig_table[index] = ecn;
  return;
}

void CongruenceClosureExplain::LookupTable::erase(UnionFind::EqClass a1, UnionFind::EqClass a2){
  sig_table.erase(hash_combine(a1, a2));
}

const CurryNode::EquationCurryNodes * CongruenceClosureExplain::LookupTable::query(
    UnionFind::EqClass a1, UnionFind::EqClass a2){
  auto r = sig_table.find(hash_combine(a1, a2));
  if(r == sig_table.end())
    return nullptr;
  return r->second;
}

