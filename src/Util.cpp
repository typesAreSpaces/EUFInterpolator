#include "Util.h"
#include <functional>

bool Util::compareEquation(const z3::expr & eq1, const z3::expr & eq2){
  switch(eq1.decl().decl_kind()){
    case Z3_OP_EQ:
      switch(eq2.decl().decl_kind()){
        case Z3_OP_EQ:
          return std::min(eq1.arg(0).id(), eq1.arg(1).id()) > std::min(eq2.arg(0).id(), eq2.arg(1).id()); // <--- Heuristic
        default:
          throw "Problem @ compareEquation. eq2 is not an equality";
      }
    default:
      throw "Problem @ compareEquation. eq1 is not an equality";
  }
}

// Read it as: compareTerm(t1, t2) iff t2 is 'better than' t1
bool Util::compareTerm(const z3::expr & t1, const z3::expr & t2){
  if (t1.is_common() != t2.is_common())
    return t1.is_common() < t2.is_common();
  else{
    unsigned arity1 = t1.num_args(), arity2 = t2.num_args();
    if(arity1 != arity2)
      // Because we prefer a term with fewer arity
      return arity1 > arity2;
    else
      // Because we prefer a term with smaller id
      return t1.id() > t2.id();
  }
}

std::hash<unsigned>    Util::unsigned_hasher;
std::hash<std::string> Util::string_hasher;
std::hash<CurryNode*>  Util::curry_hasher;
