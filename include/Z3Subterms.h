#ifndef _Z3_SUBTERMS_
#define _Z3_SUBTERMS_

#include <vector>
#include "z3++.h"

struct Z3Subterms {
  z3::expr_vector subterms;
  std::vector<bool> visited;
  
  Z3Subterms(z3::context &);

  class iterator {
    const Z3Subterms * m_subterms;
    unsigned m_index;

    public:
    iterator(Z3Subterms const *, unsigned);
    iterator operator=(iterator const &);
    bool operator==(const iterator &) const;
    bool operator!=(const iterator & other) const;
    iterator & operator++();
    z3::expr operator*() const;
    bool notValidPosition() const;
    unsigned getIndex() const;
  };

  iterator begin() const;  
  iterator end() const;

  void resize(unsigned);
  unsigned size() const;
  void set(unsigned, const z3::expr &);
  z3::expr operator[](unsigned) const;
  friend std::ostream & operator << (std::ostream &, Z3Subterms const &);
};

#endif

