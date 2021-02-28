#include "EUFInterpolant.h"

EUFInterpolant::EUFInterpolant(z3::expr_vector const & assertions) : 
  // Congruence Closure step
  Input(assertions), 
  assertions((
        // Unconditional uncommon symbol elimination step
        exposeUncommons(), 
        assertions)), 
  // Conditional uncommon symbol elimination step
  hsat(cce, horn_clauses), result(ctx),
  conditional_horn_clauses(ctx, ufe, original_num_terms)
{        
  //for(auto const & equation : assertions){
    //if(!equation.is_eq()) continue;
    //hsat.equiv_classes.merge(equation.arg(0), equation.arg(1));
  //}

#if DEBUG_EXPOSE_UNCOMMS
  std::cout << "After expose uncommons" << std::endl;
  std::cout << horn_clauses << std::endl;
#endif

#if DEBUG_HSAT_INTER
  std::cout << "After conditional elimination" << std::endl;
  std::cout << hsat << std::endl;
#endif

#if DEBUG_TEMP
  std::cout << "All the subterms" << std::endl;
  std::cout << subterms << std::endl;
#endif

  conditionalElimination();
  buildInterpolant();
  return;
}

EUFInterpolant::~EUFInterpolant(){
#if DEBUG_DESTRUCTOR_EUF
  std::cout << "Bye EUFInterpolant" << std::endl;
#endif
}

z3::expr_vector EUFInterpolant::buildHCBody(z3::expr const & t1, z3::expr const & t2){
  z3::expr_vector hc_body(ctx);
  unsigned num_args = t1.num_args();
  for(unsigned i = 0; i < num_args; i++)
    hc_body.push_back(z3_repr(t1.arg(i)) == z3_repr(t2.arg(i)));
  return hc_body;
}

void EUFInterpolant::exposeUncommons(){
  for(auto const & iterator : fsym_positions){
#if DEBUG_EXPOSE_UNCOMMS
    std::cout << "Debugging exposeUncommons" << std::endl;
    std::cout << iterator.first << std::endl;
    for(auto const & entry : iterator.second)
      std::cout << subterms[entry] << std::endl;
#endif

    unsigned current_num = iterator.second.size();
    if(current_num >= 2)
      for(unsigned index_1 = 0; index_1 < current_num - 1; index_1++)
        for(unsigned index_2 = index_1 + 1; index_2 < current_num; index_2++){
          z3::expr const 
            & t1 = subterms[iterator.second[index_1]], 
            & t2 = subterms[iterator.second[index_2]];

          // // Only expose terms if at least one term is uncommon
          // //if(!t1.is_common() || !t2.is_common()){
          // //}
          // If the latter is true, some nontrivial
          // Horn clauses are not produced.
          z3::expr_vector hc_body = buildHCBody(t1, t2);
          z3::expr        hc_head = z3_repr(t1) == z3_repr(t2);
          horn_clauses.add(new HornClause(ctx, hc_body, hc_head, ufe));
        }
  }
  return;
}

void EUFInterpolant::conditionalElimination(){
  // Approach: add stuff to the Horn Clauses
  // in the input using add by eliminating
  // uncommon term using the explanation 
  // operator

#if DEBUG_COND_ELIM
  std::cout << "Before conditionalEliminationEqs" << std::endl;
  std::cout << horn_clauses << std::endl;
#endif

  // Process original equations
  conditionalEliminationEqs();

#if DEBUG_COND_ELIM
  std::cout << "After conditionalEliminationEqs/" 
    "Before conditionalEliminationHcs" << std::endl;
  std::cout << horn_clauses << std::endl;
#endif

  // Process Horn clauses produced
  conditionalEliminationHcs();

#if DEBUG_COND_ELIM
  std::cout << "After conditionalEliminationHcs" << std::endl;
  std::cout << horn_clauses << std::endl;
#endif

  return;
}

z3::expr_vector EUFInterpolant::explainUncommons(z3::expr const & t1, z3::expr const & t2){
#if DEBUG_COND_ELIM
  std::cout << "Inside explainUncommons" << std::endl;
  std::cout << "Inputs: " << std::endl << t1 << std::endl << t2 << std::endl;
#endif
  z3::expr_vector ans(t1.ctx());
  if(t1.id() == t2.id())
    return ans;
  auto partial_explain = hsat.equiv_classes.z3Explain(t1, t2);
#if DEBUG_COND_ELIM
  std::cout << "Explanation " << partial_explain << std::endl;
#endif
  for(auto const & equation : partial_explain){
#if DEBUG_COND_ELIM
  std::cout << equation << std::endl;
#endif
    if(equation.is_common())
      ans.push_back(equation);
    else{
      // --------------------------------
      // Look at the horn_clauses in hsat
      // Identify the proper head term
      // append its antecedent
      // --------------------------------
      auto const & entry = hsat.head_term_indexer.find(equation.id());
      ASSERT(entry != hsat.head_term_indexer.end(), 
          "At this point the system should able to find some "
          "Horn clause with "
          "quote(equation) as head equation");
      for(auto const & antecedent_equation : entry->second->getAntecedent()){
        if(antecedent_equation.is_common())
          ans.push_back(antecedent_equation);
        else{
#if DEBUG_COND_ELIM
          std::cout << "Pushing extra common equations" << std::endl;
#endif
          for(auto extra_common_equation : explainUncommons(antecedent_equation.arg(0), antecedent_equation.arg(1)))
            ans.push_back(extra_common_equation);
        }
      }
    }

  }
  return ans;
}

std::list<z3::expr> EUFInterpolant::candidates(z3::expr const & e){
  std::list<z3::expr> ans({});

  if(e.is_common()){
    ans.push_back(e);
    return ans;
  }

  std::set<UnionFind::EqClass> ids({});
  UnionFind::EqClass repr_index = hsat.equiv_classes.find(e.id());
  auto it = hsat.ufe.begin(repr_index);
  auto end = hsat.ufe.end(repr_index);
  for(; it != end; ++it){
    Z3Index index = factory_curry_nodes.getCurryNode(*it)->getZ3Id();
    z3::expr term = subterms[index];
    if(term.is_common() && ids.find(index) == ids.end()){
      ids.insert(index);
      ans.push_back(term);
    }
  }

  return ans;
}

std::list<std::list<z3::expr> > EUFInterpolant::allCandidates(z3::expr const & t){
  assert(t.is_app() && !t.is_const());

  std::list<std::list<z3::expr> > ans({});
  // The following test if the function symbol
  // is not common
  auto f = t.decl().name().str();
  if(f[0] != 'c'){
    ans.push_back(std::list<z3::expr>({}));
    return ans;
  }

  unsigned num_args = t.num_args();
  for(unsigned index = 0; index < num_args; index++)
    ans.push_back(candidates(t.arg(index)));

  return ans;
}

std::vector<z3::expr_vector> EUFInterpolant::cartesianProd(std::list<std::list<z3::expr> > candidates){
  // ans_size can get really large
  // at most O(n^n)
  // so be aware of 
  // overflow problems
  unsigned ans_size = 1;
  for(auto const & entry : candidates)
    ans_size *= entry.size();

  std::vector<z3::expr_vector> ans;
  ans.reserve(ans_size);
  for(unsigned i = 0; i < ans_size; ++i)
    ans.push_back(z3::expr_vector(ctx));

  if(ans_size){
    unsigned max_repetition = 1;
    for(auto const & entry : candidates){
      unsigned index = 0;
      while(index < ans_size)
        for(auto const & value : entry){
          unsigned index_repetition = 0;
          while(index_repetition++ < max_repetition)
            ans[index++].push_back(value);
        }
      max_repetition *= entry.size();
    }
  }

  return ans;
}

void EUFInterpolant::buildInterpolant(){

  horn_clauses.filterCommons();
  horn_clauses.simplify();
  conditional_horn_clauses.filterCommons();
  conditional_horn_clauses.simplify();

#if DEBUG_BUILD_INTERP
  std::cout << horn_clauses << std::endl;
  std::cout << conditional_horn_clauses << std::endl;
#endif

  for(auto const & element : horn_clauses)
    if(element->isLeader())
      //result.push_back(element->ToZ3Expr());
      result.push_back(element->ToZ3Expr().simplify());
  for(auto const & element : conditional_horn_clauses)
    if(element->isLeader())
      //result.push_back(element->ToZ3Expr());
      result.push_back(element->ToZ3Expr().simplify());

  return;
}

z3::expr_vector EUFInterpolant::getInterpolant() const {
  return result;
}

std::ostream & operator << (std::ostream & os, EUFInterpolant & euf){
  return os;
}

