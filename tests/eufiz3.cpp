#include <iostream>
#include <z3++.h>
#include <z3.h>
#include <z3_interp.h>

int main(int argc, char ** argv){

  switch(argc){
    case 2: 
      {
        z3::config cfg;
        cfg.set("PROOF", true);
        cfg.set("MODEL", true);
        z3::context ctx(cfg);
        z3::params _params(ctx);

        z3::solver parser(ctx);
        parser.from_file(argv[1]);

        auto const & assertions = parser.assertions();
        assert(assertions.size() == 2);

        auto const & part_a = z3::expr(ctx, Z3_mk_interpolant(ctx, assertions[0]));
        auto const & part_b = assertions[1];
        //assert(part_a.decl().decl_kind() == Z3_OP_AND
            //&& part_b.decl().decl_kind() == Z3_OP_AND);

        Z3_ast_vector * vector = new Z3_ast_vector();
        Z3_model * model = new Z3_model();

        auto result = Z3_compute_interpolant(ctx, part_a && part_b, _params, vector, model);
        if (result == Z3_L_TRUE) {
          std::cout << "true" << std::endl;
        } 
        else if (result == Z3_L_FALSE) {
          std::cout << "false" << std::endl;
          std::cout << z3::ast_vector(ctx, *vector) << std::endl;
        } 
        else {
          std::cout << "unknown" << std::endl;
        }

      }
      return 0;

    default:
      return 1;
  }
}
