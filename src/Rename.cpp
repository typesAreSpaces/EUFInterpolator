#include "Rename.h"

Rename::Rename(z3::context & ctx) : 
  visited(), renamed_input(ctx)
{
}

z3::expr Rename::removePrefix(z3::expr const & e) const {
  if(e.is_app()){
    auto f = e.decl();
    switch(f.decl_kind()){
      case Z3_OP_TRUE:
      case Z3_OP_FALSE:
      case Z3_OP_ANUM:
        return e;
      case Z3_OP_NOT:
      case Z3_OP_UMINUS:
        return f(removePrefix(e.arg(0)));
      case Z3_OP_OR:
        {
          unsigned num = e.num_args();
          z3::expr_vector new_args(e.ctx());
          for(unsigned i = 0; i < num; i++)
            new_args.push_back(removePrefix(e.arg(i)));
          return z3::mk_or(new_args);
        }
      case Z3_OP_AND:
        {
          unsigned num = e.num_args();
          z3::expr_vector new_args(e.ctx());
          for(unsigned i = 0; i < num; i++)
            new_args.push_back(removePrefix(e.arg(i)));
          return z3::mk_and(new_args);
        }
      case Z3_OP_IMPLIES:
      case Z3_OP_EQ:
      case Z3_OP_DISTINCT:
      case Z3_OP_LE:
      case Z3_OP_GE:
      case Z3_OP_LT:
      case Z3_OP_GT:
      case Z3_OP_ADD:
      case Z3_OP_SUB:
      case Z3_OP_MUL:
      case Z3_OP_DIV:
      case Z3_OP_IDIV:
        return f(removePrefix(e.arg(0)), removePrefix(e.arg(1)));
      case Z3_OP_UNINTERPRETED:
        {
          unsigned num = e.num_args();
          z3::expr_vector new_args    (e.ctx());
          z3::sort_vector domain_sorts(e.ctx());
          for(unsigned i = 0; i < num; i++){
            new_args.push_back(removePrefix(e.arg(i)));
            domain_sorts.push_back(f.domain(i));
          }
          auto name = f.name().str();

          auto new_f = z3::function(
              name.substr(2, name.length()).c_str(), 
              domain_sorts, 
              f.range());
          return new_f(new_args);
        }
      default:
        throw "Problem @ Rename::reformulate: The formula e is not an QF_IUF.";
    }
  }
  throw "Problem @ Rename::reformulate: The formula e is not an expression.";
}

z3::expr_vector Rename::removePrefix(z3::expr_vector const & input) const {
  z3::expr_vector ans(input.ctx());
  for(auto const & entry : input)
    ans.push_back(removePrefix(entry));
  return ans;
}
