#include "HornClause.h"

HornClause::HornClause(z3::context & ctx, z3::expr_vector antecedent, z3::expr consequent, UnionFindExplain & ufe) :
  ctx(ctx),
  antecedent(antecedent), consequent(consequent), 
  is_common_antecedent(true), local_max_lit_id(0),
  is_leader(true)
{

  // ----------------
  normalize(ufe);  //
  orient();        //
  // ----------------

  // ---------------------------------------------------------------
  // This part updates:
  // 1) is_common_antecedent
  for(auto hyp : this->antecedent){
    if(local_max_lit_id < hyp.id())
      local_max_lit_id = hyp.id();
    if(!hyp.is_common())
      is_common_antecedent = false;
  }
  if(local_max_lit_id < this->consequent.id()){
    local_max_lit_id = this->consequent.id();
  }
  // ---------------------------------------------------------------
  return;
}

HornClause::HornClause(z3::context & ctx, z3::expr_vector antecedent, z3::expr consequent) :
  ctx(ctx),
  antecedent(antecedent), consequent(consequent), 
  is_common_antecedent(true), local_max_lit_id(0),
  is_leader(true)
{

  // ----------------
  // We dont' normalize the 
  // produce Horn clause since
  // the antecedent will be produced
  // by some explanation, which means 
  // that the equations in the antecedent
  // will be considered trivial by 
  // the normalization procedure
  //normalize(cce);//
  orient();        //
  // ----------------

  // ---------------------------------------------------------------
  // This part updates:
  // 1) num_uncomm_antecedent
  // 2) is_common_antecedent
  for(auto hyp : this->antecedent){
    if(local_max_lit_id < hyp.id())
      local_max_lit_id = hyp.id();
    if(!hyp.is_common())
      is_common_antecedent = false;
  }
  if(local_max_lit_id < this->consequent.id()){
    local_max_lit_id = this->consequent.id();
  }
  // ---------------------------------------------------------------
  return;
}


HornClause::~HornClause(){
#if DEBUG_DESTRUCTOR_HC
  std::cout << "Done ~HornClause" << std::endl;
#endif
}

// Removes trivial equalities in the antecedent
// sorting these elements using the following heuristic:
// compareEquation from Util
void HornClause::normalize(UnionFindExplain & ufe){
  std::vector<z3::expr> aux_antecedent ({});

  for(auto expr : antecedent)
    aux_antecedent.push_back(expr);

  std::sort(aux_antecedent.begin(), aux_antecedent.end(), Util::compareEquation);
  antecedent.resize(0);

  for(auto expr : aux_antecedent){
    if(ufe.find(expr.arg(0).id()) != ufe.find(expr.arg(1).id()))
      antecedent.push_back(expr);
  }
  return;
}

// Removes trivial equalities in the antecedent
// sorting these elements using the following heuristic:
// compareEquation from Util
void HornClause::normalize(CongruenceClosureExplain & cce){
  std::vector<z3::expr> aux_antecedent ({});

  for(auto expr : antecedent)
    aux_antecedent.push_back(expr);

  std::sort(aux_antecedent.begin(), aux_antecedent.end(), Util::compareEquation);
  antecedent.resize(0);

  for(auto expr : aux_antecedent){
    if(cce.areSameClass(expr.arg(0), expr.arg(1)))
      antecedent.push_back(expr);
  }
  return;
}


// Rearranges a Horn Clauses to the form
// (/\_i u_i = v_i) => a = b, where u_i >= v_i and a >= b
// The < relation on (Term, Term) is HornClause::compareTerm
void HornClause::orient(){

  z3::expr current_lhs(ctx), current_rhs(ctx);

  z3::expr_vector aux_antecedent(ctx);
  for(auto expr : antecedent){
    current_lhs = expr.arg(0), current_rhs = expr.arg(1);
    if(Util::compareTerm(current_lhs, current_rhs))
      aux_antecedent.push_back(current_rhs == current_lhs);
    else
      aux_antecedent.push_back(expr);
  }
  antecedent = aux_antecedent;
  
  std::string consequent_name = consequent.decl().name().str();
  if(consequent_name == "false")
    return;

  current_lhs = consequent.arg(0), current_rhs = consequent.arg(1);
  if(Util::compareTerm(current_lhs, current_rhs))
    consequent = (current_rhs == current_lhs);
  return;
}

bool HornClause::checkTriviality(UnionFindExplain & ufe){
  return ufe.find(consequent.arg(0).id()) == ufe.find(consequent.arg(1).id());
}

z3::expr_vector const & HornClause::getAntecedent() const {
  return antecedent;
}

const z3::expr & HornClause::getConsequent() const {
  return consequent;
}

bool HornClause::isCommonAntecedent() const {
  return is_common_antecedent;
}

bool HornClause::isCommonConsequent() const {
  return consequent.is_common();
}

bool HornClause::isCommon() const {
  return is_common_antecedent && consequent.is_common();
}

unsigned HornClause::numAntecedent() const {
  return antecedent.size();
}

unsigned HornClause::getLocalMaxLitId() const {
  return local_max_lit_id;
}

// Definition: > \in HornClause \times HornClause -> Bool
// Let hc1 be of the form (/\_{i=0}^m u_i   = v_i  ) => a_1 = a_2
// Let hc2 be of the form (/\_{j=0}^n u_j^' = v_j^') => a_1 = a_2
// hc1 < hc2 iff n < m
//               and |- (/\_j^n u_j^' = v_j^') => (/\_i^m u_i = v_i_)
// -------------------------------------------------------------------
bool operator < (HornClause const & hc1, HornClause const & hc2){
  // ------------------------------------------------
  // Precondition:
  // This comparison assumes the consequent of
  // hc1 and hc2 are equal
  // ------------------------------------------------
  assert(hc1.consequent.id() == hc2.consequent.id());

  if(hc1.antecedent.size() < hc2.antecedent.size()){
    z3::solver s(hc1.ctx, "QF_UF");
    s.add(z3::mk_and(hc2.antecedent));
    s.add(not(z3::mk_and(hc1.antecedent)));
    return s.check() == z3::unsat;
  }
  return false;
}


z3::expr HornClause::ToZ3Expr() const{
  unsigned antecedent_size = antecedent.size();
  switch(antecedent_size){
    case 0:
      return consequent;
    case 1:
      return z3::implies(antecedent[0], consequent);
    default:
      return z3::implies(mk_and(antecedent), consequent);
  }
}

bool HornClause::isLeader() const {
  return is_leader;
}

void HornClause::noLongerLeader(){
  is_leader = false;
}

bool operator > (HornClause const & hc1, HornClause const & hc2){
  return hc2 < hc1;
}

std::ostream & operator << (std::ostream & os, HornClause const & hc){
  bool first_flag = true;
  unsigned num_antecedent = hc.antecedent.size();
  os << (hc.isLeader() ? "(Leader) " : "(Not leader) ");
  for(unsigned i = 0; i < num_antecedent; i++){
    if(!first_flag)
      os << " and ";
    first_flag = false;
    os << hc.antecedent[i];
  }
  os << " -> " << hc.consequent;
  return os;
}
