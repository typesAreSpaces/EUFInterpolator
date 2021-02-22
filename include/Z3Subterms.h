#ifndef _Z3_SUBTERMS_
#define _Z3_SUBTERMS_

#include <vector>
#include <z3++.h>

struct Z3Subterms {
  z3::expr_vector subterms;
  std::vector<bool> visited;
  
  Z3Subterms(z3::context & ctx);

  class iterator {
    const Z3Subterms * m_subterms;
    unsigned m_index;

    public:
    iterator(const Z3Subterms * s, unsigned i): m_subterms(s), m_index(i) {}
    iterator operator=(const iterator & other) {
      m_subterms = other.m_subterms;
      m_index = other.m_index;
      return *this;
    }
    bool operator==(const iterator & other) const { 
      return other.m_index == m_index;
    }
    bool operator!=(const iterator & other) const { 
      return other.m_index != m_index;
    }
    iterator & operator++() {
      ++m_index;
      while(this->notValidPosition())
        ++m_index;
      return *this;
    }
    z3::expr operator*() const {
      return (m_subterms->subterms)[m_index];
    }
    unsigned getIndex() const {
      return m_index;
    }
    bool notValidPosition() const {
      return m_index < m_subterms->size() && !(m_subterms->visited[m_index]);
    }
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

