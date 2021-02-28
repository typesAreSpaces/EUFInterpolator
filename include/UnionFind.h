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

  void link(EqClass, EqClass);

  protected:
    std::vector<EqClass>  representative;
    std::vector<unsigned> rank;
    unsigned              size;

  public:
    UnionFind();
    UnionFind(unsigned);
    UnionFind(EqClass [], unsigned);
    UnionFind(UnionFind const &);
    virtual ~UnionFind();

    void            combine(EqClass, EqClass);
    void            merge(EqClass, EqClass);
    virtual EqClass find(EqClass);
    bool            greater(EqClass, EqClass);

    class iterator {
      UnionFind * m_uf;
      EqClass     m_element;
      unsigned    m_index;

      public:
      iterator(UnionFind *, EqClass, unsigned);

      bool       operator ==(iterator const &);
      bool       operator !=(iterator const &);
      iterator & operator ++();
      EqClass    operator *();
    };

    iterator begin(EqClass);
    iterator end(EqClass);

    virtual void          resize(unsigned);
    const unsigned        getSize() const;
    const unsigned        getRank(EqClass);
    virtual bool          operator ==(const UnionFind &);
    friend std::ostream & operator << (std::ostream &, UnionFind &);
};

#endif
