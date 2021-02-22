#ifndef _HORN_CLAUSES_
#define _HORN_CLAUSES_
#define notInSet(y, x) x.find(y) == x.end()
#define DEBUG_HORN_CLAUSES   0
#define DEBUG_ADDINGHC       0
#define DEBUG_DESTRUCTOR_HCS 0
#define DEBUG_SIMPLIFY       0
#define DEBUG_SIMPLIFY_BLOCK 0

#include <unordered_map>
#include "Z3Subterms.h"
#include "HornClause.h"

typedef std::unordered_map<unsigned, HornClause *>   UnOrdMapHornClauses;
// Should the image be a list of HornClause* ? 
// The container in the image is still not really well-decided
typedef std::list<HornClause *>                    SimplHornEntry;
typedef std::unordered_map<unsigned, SimplHornEntry> SimplificationTable;

class HornClauses {

  UnionFindExplain &  ufe;
  UnOrdMapHornClauses horn_clauses;
  unsigned            curr_num_horn_clauses;
  unsigned            max_lit_id;
  z3::expr_vector     hash_consed_hcs;

  SimplificationTable simplification_table;
  
 public:
  HornClauses(z3::context &, UnionFindExplain &, unsigned);
  ~HornClauses();

  void filterCommons();
  void simplifyBlock(SimplHornEntry const &);
  void simplify(); 
  void swapHornClauses(unsigned, unsigned);
  void add(HornClause *);

  class iterator {
    UnOrdMapHornClauses::iterator it;
    public:
      iterator(UnOrdMapHornClauses::iterator it) : 
        it(it) {}
      bool operator ==(iterator const & other){
        return it == other.it;
      }
      bool operator !=(iterator const & other){
        return it != other.it;
      }
      iterator & operator ++(){
        ++it;
        return *this;
      }
      HornClause const * operator *() const {
        return it->second;
      }
  };

  iterator begin() {
    return iterator(this->horn_clauses.begin());
  }
  iterator end() {
    return iterator(this->horn_clauses.end());
  }

  unsigned                       size() const;
  HornClause *                   operator[] (unsigned) const; 
  std::vector<HornClause*> const getHornClauses() const;
  unsigned                       getUFESize()  const;
  unsigned                       getMaxLitId() const;
  friend std::ostream &          operator << (std::ostream &, const HornClauses &);
};

#endif
