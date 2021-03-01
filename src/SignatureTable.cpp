#include "SignatureTable.h"

SignatureTable::SignatureTable(UnionFindExplain & uf):
  uf(uf) 
{
}

SignatureTable::~SignatureTable(){
#if DEBUG_DESTRUCTORS_CC
  std::cout << "Done ~SignatureTable" << std::endl;
#endif
}

std::size_t SignatureTable::hash_z3expr(const z3::expr & e){
  unsigned num_args = e.num_args();
  std::string name = e.decl().name().str();
  std::size_t seed = Util::string_hasher(name);
  Util::hash_combine(seed, num_args, Util::unsigned_hasher);
  //seed ^= Util::unsigned_hasher(uf.find(e.arg(i).id())) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  for(unsigned i = 0; i < num_args; i++)
    Util::hash_combine(seed, uf.find(e.arg(i).id()), Util::unsigned_hasher);
  return seed;
}

void SignatureTable::enter(const z3::expr & e){
  sig_table[hash_z3expr(e)] = e.id();
}

void SignatureTable::erase(const z3::expr & e){
  sig_table.erase(hash_z3expr(e));
}

unsigned SignatureTable::query(z3::expr const & e){
  auto r = sig_table.find(hash_z3expr(e));
  if(r == sig_table.end())
    throw "Element not in the table";
  return r->second;
}

std::ostream & operator << (std::ostream & os, SignatureTable const & st){
  return os;
}
