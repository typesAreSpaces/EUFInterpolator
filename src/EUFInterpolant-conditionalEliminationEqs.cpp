#include "EUFInterpolant.h"

void EUFInterpolant::conditionalEliminationEqs(){
#if DEBUG_COND_ELIM_EQS
  std::cout << "Executing conditionalElimination" << std::endl;
#endif

  for(auto const & equation : assertions){
    if(!equation.is_eq()) continue;
    auto const & lhs = equation.arg(0), & rhs = equation.arg(1);
#if DEBUG_COND_ELIM_EQS
    std::cout << "--Processing this equation: " << equation << std::endl;
#endif

    if(lhs.is_const()){
      if(rhs.is_const()){
#if DEBUG_COND_ELIM_EQS 
        std::cout << "----Case lhs constant rhs constant" << std::endl;
#endif
        for(auto const & e_x : candidates(lhs)){
          for(auto const & e_y : candidates(rhs)){
            Explanation expl(ctx);
            expl.add(explainUncommons(e_x, lhs));
            expl.add(explainUncommons(e_y, rhs));
            horn_clauses.add(new HornClause(ctx, 
                  expl.result, 
                  e_x == e_y));
#if DEBUG_COND_ELIM_EQS
            std::cout << e_x << ", " << e_y << std::endl;
#endif
          }
        }
      }
      else{
#if DEBUG_COND_ELIM_EQS 
        std::cout << "----Case lhs constant rhs function app" << std::endl;
#endif
        for(auto const & e_x : candidates(lhs)){
          for(auto const & e_f_y : candidates(rhs)){
            Explanation expl(ctx);
            expl.add(explainUncommons(e_x, lhs));
            expl.add(explainUncommons(e_f_y, rhs));
            horn_clauses.add(new HornClause(ctx, 
                  expl.result, 
                  e_x == e_f_y));
#if DEBUG_COND_ELIM_EQS
            std::cout << e_x << ", " << e_f_y << std::endl;
#endif
          }
          z3::func_decl f_y = rhs.decl();
          for(auto const & arguments_f_y : cartesianProd(allCandidates(rhs))){
            Explanation expl(ctx);
            expl.add(explainUncommons(e_x, lhs));
            unsigned _index = 0;
            for(auto const & arg_f_y : arguments_f_y)
              expl.add(explainUncommons(rhs.arg(_index++), arg_f_y));
            horn_clauses.add(new HornClause(ctx, 
                  expl.result, 
                  e_x == f_y(arguments_f_y)));
#if DEBUG_COND_ELIM_EQS
            std::cout << e_x << ", " << f_y(arguments_f_y) << std::endl;
#endif
          }
        }
      }
    }
    else{
      if(rhs.is_const()){
#if DEBUG_COND_ELIM_EQS 
        std::cout << "----Case lhs function app rhs constant" << std::endl;
#endif
        for(auto const & e_y : candidates(rhs)){
          for(auto const & e_f_x : candidates(lhs)){
            Explanation expl(ctx);
            expl.add(explainUncommons(e_f_x, lhs));
            expl.add(explainUncommons(e_y, rhs));
#if DEBUG_COND_ELIM_EQS
            std::cout << e_f_x << ", " << e_y << std::endl;
#endif
            horn_clauses.add(new HornClause(ctx, 
                  expl.result, 
                  e_f_x == e_y));
          }
          z3::func_decl f_x = lhs.decl();
          for(auto const & arguments_f_x : cartesianProd(allCandidates(lhs))){
            Explanation expl(ctx);
            unsigned _index = 0;
            for(auto const & arg_f_x : arguments_f_x)
              expl.add(explainUncommons(lhs.arg(_index++), arg_f_x));
            expl.add(explainUncommons(e_y, rhs));
#if DEBUG_COND_ELIM_EQS
            std::cout << f_x(arguments_f_x) << ", " << e_y << std::endl;
#endif
            horn_clauses.add(new HornClause(ctx, 
                  expl.result, 
                  f_x(arguments_f_x) == e_y));
          }
        }
      }
      else{
#if DEBUG_COND_ELIM_EQS 
        std::cout << "----Case lhs function app rhs function app" << std::endl;
#endif
        for(auto const & e_f_x : candidates(lhs)){
          for(auto const & e_f_y : candidates(rhs)){
            Explanation expl(ctx);
            expl.add(explainUncommons(e_f_x, lhs));
            expl.add(explainUncommons(e_f_y, rhs));
#if DEBUG_COND_ELIM_EQS
            std::cout << e_f_x << ", " << e_f_y << std::endl;
#endif
            horn_clauses.add(new HornClause(ctx, 
                  expl.result, 
                  e_f_x == e_f_y));
          }
          z3::func_decl f_y = rhs.decl();
          for(auto const & arguments_f_y : cartesianProd(allCandidates(rhs))){
            Explanation expl(ctx);
            expl.add(explainUncommons(e_f_x, lhs));
            unsigned _index = 0;
            for(auto const & arg_f_y : arguments_f_y)
              expl.add(explainUncommons(rhs.arg(_index++), arg_f_y));
#if DEBUG_COND_ELIM_EQS
            std::cout << e_f_x << ", " << f_y(arguments_f_y) << std::endl;
#endif
            horn_clauses.add(new HornClause(ctx, 
                  expl.result, 
                  e_f_x == f_y(arguments_f_y)));
          }
        }
        z3::func_decl f_x = lhs.decl();
        for(auto const & arguments_f_x : cartesianProd(allCandidates(lhs))){
          for(auto const & e_f_y : candidates(rhs)){
            Explanation expl(ctx);
            unsigned _index = 0;
            for(auto const & arg_f_x : arguments_f_x)
              expl.add(explainUncommons(lhs.arg(_index++), arg_f_x));
            expl.add(explainUncommons(e_f_y, rhs));
#if DEBUG_COND_ELIM_EQS
            std::cout << f_x(arguments_f_x) << ", " << e_f_y << std::endl;
#endif
            horn_clauses.add(new HornClause(ctx, 
                  expl.result, 
                  f_x(arguments_f_x) == e_f_y));
          }
          z3::func_decl f_y = rhs.decl();
          for(auto const & arguments_f_y : cartesianProd(allCandidates(rhs))){
            Explanation expl(ctx);
            unsigned _index = 0;
            for(auto const & arg_f_x : arguments_f_x)
              expl.add(explainUncommons(lhs.arg(_index++), arg_f_x));
            _index = 0;
            for(auto const & arg_f_y : arguments_f_y)
              expl.add(explainUncommons(rhs.arg(_index++), arg_f_y));
#if DEBUG_COND_ELIM_EQS
            std::cout << f_x(arguments_f_x) << ", " << f_y(arguments_f_y) << std::endl;
#endif
            horn_clauses.add(new HornClause(ctx, 
                  expl.result, 
                  f_x(arguments_f_x) == f_y(arguments_f_y)));
          }
        }
      }
    }
  }
}
