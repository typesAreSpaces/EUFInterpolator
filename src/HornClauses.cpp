#include "HornClauses.h"

HornClauses::iterator::iterator(UnOrdMapHornClauses::iterator it):
  it(it)
{
}

bool HornClauses::iterator::operator == (HornClauses::iterator const & other){
  return it == other.it;
}

bool HornClauses::iterator::operator != (HornClauses::iterator const & other){
  return it != other.it;
}

HornClauses::iterator & HornClauses::iterator::operator++(){
  ++it;
  return *this;
}

HornClause const * HornClauses::iterator::operator * () const {
  return it->second;
}

HornClauses::HornClauses(z3::context & ctx, UnionFindExplain & ufe, unsigned num_terms) : 
  ufe(ufe), curr_num_horn_clauses(0), max_lit_id(num_terms),
  hash_consed_hcs(ctx),
  simplification_table()
{
}

HornClauses::~HornClauses(){
  for(auto it : horn_clauses)
    delete it.second;
#if DEBUG_DESTRUCTOR_HCS
  std::cout << "Done ~HornClauses" << std::endl;
#endif
}

void HornClauses::filterCommons(){
  for(auto it = horn_clauses.begin(); it != horn_clauses.end(); ){
    if(it->second->isCommon())
      ++it;
    else
      it = horn_clauses.erase(it);
  }
}

void HornClauses::simplifyBlock(SimplHornEntry const & block){
  for(auto const & src : block){
    for(auto const & trgt : block){
      if(&src != &trgt 
          && src->isLeader() && trgt->isLeader()
          && src > trgt){
        src->noLongerLeader();
#if DEBUG_SIMPLIFY_BLOCK
        std::cout << "This hc is no longer a leader" << std::endl;
        std::cout << *src << std::endl;
#endif
      }
    }
  }
}

// This procedure removes
// unnecessayr Horn Clauses.
// It implements the following rule:
// C, D -> a     C -> a
// ---------------------
//       C -> a
void HornClauses::simplify(){

  // Index Horn clauses in horn_clauses
  // by the .id() of their consequent
  for(auto const & horn_clause : horn_clauses)
    simplification_table[horn_clause.second->getConsequent().id()]
      .push_back(horn_clause.second); 

  // Simplify by each block
  for(auto & entry : simplification_table){
    // -------------------------
    simplifyBlock(entry.second);
    // -------------------------

#if DEBUG_SIMPLIFY
    // Debug/Test outcome
    std::cout << entry.first << ". entries:" << std::endl;
    for(auto const & horn_clause : entry.second)
      std::cout << *horn_clause << std::endl;
    std::cout << std::endl;
#endif
  }
}

void HornClauses::swapHornClauses(unsigned i, unsigned j){
  auto temp = horn_clauses[i];
  horn_clauses[i] = horn_clauses[j];
  horn_clauses[j] = temp;
  return;
}

void HornClauses::add(HornClause * hc){
#if DEBUG_ADDINGHC
  std::cout << "Debugging HornClauses::add BEGIN" << std::endl;
  std::cout << *hc << std::endl;
  std::cout << "Debugging HornClauses::add END" << std::endl;
#endif
  z3::expr z3_expr_hc = hc->ToZ3Expr();
  unsigned id = z3_expr_hc.hash();
  auto it = horn_clauses.find(id);

  if(it != horn_clauses.end()){
#if DEBUG_ADDINGHC
    std::cout << "Not added" << std::endl;
#endif
    delete hc;
    return;
  }
  const z3::expr & consequent = hc->getConsequent();
  switch(consequent.decl().decl_kind()){
    case Z3_OP_EQ:
      if(hc->checkTriviality(ufe)){
        delete hc;
#if DEBUG_ADDINGHC
        std::cout << "Not added" << std::endl;
#endif
        return;
      }
#if DEBUG_ADDINGHC
      std::cout << "Added" << std::endl;
#endif
      horn_clauses.insert({id, hc});
      if(max_lit_id < hc->getLocalMaxLitId())
        max_lit_id = hc->getLocalMaxLitId();
      curr_num_horn_clauses++;
      return;
    case Z3_OP_FALSE:
#if DEBUG_ADDINGHC
      std::cout << "Added" << std::endl;
#endif
      horn_clauses.insert({id, hc});
      if(max_lit_id < hc->getLocalMaxLitId())
        max_lit_id = hc->getLocalMaxLitId();
      curr_num_horn_clauses++;
      return;
    default: 
      throw "Problem @ HornClauses::add. The consequent of HornClause * hc is neither an equation nor a false constant";
  }
}

HornClauses::iterator HornClauses::begin(){
  return iterator(this->horn_clauses.begin());
}

HornClauses::iterator HornClauses::end() {
  return iterator(this->horn_clauses.end());
}

unsigned HornClauses::size() const { return horn_clauses.size(); }

HornClause* HornClauses::operator[](unsigned i) const { 
  auto it = horn_clauses.find(i);
  if(it != horn_clauses.end())
    return it->second;
  throw "Problem @ HornClauses::operator[]. Element not found.";
}

std::vector<HornClause *> const HornClauses::getHornClauses() const {
  std::vector<HornClause*> ans;
  ans.reserve(horn_clauses.size());
  for(auto key_value : horn_clauses)
    ans.push_back(key_value.second);
  return ans;
}

unsigned HornClauses::getUFESize() const {
  return ufe.getSize();
}

unsigned HornClauses::getMaxLitId() const {
  return max_lit_id;
}

std::ostream & operator << (std::ostream & os, const HornClauses & hcs){
  unsigned _i = 0;
  os << "Horn clauses produced" << std::endl;
  for(auto it : hcs.horn_clauses)
    os << _i++ << ". " << it.second << " " << *(it.second) << std::endl;
  os << "Number of horn clauses: " << _i;

  return os;
}
