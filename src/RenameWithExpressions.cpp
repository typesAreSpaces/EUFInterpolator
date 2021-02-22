#include "Rename.h"

RenameWithExpressions::RenameWithExpressions(
    z3::expr_vector const & input_a, 
    z3::expr_vector const & input_b) : 
  Rename(input_a.ctx()), a_local_names(), common_names()
{
  for(auto const & equation : input_a)
    traversePartA(equation);
  for(auto const & equation : input_b)
    traversePartB(equation);
  for(auto const & equation : input_a)
    renamed_input.push_back(reformulate(equation));
}

void RenameWithExpressions::traversePartA(z3::expr const & e){
  if(visited.size() <= e.id())
    visited.resize(e.id()+1, false);
  if(visited[e.id()])
    return;

  if(e.is_app()){
    unsigned num = e.num_args();
    auto f = e.decl();
    switch(f.decl_kind()){
      case Z3_OP_ANUM:
        return;
      case Z3_OP_UMINUS:
        traversePartA(e.arg(0));
        return;
      case Z3_OP_AND:
        for(unsigned i = 0; i < num; i++)
          traversePartA(e.arg(i));
        return;
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
        traversePartA(e.arg(0));
        traversePartA(e.arg(1));
        return;
      case Z3_OP_UNINTERPRETED:
        for (unsigned i = 0; i < num; i++)
          traversePartA(e.arg(i));
        a_local_names.insert(e.decl().name().str());
        return;
      default:
        throw "Problem @ traversePartA: The formula e is not an QF_IUF.";
    }
  }
  throw "Problem @ traversePartA: The formula e is not an expression.";
}

void RenameWithExpressions::traversePartB(z3::expr const & e){
  if(visited.size() <= e.id())
    visited.resize(e.id()+1, false);
  if(visited[e.id()])
    return;

  if(e.is_app()){
    unsigned num = e.num_args();
    auto f = e.decl();
    switch(f.decl_kind()){
      case Z3_OP_ANUM:
        return;
      case Z3_OP_UMINUS:
        traversePartB(e.arg(0));
        return;
      case Z3_OP_AND:
        for(unsigned i = 0; i < num; i++)
          traversePartB(e.arg(i));
        return;
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
        traversePartB(e.arg(0));
        traversePartB(e.arg(1));
        return;
      case Z3_OP_UNINTERPRETED:
        {
          for (unsigned i = 0; i < num; i++)
            traversePartB(e.arg(i));
          auto name = e.decl().name().str();
          if(a_local_names.find(name) != a_local_names.end())
            common_names.insert(name);
          return;
        }
      default:
        throw "Problem @ traversePartB: The formula e is not an QF_UIF.";
    }
  }
  throw "Problem @ traversePartA: The formula e is not an expression.";
}

z3::expr RenameWithExpressions::reformulate(z3::expr const & e){
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
                (common_names.find(name) != common_names.end() ?
                 "c_" 
                 : (a_local_names.find(name) != a_local_names.end() ? 
                   "a_" 
                   : "b_"))
                + name).c_str(), domain_sorts, f.range());
          return new_f(new_args);
        }
      default:
        throw "Problem @ reformulate: The formula e is not an QF_IUF.";
    }
  }
  throw "Problem @ reformulate: The formula e is not an expression.";
}
