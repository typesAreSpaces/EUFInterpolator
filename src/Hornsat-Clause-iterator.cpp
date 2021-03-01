#include "Hornsat.h"

Hornsat::Clause::iterator::iterator(Hornsat::Clause const * n) : 
  it(n)
{
}

bool Hornsat::Clause::iterator:: operator ==(Hornsat::Clause::iterator const & other){
  return it == other.it;
}

bool Hornsat::Clause::iterator:: operator !=(Hornsat::Clause::iterator const & other){
  return it != other.it;
}

Hornsat::Clause::iterator & Hornsat::Clause::iterator:: operator ++(){
  if(it)
    it = it->next;
  return *this;
}

struct Hornsat::Clause const * Hornsat::Clause::iterator:: operator *() const {
  return it;
}

