#include "EUFInterpolantWithUncomSymbols.h"

EUFInterpolantWithUncomSymbols::EUFInterpolantWithUncomSymbols(z3::expr_vector const & input_a, std::set<std::string> const & uncommon_syms) : 
  RenameWithUncomSymbols(input_a, uncommon_syms), EUFInterpolant(renamed_input)
{
}

EUFInterpolantWithUncomSymbols::~EUFInterpolantWithUncomSymbols()
{
}
