#ifndef _HORNSAT_
#define _HORNSAT_
#include <unordered_map>
#define FALSELITERAL          0 // THIS SHOULD NOT BE CHANGED!
#define DEBUG_DESTRUCTORS     0
#define DEBUGGING_SATISFIABLE 0
#define DEBUGGING_UNIONUPDATE 0
#define DEBUGGING_CONSTRUCTOR 0

#include <iostream>
#include <queue>
#include "Debugging.h"
#include "CongruenceClosureExplain.h"
#include "HornClauses.h"
#include "Util.h"

typedef unsigned TermId;
typedef unsigned LiteralId;
typedef unsigned ClauseId;

struct Clause {

  ClauseId clause_id;
  struct Clause * next;

  Clause(ClauseId id, struct Clause * clause) : clause_id(id), next(clause){}
  
  ~Clause(){
#if DEBUG_DESTRUCTORS
    std::cout << "Done ~Clause with " << clause_id  << std::endl;
#endif
  }

  struct Clause * add(ClauseId element){
    return new Clause(element, this);
  }

  class iterator {
    struct Clause const * it;
  public:
    iterator(struct Clause const * n) : it(n){}
    bool operator ==(iterator const & other){
      return it == other.it;
    }
    bool operator !=(iterator const & other){
      return it != other.it;
    }
    iterator & operator ++(){
      if(it)
        it = it->next;
      return *this;
    }
    struct Clause const * operator *() const {
      return it;
    }
  };
  iterator begin() const {
    return iterator(this);
  }
  iterator end() const {
    return iterator(nullptr);
  }
  friend std::ostream & operator << (std::ostream & os, struct Clause const & clause){
    for(auto const & it : clause)
      os << it << " ";
    return os;
  }
};

struct Literal {

  static unsigned curr_num_literals;
  // Given a literal l in Z3Subterms::subterms
  // literal_id matches l.id()
  LiteralId literal_id;
  TermId l_id, r_id;
  UnionFind::EqClass l_class, r_class; 
  bool val, is_common;
  struct Clause * clause_list;

  Literal(LiteralId literal_id, bool val, struct Clause * clause_list) :
    literal_id(literal_id), 
    l_id(0), r_id(0), 
    l_class(0), r_class(0),
    val(val), is_common(false),
    clause_list(clause_list)
  {
  }

  Literal() : Literal(curr_num_literals++, false, nullptr) {}

  void update(z3::expr const & literal, CongruenceClosureExplain & cce){
    this->l_id = literal.arg(0).id();
    this->r_id = literal.arg(1).id();
    this->l_class = cce.find(this->l_id); 
    this->r_class = cce.find(this->r_id);
    this->is_common = literal.is_common();
  }

  void update(z3::expr const & literal, CongruenceClosureExplain & cce, ClauseId clause_id){
    this->update(literal, cce);
    this->clause_list = this->clause_list->add(clause_id);
  }

  ~Literal(){
#if DEBUG_DESTRUCTORS
    std::cout << "Done ~Literal with " << literal_id  << std::endl;
#endif
  }

  friend std::ostream & operator << (std::ostream & os, const Literal & l){
    return (os 
        << "Literal: " << l.literal_id
        << " Equation: " << l.l_id << "=" << l.r_id 
        << " (" << l.l_class << "=" << l.r_class << ")"
        << " Val: " << l.val);
  }
};

struct ClassListPos {

  Literal * lit_pointer;
  CurryNode::SideOfEquation eq_side;

  ClassListPos(Literal * lit_pointer, CurryNode::SideOfEquation eq_side) :
    lit_pointer(lit_pointer), eq_side(eq_side){}

  ~ClassListPos(){
#if DEBUG_DESTRUCTORS
    std::cout << "Done ~ClassListPos with " << lit_pointer << (eq_side == CurryNode::LHS ? " LHS" : " RHS") << std::endl;
#endif
  }

  friend std::ostream & operator << (std::ostream & os, const ClassListPos & clp){
    return (os 
        << *(clp.lit_pointer) 
        << (clp.eq_side == CurryNode::LHS ? " LHS" : " RHS"));
  }
};

// "For every node u \in GT(H), we create a list (possibly empty)
// classlist(u) of pointers, such that each pointer either 
// points to the class field lclass(L) of each node L := u = v
// in the graph GC(H), or to the class field rclass(L) of each
// node L := v = u in the graph GC(H)".
// The implementation indexes the nodes and returns the list
// as a vector with the above properties.
typedef std::vector<std::vector<ClassListPos> > ClassList;

struct TermIdPair {
  TermId lhs, rhs;
  TermIdPair(TermId lhs, TermId rhs) : lhs(lhs), rhs(rhs) {}
  friend std::ostream & operator << (std::ostream & os, TermIdPair const & tp){
    return (os << tp.lhs << " " << tp.rhs);
  }
};

class Hornsat {

  friend class EUFInterpolant;

  unsigned num_hcs, num_literals;
  // This structure is only used in our approach
  // for conditional-elimination
  std::unordered_map<unsigned, HornClause *> head_term_indexer;

  UnionFindExplain         ufe;
  CongruenceClosureExplain equiv_classes;

  std::vector<Literal>   list_of_literals;
  ClassList              class_list;
  std::vector<unsigned>  num_args;
  std::vector<LiteralId> pos_lit_list;
  // 'facts' is a queue of all the (temporary)
  // literals that have value true
  std::queue<LiteralId>  facts;
  std::queue<TermIdPair> to_combine;

  bool consistent;

  void satisfiable();
  void closure();
  
 public:
  Hornsat(CongruenceClosureExplain &, HornClauses const &);
  ~Hornsat();

  void build(CongruenceClosureExplain &, HornClauses const &);
  bool isConsistent() const ;
  void unionupdate(LiteralId, LiteralId);
  friend std::ostream & operator << (std::ostream &, Hornsat const &);
};

#endif
