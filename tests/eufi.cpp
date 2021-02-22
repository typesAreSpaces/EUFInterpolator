#include <iostream>
#include <algorithm>
#include <z3++.h>
#include "Rename.h"
#include "EUFInterpolantWithExpressions.h"

int main(int argc, char ** argv){

  switch(argc){
    case 2: 
      {
        z3::context ctx;
        z3::solver parser(ctx);
        parser.from_file(argv[1]);

        auto const & assertions = parser.assertions();
        assert(assertions.size() == 2);

        auto const & part_a = assertions[0];
        auto const & part_b = assertions[1];
        assert(part_a.decl().decl_kind() == Z3_OP_AND
            && part_b.decl().decl_kind() == Z3_OP_AND);

        z3::expr_vector part_a_vec(ctx);
        z3::expr_vector part_b_vec(ctx);
        for(unsigned i = 0; i < part_a.num_args(); i++)
          part_a_vec.push_back(part_a.arg(i));
        for(unsigned i = 0; i < part_b.num_args(); i++)
          part_b_vec.push_back(part_b.arg(i));
        
        try {
          EUFInterpolantWithExpressions test(part_a_vec, part_b_vec);
          std::cout << test << std::endl;
        } 
        catch(char const * e){
          std::cerr << e << std::endl;
        }
      }
      return 0;

    default:
      return 1;
  }
}
