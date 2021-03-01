#include "Hornsat.h"

Hornsat::Literal::Literal(
    Hornsat::LiteralId literal_id, 
    bool val, 
    struct Hornsat::Clause * clause_list) :
  literal_id(literal_id), 
  l_id(0), r_id(0), 
  l_class(0), r_class(0),
  val(val), is_common(false),
  clause_list(clause_list)
{
}

Hornsat::Literal::Literal() : 
  Literal(curr_num_literals++, false, nullptr) 
{
}

Hornsat::Literal::~Literal(){
#if DEBUG_DESTRUCTORS
  std::cout << "Done ~Literal with " << literal_id  << std::endl;
#endif
}

void Hornsat::Literal::update(
    z3::expr const & literal, CongruenceClosureExplain & cce){
  this->l_id = literal.arg(0).id();
  this->r_id = literal.arg(1).id();
  this->l_class = cce.find(this->l_id); 
  this->r_class = cce.find(this->r_id);
  this->is_common = literal.is_common();
}

void Hornsat::Literal::update(
    z3::expr const & literal, CongruenceClosureExplain & cce, 
    ClauseId clause_id){
  this->update(literal, cce);
  this->clause_list = this->clause_list->add(clause_id);
}

std::ostream & operator << (std::ostream & os, Hornsat::Literal const & l){
  return (os 
      << "Literal: " << l.literal_id
      << " Equation: " << l.l_id << "=" << l.r_id 
      << " (" << l.l_class << "=" << l.r_class << ")"
      << " Val: " << l.val);
}
