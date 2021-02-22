#include "Input.h"

Input::Input(z3::expr_vector const & assertions) :
  original_num_terms(maxIdFromAssertions(assertions) + 1),
  ctx(assertions.ctx()), subterms(ctx), contradiction(ctx.bool_val(false)), disequalities(ctx),
  fsym_positions(), ufe(original_num_terms), horn_clauses(ctx, ufe, original_num_terms),
  ids_to_merge(),
  curry_decl(), factory_curry_nodes(original_num_terms, curry_decl),
  cce(
      (
       subterms.resize(original_num_terms), 
       // The following defines: 
       // subterms, ids_to_merge, disequalities, 
       // fsym_positions, and curry_decl
       init(assertions), 
       subterms), ufe, factory_curry_nodes, ids_to_merge
      )
{
  disequalitiesToHCS();
}

Input::~Input(){
#if DEBUG_DESTRUCTOR_EUF
  std::cout << "Bye Input" << std::endl;
#endif
}

unsigned Input::maxIdFromAssertions(z3::expr_vector const & assertions){
  unsigned max_id_from_assertions = 0;
  for(auto const & assertion : assertions)
    if(assertion.id() > max_id_from_assertions)
      max_id_from_assertions = assertion.id();
  
  return max_id_from_assertions;
}

void Input::init(z3::expr_vector const & assertions){
  for(auto const & assertion : assertions){
    initFormula(assertion);
    switch(assertion.decl().decl_kind()){
      case Z3_OP_EQ:
        ids_to_merge.emplace_back(assertion.arg(0).id(), assertion.arg(1).id());
        break;
      case Z3_OP_DISTINCT:
        disequalities.push_back(assertion);
        break;
      default:
        break;
    }
  }
}

void Input::initFormula(z3::expr const & e){
  if(e.is_app()){
    if(subterms.visited[e.id()])
      return;

    subterms.set(e.id(), e);

    unsigned num = e.num_args();
    for(unsigned i = 0; i < num; i++)
      initFormula(e.arg(i));

    z3::func_decl f = e.decl();
    if(curry_decl[f.id()] == nullptr)
      curry_decl[f.id()] = factory_curry_nodes.queryCurryNode(e.id(), f.name().str(), nullptr, nullptr);

    switch(f.decl_kind()){
      case Z3_OP_UNINTERPRETED:
        if(num > 0)
          fsym_positions[f.name().str()].push_back(e.id());
      default:
        break;
    }
    return;
  }
  throw "Problem @ Input::init. The expression e is not an application term.";
}

void Input::disequalitiesToHCS(){
  unsigned num_disequalities = disequalities.size();
  for(unsigned i = 0; i < num_disequalities; i++){
    z3::expr_vector hc_body(ctx);
    // It is really important to use the representative
    // which favors being a common term
    hc_body.push_back(z3_repr(disequalities[i].arg(0)) == z3_repr(disequalities[i].arg(1)));
    horn_clauses.add(new HornClause(ctx, hc_body, contradiction, ufe));
  }
}

z3::expr Input::z3_repr(z3::expr const & e){
  return cce.z3Repr(e);
}

std::ostream & operator << (std::ostream & os, Input const & input){
  return os;
}
