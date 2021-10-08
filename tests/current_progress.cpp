#include <iostream>
#include <algorithm>
#include <z3++.h>
#include "Rename.h"
#include "EUFInterpolantWithUncomSymbols.h"

void paperExample(z3::context &);
void example(z3::context &);
void testDiseqFuncAppWithConst(z3::context &);
void testDiseqFuncAppNoConst(z3::context &);
void parametricExample(z3::context &);
void withImplications(z3::context &);
void kapurEmail_06_19_21(z3::context &);

int main(int argc, char ** argv){
  z3::context ctx;

  //paperExample(ctx);
  //example(ctx);
  //testDiseqFuncAppWithConst(ctx);
  //testDiseqFuncAppNoConst(ctx);
  //parametricExample(ctx);
  //withImplications(ctx);
  kapurEmail_06_19_21(ctx);

  return 0;
}

void paperExample(z3::context & ctx){
  z3::sort my_sort = ctx.uninterpreted_sort("A");
  z3::expr z1 = ctx.constant("z1", my_sort);
  z3::expr z2 = ctx.constant("z2", my_sort);
  z3::expr s1 = ctx.constant("s1", my_sort);
  z3::expr s2 = ctx.constant("s2", my_sort);

  z3::expr y1 = ctx.constant("y1", my_sort);
  z3::expr y2 = ctx.constant("y2", my_sort);
  z3::expr v =  ctx.constant("v",  my_sort);
  z3::expr t =  ctx.constant("t",  my_sort);
  z3::func_decl f = ctx.function("f", my_sort, my_sort, my_sort);
  std::set<std::string> uncomms({"v"});
  //std::set<std::string> uncomms({"v", "f", "y1", "y2"});
  //std::set<std::string> uncomms({"v", "z2", "s2"});

  z3::expr_vector input(ctx); 
  input.push_back(f(z1, v) == s1);
  input.push_back(f(z2, v) == s2);
  input.push_back(f(f(y1, v), f(y2, v)) == t);

  try {
    EUFInterpolantWithUncomSymbols eufi(input, uncomms);
    std::cout << eufi.removePrefix(eufi.getInterpolant()) << std::endl;
  }
  catch(char const * e){
    std::cout << e << std::endl;
  }
  return;
}

void example(z3::context & ctx){
  z3::sort my_sort = ctx.uninterpreted_sort("A");

  z3::expr a = ctx.constant("a", my_sort);
  z3::expr b = ctx.constant("b", my_sort);
  z3::expr c = ctx.constant("c", my_sort);

  z3::func_decl f = ctx.function("f", my_sort, my_sort);
  z3::func_decl g = ctx.function("g", my_sort, my_sort);
  z3::func_decl h = ctx.function("h", my_sort, my_sort, my_sort);

  std::set<std::string> uncomms({"f"});

  // A: f(a) = a, h(f(f(a)), f(a)) = h(c, c)
  // B: a = b, g(b) = b, h(g(a), g(g(b))) \neq h(c, c)

  z3::expr_vector input(ctx); 
  input.push_back(f(a) == a);
  input.push_back(h(f(f(a)), f(a)) == h(c, c));

  try {
    EUFInterpolantWithUncomSymbols eufi(input, uncomms);
    std::cout << eufi.removePrefix(eufi.getInterpolant()) << std::endl;
  }
  catch(char const * e){
    std::cout << e << std::endl;
  }
  std::cout << "Ok" << std::endl;
  return;
}

void testDiseqFuncAppWithConst(z3::context & ctx){
  z3::sort my_sort = ctx.uninterpreted_sort("A");

  z3::expr x1 = ctx.constant("x1", my_sort);
  z3::expr x2 = ctx.constant("x2", my_sort);
  z3::expr a1 = ctx.constant("a1", my_sort);
  z3::expr a2 = ctx.constant("a2", my_sort);
  z3::func_decl f = ctx.function("f", my_sort, my_sort);
  std::set<std::string> uncomms({"f", "a1", "a2"});

  z3::expr_vector input(ctx); 
  input.push_back(a1 != a2);
  input.push_back(f(x1) == a1);
  input.push_back(f(x2) == a2);

  try {
    EUFInterpolantWithUncomSymbols eufi(input, uncomms);
    std::cout << eufi.removePrefix(eufi.getInterpolant()) << std::endl;
  }
  catch(char const * e){
    std::cout << e << std::endl;
  }
  std::cout << "Ok" << std::endl;
  return;
}

void testDiseqFuncAppNoConst(z3::context & ctx){
  z3::sort my_sort = ctx.uninterpreted_sort("A");

  z3::expr x1 = ctx.constant("x1", my_sort);
  z3::expr x2 = ctx.constant("x2", my_sort);
  z3::func_decl f = ctx.function("f", my_sort, my_sort);
  std::set<std::string> uncomms({"f"});

  z3::expr_vector input(ctx); 
  input.push_back(f(x1) != f(x2));

  try {
    EUFInterpolantWithUncomSymbols eufi(input, uncomms);
    std::cout << eufi.removePrefix(eufi.getInterpolant()) << std::endl;
  }
  catch(char const * e){
    std::cout << e << std::endl;
  }
  std::cout << "Ok" << std::endl;
  return;
}

void parametricExample(z3::context & ctx){
  z3::sort my_sort = ctx.uninterpreted_sort("A");
  z3::expr x = ctx.constant("x", my_sort);
  z3::expr a = ctx.constant("a", my_sort);
  z3::func_decl f = ctx.function("f", my_sort, my_sort);
  std::set<std::string> uncomms({"f"});

  z3::expr_vector input(ctx); 
  input.push_back((f(f(x))) == x);
  input.push_back(
      f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(x))))))))))))))))
      ==
      f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(f(x)))))))))))))))))
      );
  input.push_back(f(a) != a);

  try {
    EUFInterpolantWithUncomSymbols eufi(input, uncomms);
    std::cout << eufi.removePrefix(eufi.getInterpolant()) << std::endl;
    std::cout << "Nice" << std::endl;
  }
  catch(char const * e){
    std::cout << e << std::endl;
  }

  return;
}

void withImplications(z3::context & ctx){
  z3::sort my_sort = ctx.uninterpreted_sort("A");
  z3::expr x = ctx.constant("x", my_sort);
  z3::expr a = ctx.constant("a", my_sort);
  z3::func_decl f = ctx.function("f", my_sort, my_sort);
  std::set<std::string> uncomms({"f"});

  z3::expr_vector input(ctx);
  input.push_back(z3::implies(x == f(x), f(f(x)) == x));

  try {
    EUFInterpolantWithUncomSymbols eufi(input, uncomms);
    std::cout << eufi.removePrefix(eufi.getInterpolant()) << std::endl;
    std::cout << "Nice with Implications" << std::endl;
  }
  catch(char const * e){
    std::cout << e << std::endl;
  }

  return;
}

void  kapurEmail_06_19_21(z3::context & ctx){
  z3::sort my_sort = ctx.uninterpreted_sort("A");

  z3::expr s1 = ctx.constant("s1", my_sort);
  z3::expr s2 = ctx.constant("s2", my_sort);

  z3::func_decl f = ctx.function("f", my_sort, my_sort, my_sort);

  z3::expr z1 = ctx.constant("z1", my_sort);
  z3::expr z2 = ctx.constant("z2", my_sort);
  z3::expr z3 = ctx.constant("z3", my_sort);
  z3::expr z4 = ctx.constant("z4", my_sort);

  z3::expr a = ctx.constant("a", my_sort);
  z3::expr b = ctx.constant("b", my_sort);

  z3::expr e = ctx.constant("e", my_sort);
  z3::expr e1 = ctx.constant("e1", my_sort);
  z3::expr e2 = ctx.constant("e2", my_sort);

  z3::expr t = ctx.constant("t", my_sort);
  z3::expr v1 = ctx.constant("v1", my_sort);
  z3::expr v2 = ctx.constant("v2", my_sort);

  std::set<std::string> uncomms({"e", "e1", "e2"});

  z3::expr_vector input(ctx); 
  input.push_back(s1 == f(z3, e));
  input.push_back(s2 == f(z4, e));
  input.push_back(e1 == f(z1, e));
  input.push_back(e2 == f(z2, e));
  input.push_back(t == f(e1, e2));
  input.push_back(v1 == f(e1, a));
  input.push_back(v2 == f(e2, b));

  try {
    EUFInterpolantWithUncomSymbols eufi(input, uncomms);
    std::cout << eufi.removePrefix(eufi.getInterpolant()) << std::endl;
  }
  catch(char const * e){
    std::cout << e << std::endl;
  }

  return;
}
