#include "RenameWithUncomSymbols.h"

RenameWithUncomSymbols::RenameWithUncomSymbols(
    z3::expr_vector const & input_a, 
    std::set<std::string> const & uncommon_names) :
  Rename(input_a.ctx()), uncommon_names(uncommon_names)
{
  for(auto const & equation : input_a)
    renamed_input.push_back(reformulate(equation));
}

z3::expr RenameWithUncomSymbols::reformulate(z3::expr const & e){
  if(e.is_app()){
    auto f = e.decl();
    switch(f.decl_kind()){
      case Z3_OP_ANUM:
        return e;
      case Z3_OP_UMINUS:
        return f(reformulate(e.arg(0)));
      case Z3_OP_AND:
        {
          unsigned num = e.num_args();
          z3::expr_vector new_args(e.ctx());
          for(unsigned i = 0; i < num; i++)
            new_args.push_back(removePrefix(e.arg(i)));
          return z3::mk_and(new_args);
        }
      case Z3_OP_IMPLIES:
        {
          std::cout << "TODO" << std::endl;
          std::cout << e << std::endl;
          return e.arg(1);
        }
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
        return f(reformulate(e.arg(0)), reformulate(e.arg(1)));
      case Z3_OP_UNINTERPRETED:
        {
          unsigned num = e.num_args();
          z3::expr_vector new_args(e.ctx());
          z3::sort_vector domain_sorts(e.ctx());
          for(unsigned i = 0; i < num; i++){
            new_args.push_back(reformulate(e.arg(i)));
            domain_sorts.push_back(f.domain(i));
          }
          auto name = f.name().str();
          auto new_f = z3::function((
                (uncommon_names.find(name) != uncommon_names.end() ? "a_" : "c_") 
                + name).c_str(), domain_sorts, f.range());
          return new_f(new_args);
        }
      default:
        throw "Problem @ RenameWithUncomSymbols::reformulate: The formula e is not an QF_IUF.";
    }
  }
  throw "Problem @ RenameWithUncomSymbols::reformulate: The formula e is not an expression.";
}
