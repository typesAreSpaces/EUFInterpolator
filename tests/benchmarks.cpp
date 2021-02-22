#include <algorithm>
#include <iostream>
#include <fstream>
#include <z3++.h>
#include <set>
#include <vector>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#define DEBUG 0
#define IZ3_PREFIX     "iz3_instance_"
#define MATHSAT_PREFIX "mathsat_instance_"
#define EUFI_PREFIX    "eufi_instance_"
#define SMT_SUFFIX     ".smt2"
#define TXT_SUFFIX     ".txt"

class Symbols {

  std::set<std::string> symbols;

  void traverse(z3::expr const &);

  public:
  Symbols(z3::context & ctx, z3::expr_vector const &);

  std::set<std::string> get_symbols() const;
};

class EUFSignature {

  z3::context & ctx;
  z3::sort const & sort_A;
  z3::expr_vector constants;
  z3::func_decl_vector func_names;

  z3::expr_vector grounded_terms;
  std::set<unsigned> ids;
  z3::expr_vector a_part;
  z3::expr_vector b_part;

  bool is_valid_instance;

  unsigned const num_constants;
  unsigned const num_func_names;
  unsigned const max_arity;
  unsigned const max_num_a_lits;
  unsigned const max_ground_position;
  unsigned const limit_search;
  unsigned num_vars_to_elim;

  z3::sort_vector KArySort(z3::sort const &, unsigned);
  std::list<z3::expr_vector> AllCandidates(unsigned);
  std::vector<z3::expr_vector> CartesianProd(
      std::list<z3::expr_vector>);

  void BuildGroundTerms(unsigned);
  void BuildAPart(unsigned, unsigned);
  void BuildBPart(z3::solver &, unsigned, unsigned);

  public:
  EUFSignature(z3::context &, z3::sort const &,
      unsigned, unsigned, 
      unsigned, unsigned, 
      unsigned, unsigned);

  friend std::ostream & operator << (std::ostream &, EUFSignature const &);

  void iZ3Instance()     const;
  void MathsatInstance() const;
  void EUFIInstance()    const;
  bool IsValidInstance() const;

  void UpdateNumVarsToElim();

  std::string MyName() const;
};

void iZ3Benchmark(EUFSignature const &);
void MathsatBenchmark(EUFSignature const &);
void EUFIBenchmark(EUFSignature const &);

int main(){
  /* initialize random seed: */
  srand(time(NULL));

  z3::context ctx;
  z3::sort sort_A = ctx.uninterpreted_sort("A");

  //EUFSignature S(ctx, sort_A, 10, 5, 3, 10, 100, 1000);
  //EUFSignature S(ctx, sort_A, 20, 10, 3, 40, 100, 1000);
  //std::cout << S << std::endl;

#if 1
  unsigned num_tests = 100;
  for(unsigned i = 0; i < num_tests; ++i){
    //EUFSignature S(ctx, sort_A, 
    //num_constants, num_func_names, 
    //max_arity, max_num_a_lists, 
    //max_ground_position, limit_search);
    //EUFSignature S(ctx, sort_A, 10, 5, 3, 10, 100, 1000);
    EUFSignature S(ctx, sort_A, 20, 10, 3, 40, 100, 1000);

    if(!S.IsValidInstance()){
      --i;
      continue;
    }
    iZ3Benchmark(S);
    MathsatBenchmark(S);
    EUFIBenchmark(S);
  }
#endif

  return 0;
}

Symbols::Symbols(z3::context & ctx, z3::expr_vector const & input) : 
  symbols({})
{
  for(auto const & lit : input)
    traverse(lit);
}

void Symbols::traverse(z3::expr const & e){
  if(e.is_app()){
    unsigned num_args = e.num_args();
    std::string f_name = e.decl().name().str();
    if(f_name != "=" && f_name != "distinct")
      symbols.insert(f_name);
    for(unsigned i = 0; i < num_args; ++i)
      traverse(e.arg(i));
  }
}

std::set<std::string> Symbols::get_symbols() const {
  return symbols;
}

z3::sort_vector EUFSignature::KArySort(z3::sort const & s, unsigned n){
  z3::sort_vector ans(ctx);
  for(unsigned i = 0; i < n; ++i)
    ans.push_back(s);
  return ans;
}

std::list<z3::expr_vector > EUFSignature::AllCandidates(unsigned arity){
  std::list<z3::expr_vector > ans({});

  for(unsigned index = 0; index < arity; index++)
    ans.push_back(grounded_terms);

  return ans;
}

std::vector<z3::expr_vector> EUFSignature::CartesianProd(std::list<z3::expr_vector> candidates){
  // ans_size can get really large
  // at most O(n^n)
  // so be aware of 
  // overflow problems
  unsigned ans_size = 1;
  for(auto const & entry : candidates)
    ans_size *= entry.size();

  std::vector<z3::expr_vector> ans;
  ans.reserve(ans_size);
  for(unsigned i = 0; i < ans_size; ++i)
    ans.push_back(z3::expr_vector(ctx));

  if(ans_size){
    unsigned max_repetition = 1;
    for(auto const & entry : candidates){
      unsigned index = 0;
      while(index < ans_size)
        for(auto const & value : entry){
          unsigned index_repetition = 0;
          while(index_repetition++ < max_repetition)
            ans[index++].push_back(value);
        }
      max_repetition *= entry.size();
    }
  }

  return ans;
}

void EUFSignature::BuildGroundTerms(unsigned max_ground_position){
  for(auto const & constant : constants)
    grounded_terms.push_back(constant);
  while(true){
    for(auto const & f_name : func_names){
      unsigned current_arity = f_name.arity();
      auto const & lol = CartesianProd(AllCandidates(current_arity));
      for(auto const & input : lol){
        grounded_terms.push_back(f_name(input));
        if(grounded_terms.size() >= max_ground_position)
          return;
      }
    }
  }
}

void EUFSignature::BuildAPart(unsigned max_num_a_lits, unsigned max_ground_position){
  unsigned counter_a_part = 0;
  while(counter_a_part < max_num_a_lits){
    unsigned pos_1 = rand() % max_ground_position;
    unsigned pos_2 = rand() % max_ground_position;
    auto const & term_1 = grounded_terms[pos_1];
    auto const & term_2 = grounded_terms[pos_2];
    auto const & new_eq_or_diseq = 
      (rand() % 2 == 1) ? 
      term_1 == term_2 : 
      term_1 != term_2 ;
    if(term_1.id() != term_2.id() 
        && ids.find(new_eq_or_diseq.id()) == ids.end()){
      ids.insert(new_eq_or_diseq.id());
      a_part.push_back(new_eq_or_diseq);
      ++counter_a_part;
    }
  }
}

void EUFSignature::BuildBPart(z3::solver & sol, 
    unsigned max_ground_position, unsigned limit_search){
  auto current_check = sol.check();
  z3::solver local_sol(ctx);

  unsigned num_iter = 0;

  while(current_check != z3::unsat){
    unsigned pos_1 = rand() % max_ground_position;
    unsigned pos_2 = rand() % max_ground_position;
    auto const & term_1 = grounded_terms[pos_1];
    auto const & term_2 = grounded_terms[pos_2];
    auto const & new_eq_or_diseq = 
      (rand() % 2 == 1) ? 
      term_1 == term_2 : 
      term_1 != term_2 ;
    local_sol.push();
    local_sol.add(new_eq_or_diseq);
    auto current_local_check = local_sol.check();
    local_sol.pop();
    if(term_1.id() != term_2.id() 
        && ids.find(new_eq_or_diseq.id()) == ids.end() 
        && current_local_check == z3::sat){
      local_sol.add(new_eq_or_diseq);
      ids.insert(new_eq_or_diseq.id());
      b_part.push_back(new_eq_or_diseq);
      sol.add(new_eq_or_diseq);
    }

    current_check = sol.check();
    if(++num_iter > limit_search){
#if DEBUG
      std::cout << "Not valid B-part instance" << std::endl;
#endif
      is_valid_instance = false;
      return;
    }
  }
#if DEBUG
  std::cout << "Valid B-part instance" << std::endl;
#endif
}

void EUFSignature::iZ3Instance() const {
  std::ofstream out(IZ3_PREFIX + MyName() + SMT_SUFFIX);
  out << "(set-option :produce-interpolants true)" << std::endl;
  out << "(declare-sort " << sort_A.to_string() << " 0)" << std::endl;
  for(auto const & constant : constants)
    out << constant.decl() << std::endl;
  for(auto const & f_name : func_names)
    out << f_name << std::endl;

  out << "(assert (!" << std::endl;
  out << "(and" << std::endl;
  for(auto const & lit : a_part)
    out << lit  << std::endl;
  out << ") :named a_part" << std::endl;
  out << "))" << std::endl;

  out << "(assert (!" << std::endl;
  out << "(and" << std::endl;
  for(auto const & lit : b_part)
    out << lit  << std::endl;
  out << ") :named b_part" << std::endl;
  out << "))" << std::endl;
  out << "(check-sat)" << std::endl;
  out << "(get-interpolant a_part b_part)" << std::endl;

  out.close();
}

void EUFSignature::MathsatInstance() const {
  std::ofstream out(MATHSAT_PREFIX + MyName() + SMT_SUFFIX);
  out << "(set-option :produce-interpolants true)" << std::endl;
  out << "(declare-sort " << sort_A.to_string() << " 0)" << std::endl;
  for(auto const & constant : constants)
    out << constant.decl() << std::endl;
  for(auto const & f_name : func_names)
    out << f_name << std::endl;

  out << "(assert (!" << std::endl;
  out << "(and" << std::endl;
  for(auto const & lit : a_part)
    out << lit  << std::endl;
  out << ") :interpolation-group a_part" << std::endl;
  out << "))" << std::endl;

  out << "(assert (!" << std::endl;
  out << "(and" << std::endl;
  for(auto const & lit : b_part)
    out << lit  << std::endl;
  out << ") :interpolation-group b_part" << std::endl;
  out << "))" << std::endl;
  out << "(check-sat)" << std::endl;
  out << "(get-interpolant (a_part))" << std::endl;
  out << "(exit)" << std::endl;

  out.close();
}

void EUFSignature::EUFIInstance() const {
  std::ofstream out(EUFI_PREFIX + MyName() + SMT_SUFFIX);
  out << "(declare-sort " << sort_A.to_string() << " 0)" << std::endl;
  for(auto const & constant : constants)
    out << constant.decl() << std::endl;
  for(auto const & f_name : func_names)
    out << f_name << std::endl;

  out << "(assert (and" << std::endl;
  for(auto const & lit : a_part)
    out << lit  << std::endl;
  out << "))" << std::endl;

  out << "(assert (and" << std::endl;
  for(auto const & lit : b_part)
    out << lit  << std::endl;
  out << "))" << std::endl;
  out << "(check-sat)" << std::endl;

  out.close();
}

bool EUFSignature::IsValidInstance() const {
  return is_valid_instance;
}

EUFSignature::EUFSignature(z3::context & ctx, z3::sort const & sort_A,
    unsigned num_constants, unsigned num_func_names, 
    unsigned max_arity, unsigned max_num_a_lits, 
    unsigned max_ground_position, unsigned limit_search) : 
  ctx(ctx), sort_A(sort_A), constants(ctx), func_names(ctx),
  grounded_terms(ctx), 
  ids({}), a_part(ctx), b_part(ctx),
  is_valid_instance(true),
  num_constants(num_constants), num_func_names(num_func_names),
  max_arity(max_arity), max_num_a_lits(max_num_a_lits),
  max_ground_position(max_ground_position), limit_search(limit_search),
  num_vars_to_elim(0)
{
  for(unsigned i = 0; i < num_constants; ++i)
    constants.push_back(ctx.constant(
          ("x_" + std::to_string(i)).c_str(), sort_A));
  for(unsigned i = 0; i < num_func_names; ++i)
    func_names.push_back(ctx.function(
          ("f_" + std::to_string(i)).c_str(), 
          KArySort(sort_A, rand() % max_arity + 2), sort_A));

  BuildGroundTerms(max_ground_position);
  BuildAPart(max_num_a_lits, max_ground_position);

  z3::solver sol(ctx);
  for(auto const & eq : a_part)
    sol.add(eq);
  is_valid_instance = sol.check() == z3::sat;
  if(is_valid_instance){
#if DEBUG
    std::cout << "Valid A-part instance" << std::endl;
#endif
    BuildBPart(sol, max_ground_position, limit_search);
    if(is_valid_instance)
      UpdateNumVarsToElim();
  }
#if DEBUG
  else
    std::cout << "Not valid A-part instance" << std::endl;
#endif
}

std::ostream & operator << (std::ostream & os, EUFSignature const & eufs){
  os << "A part formulas:" << std::endl;
  os << eufs.a_part << std::endl;
  os << "B part formulas:" << std::endl;
  os << eufs.b_part << std::endl;
  return os;
}

void EUFSignature::UpdateNumVarsToElim(){
  std::set<std::string> a_symbols = Symbols(ctx, a_part).get_symbols();
  std::set<std::string> b_symbols = Symbols(ctx, b_part).get_symbols();

#if DEBUG
  std::cout << "Symbols in A-part" << std::endl;
  for(auto const & a : a_symbols)
    std::cout << a << std::endl;
  std::cout << "Symbols in B-part" << std::endl;
  for(auto const & b : b_symbols)
    std::cout << b << std::endl;
#endif

  std::set<std::string> result({});
  std::set_difference(
      a_symbols.begin(), a_symbols.end(),
      b_symbols.begin(), b_symbols.end(),
      std::inserter(result, result.end())
      );
  num_vars_to_elim = result.size();
}

std::string EUFSignature::MyName() const {
  return 
    std::to_string(num_constants) 
    + "_" + std::to_string(num_func_names)
    + "_" + std::to_string(max_arity)
    + "_" + std::to_string(max_num_a_lits)
    + "_" + std::to_string(max_ground_position)
    + "_" + std::to_string(limit_search)
    + "_" + std::to_string(num_vars_to_elim)
    ;
}

void iZ3Benchmark(EUFSignature const & S){
  std::string file_name = "./results/iz3_benchmark.txt";
  system(("test -f " + file_name + " || touch " + file_name).c_str());

  S.iZ3Instance();
  system(("echo \"test: " + S.MyName() + "\">> " + file_name).c_str());
  system(("{ ../../bin/precision-time ../../bin/z3-interp " + (IZ3_PREFIX + S.MyName()) + SMT_SUFFIX + "; } 2>> " + file_name).c_str());
  system(("rm " + (IZ3_PREFIX + S.MyName()) + SMT_SUFFIX).c_str());

}

void MathsatBenchmark(EUFSignature const & S){
  std::string file_name = "./results/mathsat_benchmark.txt";
  system(("test -f " + file_name + " || touch " + file_name).c_str());

  S.MathsatInstance();
  system(("echo \"test: " + S.MyName() + "\">> " + file_name).c_str());
  system(("{ ../../bin/precision-time ../../bin/mathsat " + (MATHSAT_PREFIX + S.MyName()) + SMT_SUFFIX + "; } 2>> " + file_name).c_str());
  system(("rm " + (MATHSAT_PREFIX + S.MyName()) + SMT_SUFFIX).c_str());
}

void EUFIBenchmark(EUFSignature const & S){
  std::string file_name = "./results/eufi_benchmark.txt";
  system(("test -f " + file_name + " || touch " + file_name).c_str());

  S.EUFIInstance();
  system(("echo \"test: " + S.MyName() + "\">> " + file_name).c_str());
  system(("{ ../../bin/precision-time ./bin/eufi " + (EUFI_PREFIX + S.MyName()) + SMT_SUFFIX + "; } 2>> " + file_name).c_str());
  system(("rm " + (EUFI_PREFIX + S.MyName()) + SMT_SUFFIX).c_str());
}
