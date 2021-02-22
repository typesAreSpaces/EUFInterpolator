#include <iostream>
#include "TestCongruenceClosureExplain.h"
#include <set>
#include "Rename.h"

void inputFile(char const * file_name){
  std::cout << "Processing file: " << file_name << std::endl;
  z3::context ctx;
  auto assertions = ctx.parse_file(file_name);
  std::string input_file = file_name;
  TestCongruenceClosureExplain test(assertions);
  try{
    std::cout << test.testConsistency(assertions, 20) << std::endl;
    //test.testExplanation(1);
  }
  catch(char const * e){
    std::cout << e << std::endl;
  }
  return;
}

void testAdditionalMerge(){
  z3::context ctx;
  z3::sort my_sort = ctx.uninterpreted_sort("A");
  z3::expr a = ctx.constant("a", my_sort);
  z3::expr b = ctx.constant("b", my_sort);
  z3::expr c = ctx.constant("c", my_sort);
  z3::expr d = ctx.constant("d", my_sort);
  z3::func_decl f = ctx.function("f", my_sort, my_sort);

  z3::expr_vector input(ctx);
  //input.push_back(a == b);
  input.push_back(f(a) == c);
  input.push_back(f(b) == d);

  TestCongruenceClosureExplain test(input);
  std::cout << "Before" << std::endl;
  std::cout << test << std::endl;
  test.merge(a, b);
  std::cout << "After" << std::endl;
  std::cout << test << std::endl;

  return;
}

void testCongClosureExpl(){
  z3::context ctx;
  z3::sort my_sort = ctx.uninterpreted_sort("A");
  z3::expr a = ctx.constant("a", my_sort);
  z3::expr b = ctx.constant("b", my_sort);
  z3::expr c = ctx.constant("c", my_sort);
  z3::expr d = ctx.constant("d", my_sort);
  z3::func_decl f = ctx.function("f", my_sort, my_sort);

  z3::expr_vector input(ctx);
  input.push_back(a == b);
  input.push_back(f(a) == c);
  input.push_back(f(b) == d);

  TestCongruenceClosureExplain test(input);
  std::cout << test << std::endl;
  std::cout << test.testConsistency(input, 20) << std::endl;
  test.testExplanation(20);
  return;
}

void testCongClosureExpl2(){
  z3::context ctx;
  z3::sort my_sort = ctx.uninterpreted_sort("A");
  z3::expr a = ctx.constant("a", my_sort);
  z3::expr b = ctx.constant("b", my_sort);
  z3::func_decl g = ctx.function("g", my_sort, my_sort, my_sort, my_sort);
  z3::func_decl h = ctx.function("h", my_sort, my_sort);

  z3::expr_vector input(ctx);
  input.push_back(g(a, h(b), b) == b );
  input.push_back(g(a, h(b), h(b)) == h(b) );
  input.push_back(g(a, h(b), h(h(b))) == h(h(b)));

  TestCongruenceClosureExplain test(input);
  std::cout << test << std::endl;
  std::cout << test.testConsistency(input, 20) << std::endl;
  test.testExplanation(20);
  return;
}

void testCongClosureExpl3(){
  z3::context ctx;
  z3::sort my_sort = ctx.uninterpreted_sort("A");
  z3::expr a = ctx.constant("a", my_sort);
  z3::expr b = ctx.constant("b", my_sort);
  z3::func_decl g = ctx.function("g", my_sort, my_sort, my_sort, my_sort);
  z3::func_decl h = ctx.function("h", my_sort, my_sort);

  z3::expr_vector input(ctx);
  input.push_back(g(a, h(b), b) == b );
  input.push_back(g(a, h(b), h(b)) == h(b) );
  input.push_back(g(a, h(b), h(h(b))) == h(h(b)) );
  input.push_back(h(b) == b);

  TestCongruenceClosureExplain test(input);
  std::cout << test << std::endl;
  std::cout << test.testConsistency(input, 20) << std::endl;
  test.testExplanation(20);
  return;
}

void testCongClosureExpl4(){
  z3::context ctx;
  z3::sort my_sort = ctx.uninterpreted_sort("A");
  z3::expr a = ctx.constant("a", my_sort);
  z3::expr b = ctx.constant("b", my_sort);
  z3::func_decl g = ctx.function("g", my_sort, my_sort, my_sort, my_sort);
  z3::func_decl h = ctx.function("h", my_sort, my_sort);

  z3::expr_vector input(ctx);
  input.push_back(g(a, h(b), b) == g(b, h(b), h(b)) );
  input.push_back(h(b) == b );
  input.push_back(a == b);

  TestCongruenceClosureExplain test(input);
  std::cout << test << std::endl;
  std::cout << test.testConsistency(input, 20) << std::endl;
  test.testExplanation(20);
  return;
}

void testCongClosureExpl5(){
  z3::context ctx;
  z3::sort my_sort = ctx.uninterpreted_sort("A");
  z3::expr a = ctx.constant("a", my_sort);
  z3::expr b = ctx.constant("b", my_sort);
  z3::expr c = ctx.constant("c", my_sort);
  z3::expr d = ctx.constant("d", my_sort);
  z3::expr e = ctx.constant("e", my_sort);
  z3::expr g = ctx.constant("g", my_sort);
  z3::expr h = ctx.constant("h", my_sort);
  z3::func_decl f = ctx.function("f", my_sort, my_sort, my_sort);

  z3::expr_vector input(ctx);
  input.push_back(f(g, h) == d );
  input.push_back(c == d );
  input.push_back(f(g, d) == a );
  input.push_back(e == c );
  input.push_back(e == b );
  input.push_back(b == h);
 
  TestCongruenceClosureExplain test(input);
  std::cout << test << std::endl;
  std::cout << test.testConsistency(input, 20) << std::endl;
  test.testExplanation(20);
  return;
}

void paperExample(){

  z3::context ctx;
  z3::sort my_sort = ctx.uninterpreted_sort("A");
  z3::expr z1 = ctx.constant("z1", my_sort);
  z3::expr z2 = ctx.constant("z2", my_sort);
  z3::expr s1 = ctx.constant("s1", my_sort);
  z3::expr s2 = ctx.constant("s2", my_sort);
  z3::expr y1 = ctx.constant("y1", my_sort);
  z3::expr y2 = ctx.constant("y2", my_sort);
  z3::expr v = ctx.constant("v", my_sort);
  z3::expr t = ctx.constant("t", my_sort);
  z3::func_decl f = ctx.function("f", my_sort, my_sort, my_sort);
  std::set<std::string> uncomms({"v"});

  z3::expr_vector input(ctx); 
  input.push_back(f(z1, v) == s1);
  input.push_back(f(z2, v) == s2);
  input.push_back(f(f(y1, v), f(y2, v)) == t);
  RenameWithUncomSymbols rename(input, uncomms);

  try {
    TestCongruenceClosureExplain eufi(rename.renamed_input);
  }
  catch(char const * e){
    std::cout << e << std::endl;
  }

  return;
}


int main(int argc, char ** argv){

  paperExample();
  
  testAdditionalMerge();
  
  testCongClosureExpl();
  testCongClosureExpl2();
  testCongClosureExpl3();
  testCongClosureExpl4();
  testCongClosureExpl5();
  
  try {
    inputFile("/home/jose/Documents/GithubProjects/master-thesis/Software/Cpp/EUFInterpolant/tests/QF_UF/SEQ/SEQ013_size6.smt2");
    inputFile("/home/jose/Documents/GithubProjects/master-thesis/Software/Cpp/EUFInterpolant/tests/QF_UF/2018-Goel-hwbench/QF_UF_v_Unidec_ab_cti_max.smt2");
    inputFile("/home/jose/Documents/GithubProjects/master-thesis/Software/Cpp/EUFInterpolant/tests/QF_UF/SEQ/SEQ018_size7.smt2");
    inputFile("/home/jose/Documents/GithubProjects/master-thesis/Software/Cpp/EUFInterpolant/tests/QF_UF/SEQ/SEQ017_size6.smt2");
    inputFile("/home/jose/Documents/GithubProjects/master-thesis/Software/Cpp/EUFInterpolant/tests/QF_UF/SEQ/SEQ018_size8.smt2");
    inputFile("/home/jose/Documents/GithubProjects/master-thesis/Software/Cpp/EUFInterpolant/tests/QF_UF/SEQ/SEQ019_size6.smt2");
    inputFile("/home/jose/Documents/GithubProjects/master-thesis/Software/Cpp/EUFInterpolant/tests/QF_UF/PEQ/PEQ004_size9.smt2");
  }
  catch(char const * e){
    std::cout << e << std::endl;
  }
 
  return 0;
}


