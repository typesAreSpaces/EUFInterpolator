#include "Z3Subterms.h"

Z3Subterms::Z3Subterms(z3::context & ctx): 
  subterms(ctx), visited() {
}

Z3Subterms::iterator Z3Subterms::begin() const { 
  auto it = iterator(this, 0);
  if(it.notValidPosition())
    ++it;
  return it; 
}

Z3Subterms::iterator Z3Subterms::end() const { 
  return iterator(this, subterms.size()); 
}

void Z3Subterms::resize(unsigned size) { subterms.resize(size); visited.resize(size, false); }

unsigned Z3Subterms::size() const { return subterms.size(); }

void Z3Subterms::set(unsigned index, const z3::expr & e) {
  subterms.set(index, (z3::expr &) e);
  visited[e.id()] = true;
}

z3::expr Z3Subterms::operator[](unsigned i) const {
  if(i < visited.size() && visited[i])
    return subterms[i];
  throw "Element not defined";
}

std::ostream & operator << (std::ostream & os, Z3Subterms const & z3subterms){
  for(auto it = z3subterms.begin(); it != z3subterms.end(); ++it){
    os << it.getIndex() << ". " << *it << std::endl;
  }
  return os;
}
