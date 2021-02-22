#include "Hornsat.h"
#include <z3++.h>

unsigned Literal::curr_num_literals = 0;

Hornsat::Hornsat(CongruenceClosureExplain & cce, 
    HornClauses const & hcs) :
  num_hcs(hcs.size()), num_literals(hcs.getMaxLitId()), head_term_indexer(),
  /*This ufe is empty*/ 
  ufe(hcs.getUFESize()), equiv_classes(this, cce, ufe),
  list_of_literals(), class_list(), num_args(), pos_lit_list(), 
  facts(), to_combine(),
  consistent(true)
{
  Literal::curr_num_literals = 0;

  list_of_literals.resize(num_literals + 1);
  class_list      .resize(num_literals + 1);
  num_args        .resize(num_hcs);
  pos_lit_list    .resize(num_hcs);

  build(equiv_classes, hcs);
}

void Hornsat::build(CongruenceClosureExplain & cce, HornClauses const & hcs){
#if DEBUGGING_CONSTRUCTOR
  std::cout << "Horn Clauses processed by Hornsat" << std::endl;
#endif

  ClauseId index_hc = 0;
  for(auto horn_clause : hcs.getHornClauses()){
#if DEBUGGING_CONSTRUCTOR
    std::cout << "Processing this Horn clause:" << std::endl;
    std::cout << index_hc << " " << *horn_clause << std::endl;
    std::cout << "Num antecedent: " << horn_clause->numAntecedent() << std::endl;
#endif
    // -----------------------------------------------------
    // The following line defines our approach -------------
    // towards conditional elimination
    //num_args[index_hc] = horn_clause->numUncommAntecedent();
    num_args[index_hc] = horn_clause->numAntecedent();
    // -----------------------------------------------------

    // ------------------------------------------------------
    // Horn clause body processing --------------------------
    // Remark: We only have equations in the antecedent
    for(auto antecedent : horn_clause->getAntecedent()){
#if DEBUGGING_CONSTRUCTOR
      std::cout << "Literals inside antedecent " 
        << antecedent.id() << " " 
        << antecedent 
        << (antecedent.is_common() ? " This one is common! " : " Not common " )
        << std::endl;
#endif
      Literal * literal = &list_of_literals[antecedent.id()];
      literal->update(antecedent, cce, index_hc);
      class_list[literal->l_id].emplace_back(literal, LHS);
      class_list[literal->r_id].emplace_back(literal, RHS);

      // If literal is common insert it as a fact
      if(antecedent.is_common()){
        literal->val = true;
        facts.push(antecedent.id());
        to_combine.push(TermIdPair(
              antecedent.arg(0).id(), 
              antecedent.arg(1).id()));
      }
    }
    // ------------------------------------------------------

    // ------------------------------------------------------------
    // Horn clause head processing --------------------------------
    auto consequent = horn_clause->getConsequent();
    // This structure is only used in our approach
    // for conditional-elimination
    head_term_indexer[consequent.id()] = horn_clause;
#if DEBUGGING_CONSTRUCTOR
    std::cout << "Consequent Literal " 
      << consequent.id() << " " << consequent << std::endl;
#endif
    Literal * literal = 
      &list_of_literals[consequent.decl().name().str() == "false" ?
      FALSELITERAL : consequent.id()];

    pos_lit_list[index_hc] = literal->literal_id;
    if(literal->literal_id > FALSELITERAL){
      literal->update(consequent, cce);
      class_list[literal->l_id].emplace_back(literal, LHS);
      class_list[literal->r_id].emplace_back(literal, RHS);
    }
    // In the original formulation by Gallier, 
    // this checks if the Horn Clause is a fact,
    // in this approach, this checks if a Horn Clause
    // can be spreading because the its antecedent is
    // common.
    if(num_args[index_hc] == 0){
      if(literal->literal_id == FALSELITERAL)
        consistent = false;
      else{
#if DEBUGGING_CONSTRUCTOR
        std::cout << "Pushing " << consequent << " into the (conditional) equiv_classes" << std::endl;
#endif
        literal->val = true;
        facts.push(consequent.id());
        to_combine.push(TermIdPair(
              consequent.arg(0).id(), 
              consequent.arg(1).id()));
      }
    }
    // ------------------------------------------------------------

    index_hc++;
  }
#if DEBUGGING_CONSTRUCTOR
  std::cout << "Done @ Hornsat constructor" << std::endl;
#endif
  satisfiable();
}

Hornsat::~Hornsat(){
  for(auto literal : list_of_literals){
    for(auto it = literal.clause_list->begin(); 
        it != literal.clause_list->end(); ){
#if DEBUG_DESTRUCTORS
      std::cout << "Deleting this clause: " << (*it)->clause_id << std::endl;
#endif
      auto first_move_then_delete = it;
      ++it;
      delete *first_move_then_delete;
    }
  }
#if DEBUG_DESTRUCTORS
  std::cout << "Done ~Hornsat" << std::endl;
#endif
}

void Hornsat::satisfiable(){
  LiteralId node;
  //while(!facts.empty() && consistent){
  while(!facts.empty()){
    node = facts.front();

    facts.pop();
    for(auto it : *(list_of_literals[node].clause_list)){
      ClauseId clause1 = it->clause_id;
      --num_args[clause1];
#if DEBUGGING_SATISFIABLE
      ASSERT(num_args[clause1] >= 0, 
          "num_args become negative");
#endif
      // -----------------------------------------------
      if(num_args[clause1] == 0){
        LiteralId nextnode = pos_lit_list[clause1];
        if(!list_of_literals[nextnode].val){
          if(nextnode){
            facts.push(nextnode);
            list_of_literals[nextnode].val = true;
            TermId u = list_of_literals[nextnode].l_id, 
                   v = list_of_literals[nextnode].r_id;
            if(!equiv_classes.areSameClass(u, v))
              to_combine.push(TermIdPair(u, v));
          }
          else
            consistent = false;
        } 
      }
    }
    //if(facts.empty() && consistent)
    if(facts.empty())
      closure();
  }
}

void Hornsat::closure(){ 
  while(!to_combine.empty()){
    auto const & pair = to_combine.front();
    TermId u = pair.lhs, v = pair.rhs;
    equiv_classes.merge(u, v); 
    to_combine.pop();
  }
}

bool Hornsat::isConsistent() const {
  return consistent;
}

void Hornsat::unionupdate(TermId u, EqClass alpha){ 
  for(auto p : class_list[u]){
#if DEBUGGING_UNIONUPDATE
    std::cout << "Before, Term: " << u << " " << p << std::endl;
#endif
    if(!p.lit_pointer->val){
      switch(p.eq_side){
        case LHS:
          p.lit_pointer->l_class = alpha;
          break;
        case RHS:
          p.lit_pointer->r_class = alpha;
          break;
      }
      if(p.lit_pointer->l_class == p.lit_pointer->r_class){
        facts.push(p.lit_pointer->literal_id);
        p.lit_pointer->val= true;
      }
    }
#if DEBUGGING_UNIONUPDATE
    std::cout << "After,  Term: " << u << " " << p << std::endl;
#endif
  }
  // There is no need to make a union call
  // since this function will only be called inside 
  // propagateAux of equiv_classes
}

std::ostream & operator << (std::ostream & os, const Hornsat & hsat){
  for(auto x : hsat.list_of_literals)
    os << "Literal " << x.literal_id
      << " LClass: " << x.l_class
      << " RClass: " << x.r_class
      << " Assigment: " << x.val 
      << " Common?: " << x.is_common
      << std::endl;

  os << "Consistent: " << (hsat.consistent ? "Yes" : "No");
  return os;
}
