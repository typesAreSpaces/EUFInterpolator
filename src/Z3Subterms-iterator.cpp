#include "Z3Subterms.h"

Z3Subterms::iterator::iterator(Z3Subterms const * s, 
    unsigned i) : 
  m_subterms(s), m_index(i) 
{
}

Z3Subterms::iterator Z3Subterms::iterator::operator=(iterator const & other) {
  m_subterms = other.m_subterms;
  m_index = other.m_index;
  return *this;
}

bool Z3Subterms::iterator::operator==(iterator const & other) const { 
  return other.m_index == m_index;
}

bool Z3Subterms::iterator::operator!=(iterator const & other) const { 
  return other.m_index != m_index;
}

Z3Subterms::iterator & Z3Subterms::iterator::operator++() {
  ++m_index;
  while(this->notValidPosition())
    ++m_index;
  return *this;
}

z3::expr Z3Subterms::iterator::operator*() const {
  return (m_subterms->subterms)[m_index];
}
unsigned Z3Subterms::iterator::getIndex() const {
  return m_index;
}
bool Z3Subterms::iterator::notValidPosition() const {
  return m_index < m_subterms->size() && !(m_subterms->visited[m_index]);
}




