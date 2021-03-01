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

class Hornsat {

  typedef unsigned TermId;
  typedef unsigned LiteralId;
  typedef unsigned ClauseId;

  public:

  struct Clause {

    ClauseId clause_id;
    struct Clause * next;

    Clause(ClauseId id, struct Clause *);
    ~Clause();

    struct Clause * add(ClauseId);

    class iterator {
      struct Clause const * it;
      public:
      iterator(Clause const *);
      bool operator ==(iterator const &);
      bool operator !=(iterator const &);
      iterator & operator ++();
      struct Clause const * operator *() const;
    };
    iterator begin() const;
    iterator end() const;
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

    Literal(LiteralId, bool, struct Clause *);
    Literal();
    ~Literal();

    void update(z3::expr const &, CongruenceClosureExplain &);
    void update(z3::expr const &, CongruenceClosureExplain &, 
        ClauseId);

    friend std::ostream & operator << (std::ostream &, const Literal &);
  };

  struct ClassListPos {

    Literal * lit_pointer;
    CurryNode::SideOfEquation eq_side;

    ClassListPos(Literal *, CurryNode::SideOfEquation);
    ~ClassListPos();

    friend std::ostream & operator << (std::ostream &, ClassListPos const &);
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
    TermIdPair(TermId, TermId);
    friend std::ostream & operator << (std::ostream &, TermIdPair const &);
  };

  private:
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
