#include "TestCongruenceClosureExplain.h"
#include <z3++.h>

TestCongruenceClosureExplain::TestCongruenceClosureExplain(z3::expr_vector const & assertions) :
  input(assertions)
{ 
}

bool TestCongruenceClosureExplain::testConsistency(z3::expr_vector const & e, 
    unsigned max_iter){
  z3::solver s(input.ctx);
  for(auto const & assertion : e)
    s.add(assertion);

  std::cout << "Starting consistency test" << std::endl;

  for(auto it = input.subterms.begin();  it != input.subterms.end(); ++it){
    unsigned index = (*it).id();
    auto repr = input.cce.z3Repr(*it);
    unsigned repr_index = repr.id();
    // Checking the non-trivial equalities
    // of the same sort
    if(index != repr_index 
        && (*it).get_sort().id() == repr.get_sort().id()){
      if(max_iter > 1)
        s.push();
      std::cout << "To check that " 
        << *it << " and " << repr 
        << " are equivalent: ";
      s.add((*it) != repr);
      switch(s.check()){
        case z3::unsat:
          std::cout << "They are." << std::endl;
          break;
        default:
          throw "There is a problem with the congruence closure algorithm";
      }
      if(max_iter > 1)
        s.pop();
      if(--max_iter == 0)
        return true;
    }
  }
  return true;
}

void TestCongruenceClosureExplain::testExplanation(unsigned max_iter){
  for(auto it = input.subterms.begin(); it != input.subterms.end(); ++it){
    unsigned index = (*it).id();
    auto repr = input.cce.z3Repr(*it);
    unsigned repr_index = repr.id();
    // Checking the non-trivial equalities
    if(index != repr_index){
      input.cce.z3Explanation(std::cout, *it, repr);
      if(--max_iter == 0)
        return;
    }
  }
  return;
}

void TestCongruenceClosureExplain::merge(z3::expr const & e1, z3::expr const & e2){
  input.cce.merge(e1, e2);
  return;
}

std::ostream & operator << (std::ostream & os, TestCongruenceClosureExplain & test) {
  unsigned num_changes = 0;
  os << "Printing all the original subterms:" << std::endl;
  os << test.input.subterms.size() << std::endl;

  for(auto it = test.input.subterms.begin(); it != test.input.subterms.end(); ++it){
    unsigned index = (*it).id();
    try {
      assert(test.input.subterms[index].id() == index);
      
      auto repr = test.input.cce.z3Repr(*it);
      unsigned repr_index = repr.id();

      os << index << ". " << (index == repr_index ? "(Same)" : (num_changes++, "(Different)")) << std::endl;
      os << "Original:       " << test.input.subterms[index] << std::endl;
      os << "Representative: " << repr << std::endl;

    }
    catch(char const * e){
      os << e << std::endl;
      os << index << std::endl;
    }
  }

  os << "Number of changes: " << num_changes;

  return os;
}
