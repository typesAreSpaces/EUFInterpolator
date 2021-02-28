#include "UnionFind.h"

UnionFind::iterator::iterator(UnionFind * m_uf, EqClass m_element, unsigned m_index) : m_uf(m_uf), m_element(m_element), m_index(m_index)
{
}

bool UnionFind::iterator::operator ==(iterator const & other) { 
  return m_element == other.m_element && m_index == other.m_index; 
}

bool UnionFind::iterator::operator !=(iterator const & other) { 
  return m_element != other.m_element || m_index != other.m_index; 
}

UnionFind::iterator & UnionFind::iterator::operator ++(){
  m_index++;
  while(m_index < m_uf->size && m_uf->find(m_index) != m_element) m_index++;
  return *this;
}

EqClass UnionFind::iterator::operator *(){ 
  return m_index; 
}

