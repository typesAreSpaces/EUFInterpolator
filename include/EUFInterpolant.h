#ifndef _EUF_INTERPOLANT_
#define _EUF_INTERPOLANT_
#define DEBUG_DESTRUCTOR_EUF 0
#define DEBUG_EUFINTERPOLANT 0
#define DEBUG_EXPOSE_UNCOMMS 0
#define DEBUG_HSAT_INTER     0
#define DEBUG_COND_ELIM      0
#define DEBUG_COND_ELIM_EQS  0
#define DEBUG_COND_ELIM_HCS  0
#define DEBUG_BUILD_INTERP   0
#define DEBUG_TEMP           0

#include "Input.h"
#include "Explanation.h"
#include "Debugging.h"


class EUFInterpolant : public Input {

  typedef unsigned Z3Index ;

  z3::expr_vector const & assertions;
  Hornsat                 hsat;
  z3::expr_vector         result;
  HornClauses             conditional_horn_clauses;

  z3::expr_vector buildHCBody(z3::expr const &, z3::expr const &);

  void exposeUncommons();
  void conditionalElimination(); 
  void conditionalEliminationEqs(); 
  void conditionalEliminationHcs(); 
  void conditionalEliminationHcsComUncom(z3::expr_vector const &, z3::expr const &);

  z3::expr_vector                 explainUncommons(z3::expr const &, z3::expr const &); 
  std::list<z3::expr>             candidates(z3::expr const &);
  std::list<std::list<z3::expr> > allCandidates(z3::expr const &);
  std::vector<z3::expr_vector>    cartesianProd(std::list<std::list<z3::expr> >);

  public:
  EUFInterpolant(z3::expr_vector const &);
  ~EUFInterpolant();

  void            buildInterpolant();
  z3::expr_vector getInterpolant() const;

  friend std::ostream & operator << (std::ostream &, EUFInterpolant &);
};

#endif
