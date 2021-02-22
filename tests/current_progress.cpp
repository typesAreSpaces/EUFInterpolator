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

int main(int argc, char ** argv){
  z3::context ctx;

  //paperExample(ctx);
  //example(ctx);
  //testDiseqFuncAppWithConst(ctx);
  //testDiseqFuncAppNoConst(ctx);
  parametricExample(ctx);

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
