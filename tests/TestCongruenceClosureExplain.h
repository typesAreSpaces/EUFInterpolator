#ifndef _TEST_CONG_CLOSURE_EXPLAIN_
#define _TEST_CONG_CLOSURE_EXPLAIN_

#include <iostream>
#include <algorithm>
#include "Input.h"

class TestCongruenceClosureExplain {

  Input input;

  public:

  TestCongruenceClosureExplain(z3::expr_vector const &);
  bool testConsistency(z3::expr_vector const &, unsigned);
  void testExplanation(unsigned);
  void merge(z3::expr const &, z3::expr const &);
  friend std::ostream & operator << (std::ostream &, TestCongruenceClosureExplain &);

};

#endif
