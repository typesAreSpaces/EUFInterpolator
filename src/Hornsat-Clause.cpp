#include "Hornsat.h"

Hornsat::Clause::Clause(ClauseId id, struct Clause * clause) : 
  clause_id(id), next(clause)
{
}

Hornsat::Clause::~Clause(){
#if DEBUG_DESTRUCTORS
  std::cout << "Done ~Clause with " << clause_id  << std::endl;
#endif
}

struct Hornsat::Clause * Hornsat::Clause::add(ClauseId element){
  return new Clause(element, this);
}

Hornsat::Clause::iterator Hornsat::Clause::begin() const {
  return Hornsat::Clause::iterator(this);
}

Hornsat::Clause::iterator Hornsat::Clause::end() const {
  return Hornsat::Clause::iterator(nullptr);
}

