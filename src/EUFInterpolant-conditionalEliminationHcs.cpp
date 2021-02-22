#include "EUFInterpolant.h"

void EUFInterpolant::conditionalEliminationHcs(){
#if DEBUG_COND_ELIM_HCS
  std::cout << "Executing conditionalElimination"
    "for Horn clauses" << std::endl;
#endif

  for(auto const & horn_clause : horn_clauses){
    if(horn_clause->isCommonAntecedent()){
      // Case common common
      // Nothing to do if the consequent is common

      // Case antecedent : common ; consequent : uncommon
#if DEBUG_COND_ELIM_HCS
        std::cout << "Case antecedent : common ; consequent : uncommon" << std::endl;
#endif
      if(!horn_clause->isCommonConsequent()){
        conditionalEliminationHcsComUncom(
            horn_clause->getAntecedent(),
            horn_clause->getConsequent());
      }
    }
    else{
      if(horn_clause->isCommonConsequent()){
        // Case antecedent : uncommon ; consequent : common
#if DEBUG_COND_ELIM_HCS
        std::cout << "Case antecedent : uncommon ; consequent : common" << std::endl;
#endif
        bool is_antecedent_explainable = true;
        Explanation expl(ctx);
        for(auto const & equation : horn_clause->getAntecedent()){
          auto const & lhs = equation.arg(0);
          auto const & rhs = equation.arg(1);
          if(!hsat.equiv_classes.areSameClass(lhs, rhs)){
            is_antecedent_explainable = false;
            break;
          }
          expl.add(explainUncommons(lhs, rhs));
        }
#if DEBUG_COND_ELIM_HCS
        std::cout << "Is antecedent explainable?" << std::endl;
        std::cout << (is_antecedent_explainable ? "Yes" : "No") << std::endl;
#endif
        if(is_antecedent_explainable)
          conditional_horn_clauses.add(new HornClause(ctx,
                expl.result,
                horn_clause->getConsequent()));
      }
      else{
        // Case antecedent : uncommon ; consequent : uncommon
#if DEBUG_COND_ELIM_HCS
        std::cout << "Case antecedent : uncommon ; consequent : uncommon" << std::endl;
#endif
        bool is_antecedent_explainable = true;
        z3::expr_vector _antecedent(ctx);
        for(auto const & equation : horn_clause->getAntecedent()){
          if(equation.is_common())
            _antecedent.push_back(equation);
          else{
            auto const & lhs = equation.arg(0);
            auto const & rhs = equation.arg(1);
            if(!hsat.equiv_classes.areSameClass(lhs, rhs)){
              is_antecedent_explainable = false;
              break;
            }
            for(auto const & explanation : explainUncommons(lhs, rhs))
              _antecedent.push_back(explanation);
          }
        }

        if(is_antecedent_explainable)
          conditionalEliminationHcsComUncom(
              _antecedent,
            horn_clause->getConsequent());
      }
    }
  }
}

void EUFInterpolant::conditionalEliminationHcsComUncom(z3::expr_vector const & antecedent, z3::expr const & consequent){

  assert(consequent.is_eq());
  auto const & lhs = consequent.arg(0);
  auto const & rhs = consequent.arg(1);

#if DEBUG_COND_ELIM_HCS
  std::cout << "--Processing this consequent: " << consequent << std::endl;
#endif
  if(lhs.is_const()){
    if(rhs.is_const()){
#if DEBUG_COND_ELIM_HCS 
      std::cout << "----Case lhs constant rhs constant" << std::endl;
#endif
      for(auto const & e_x : candidates(lhs)){
        for(auto const & e_y : candidates(rhs)){
          Explanation expl(ctx);
          expl.add(antecedent);
          expl.add(explainUncommons(e_x, lhs));
          expl.add(explainUncommons(e_y, rhs));
          horn_clauses.add(new HornClause(ctx, 
                expl.result, 
                e_x == e_y));
#if DEBUG_COND_ELIM_HCS
          std::cout << e_x << ", " << e_y << std::endl;
#endif
        }
      }
    }
    else{
#if DEBUG_COND_ELIM_HCS 
      std::cout << "----Case lhs constant rhs function app" << std::endl;
#endif
      for(auto const & e_x : candidates(lhs)){
        for(auto const & e_f_y : candidates(rhs)){
          Explanation expl(ctx);
          expl.add(antecedent);
          expl.add(explainUncommons(e_x, lhs));
          expl.add(explainUncommons(e_f_y, rhs));
          horn_clauses.add(new HornClause(ctx, 
                expl.result, 
                e_x == e_f_y));
#if DEBUG_COND_ELIM_HCS
          std::cout << e_x << ", " << e_f_y << std::endl;
#endif
        }
        z3::func_decl f_y = rhs.decl();
        for(auto const & arguments_f_y : cartesianProd(allCandidates(rhs))){
          Explanation expl(ctx);
          expl.add(antecedent);
          expl.add(explainUncommons(e_x, lhs));
          unsigned _index = 0;
          for(auto const & arg_f_y : arguments_f_y)
            expl.add(explainUncommons(rhs.arg(_index++), arg_f_y));
          horn_clauses.add(new HornClause(ctx, 
                expl.result, 
                e_x == f_y(arguments_f_y)));
#if DEBUG_COND_ELIM_HCS
          std::cout << e_x << ", " << f_y(arguments_f_y) << std::endl;
#endif
        }
      }
    }
  }
  else{
    if(rhs.is_const()){
#if DEBUG_COND_ELIM_HCS 
      std::cout << "----Case lhs function app rhs constant" << std::endl;
#endif
      for(auto const & e_y : candidates(rhs)){
        for(auto const & e_f_x : candidates(lhs)){
          Explanation expl(ctx);
          expl.add(antecedent);
          expl.add(explainUncommons(e_f_x, lhs));
          expl.add(explainUncommons(e_y, rhs));
#if DEBUG_COND_ELIM_HCS
          std::cout << e_f_x << ", " << e_y << std::endl;
#endif
          horn_clauses.add(new HornClause(ctx, 
                expl.result, 
                e_f_x == e_y));
        }
        z3::func_decl f_x = lhs.decl();
        for(auto const & arguments_f_x : cartesianProd(allCandidates(lhs))){
          Explanation expl(ctx);
          expl.add(antecedent);
          unsigned _index = 0;
          for(auto const & arg_f_x : arguments_f_x)
            expl.add(explainUncommons(lhs.arg(_index++), arg_f_x));
          expl.add(explainUncommons(e_y, rhs));
#if DEBUG_COND_ELIM_HCS
          std::cout << f_x(arguments_f_x) << ", " << e_y << std::endl;
#endif
          horn_clauses.add(new HornClause(ctx, 
                expl.result, 
                f_x(arguments_f_x) == e_y));
        }
      }
    }
    else{
#if DEBUG_COND_ELIM_HCS 
      std::cout << "----Case lhs function app rhs function app" << std::endl;
#endif
      for(auto const & e_f_x : candidates(lhs)){
        for(auto const & e_f_y : candidates(rhs)){
          Explanation expl(ctx);
          expl.add(antecedent);
          expl.add(explainUncommons(e_f_x, lhs));
          expl.add(explainUncommons(e_f_y, rhs));
#if DEBUG_COND_ELIM_HCS
          std::cout << e_f_x << ", " << e_f_y << std::endl;
#endif
          horn_clauses.add(new HornClause(ctx, 
                expl.result, 
                e_f_x == e_f_y));
        }
        z3::func_decl f_y = rhs.decl();
        for(auto const & arguments_f_y : cartesianProd(allCandidates(rhs))){
          Explanation expl(ctx);
          expl.add(antecedent);
          expl.add(explainUncommons(e_f_x, lhs));
          unsigned _index = 0;
          for(auto const & arg_f_y : arguments_f_y)
            expl.add(explainUncommons(rhs.arg(_index++), arg_f_y));
#if DEBUG_COND_ELIM_HCS
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
          expl.add(antecedent);
          unsigned _index = 0;
          for(auto const & arg_f_x : arguments_f_x)
            expl.add(explainUncommons(lhs.arg(_index++), arg_f_x));
          expl.add(explainUncommons(e_f_y, rhs));
#if DEBUG_COND_ELIM_HCS
          std::cout << f_x(arguments_f_x) << ", " << e_f_y << std::endl;
#endif
          horn_clauses.add(new HornClause(ctx, 
                expl.result, 
                f_x(arguments_f_x) == e_f_y));
        }
        z3::func_decl f_y = rhs.decl();
        for(auto const & arguments_f_y : cartesianProd(allCandidates(rhs))){
          Explanation expl(ctx);
          expl.add(antecedent);
          unsigned _index = 0;
          for(auto const & arg_f_x : arguments_f_x)
            expl.add(explainUncommons(lhs.arg(_index++), arg_f_x));
          _index = 0;
          for(auto const & arg_f_y : arguments_f_y)
            expl.add(explainUncommons(rhs.arg(_index++), arg_f_y));
#if DEBUG_COND_ELIM_HCS
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
