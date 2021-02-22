#include <iostream>
#include "TestCongruenceClosureExplain.h"

int main(int argc, char ** argv){

  if(argc == 2){
    z3::context ctx;
    auto assertions = ctx.parse_file(argv[1]);
    std::string input_file = argv[1];
    TestCongruenceClosureExplain test(assertions);
    try {
      test.testConsistency(assertions, 1);
    }
    catch(char const * e){
      std::cout << e << std::endl;
    }
    return 0;
  }
  return 1;
}
