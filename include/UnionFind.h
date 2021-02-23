#ifndef UNIONFIND_H
#define UNIONFIND_H
#define DEBUG_DESTRUCTOR_UF false

#include <iostream>
#include <vector>
#include <cassert>
#include <list>

typedef unsigned EqClass;
typedef std::vector<std::list<EqClass> > PredList;

class UnionFind {

  protected:
    std::vector<EqClass> representative;
    std::vector<unsigned> rank;
    unsigned              size;

  public:
    UnionFind();
    UnionFind(unsigned);
    UnionFind(EqClass [], unsigned);
    UnionFind(UnionFind const &);
    virtual ~UnionFind();
    void combine(EqClass, EqClass);
    void merge(EqClass, EqClass);
    void link(EqClass, EqClass);
    virtual EqClass find(EqClass);
    bool greater(EqClass, EqClass);
    class iterator {
      UnionFind * m_uf;
      EqClass     m_element;
      unsigned    m_index;
      public:
      iterator(UnionFind * m_uf, EqClass m_element, unsigned m_index) :
        m_uf(m_uf), m_element(m_element), m_index(m_index){}
      bool operator ==(iterator const & other) { return m_element == other.m_element && m_index == other.m_index; }
      bool operator !=(iterator const & other) { return m_element != other.m_element || m_index != other.m_index; }
      iterator & operator ++(){
        m_index++;
        while(m_index < m_uf->size && m_uf->find(m_index) != m_element) m_index++;
        return *this;
      }
      EqClass operator *(){ return m_index; }
    };
    iterator begin(EqClass m_element){
      auto r = iterator(this, m_element, 0);
      if(find(0) != m_element) ++r;
      return r;
    }
    iterator end(EqClass m_element){ return iterator(this, m_element, size); }
    virtual void resize(unsigned);
    virtual bool operator ==(const UnionFind &);
    const unsigned getSize() const { return size; }
    const unsigned getRank(EqClass i) { return rank[find(i)]; }
    friend std::ostream & operator << (std::ostream &, UnionFind &);
};

#endif
