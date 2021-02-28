#ifndef _CURRY_NODE_
#define _CURRY_NODE_ 
#define FULL_OSTREAM true

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <unordered_map>
#include "z3++.h"

template <class T>
inline void hash_combine(std::size_t & seed, const T & v, const std::hash<T> & hasher){
  seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

class CurryNode {

  unsigned id, z3_id, const_id;
  bool z3_id_defined;
  std::string func_name;
  CurryNode * left, * right;
  bool is_common;
  unsigned space;

  public:
  CurryNode(unsigned, std::string, CurryNode *, CurryNode *);
  const bool isConstant()    const;
  const bool isReplaceable() const;
  const bool isDefined()     const;

  void updateLeft(CurryNode *);
  void updateRight(CurryNode *);
  void updateAllIds(unsigned);
  void updateZ3Id(unsigned);
  void updateConstId(unsigned);
  void updateCommon(bool);

  const unsigned getId()      const;
  const unsigned getLeftId()  const;
  const unsigned getRightId() const;
  const unsigned getZ3Id()    const;
  const unsigned getConstId() const;
  const bool     isCommon()   const;
  std::size_t    hash();

  friend std::ostream & operator << (std::ostream &, const CurryNode &);
};

enum SideOfEquation { LHS, RHS } ;
enum KindEquation { CONST_EQ, APPLY_EQ  };
enum PendingTag { EQ, EQ_EQ };

struct PredNode {
  CurryNode & pred;
  const SideOfEquation side_of_equation;
  PredNode(CurryNode & pred, const SideOfEquation side_of_equation) :
    pred(pred), side_of_equation(side_of_equation){
    }
  friend std::ostream & operator << (std::ostream & os, const PredNode & pred_pair){
    os << pred_pair.pred << " " << (pred_pair.side_of_equation == LHS ? "LHS" : "RHS");
    return os;
  }
};

struct EquationCurryNodes {
  const CurryNode & lhs, & rhs;
  KindEquation kind_equation;

  EquationCurryNodes(CurryNode & lhs, CurryNode & rhs) :
    lhs(lhs), rhs(rhs), kind_equation(lhs.isConstant() ? CONST_EQ : APPLY_EQ) {}
  EquationCurryNodes(CurryNode & lhs, CurryNode & rhs, KindEquation kind_equation) :
    lhs(lhs), rhs(rhs), kind_equation(kind_equation) {}
  friend std::ostream & operator << (std::ostream & os, const EquationCurryNodes & ecns){
    os << ecns.lhs << " = " << ecns.rhs;
    return os;
  }
};

struct PairEquationCurryNodes {
  const EquationCurryNodes & first, & second;

  PairEquationCurryNodes(const EquationCurryNodes & first, const EquationCurryNodes & second) :
    first(first), second(second) {}
  friend std::ostream & operator << (std::ostream & os, const PairEquationCurryNodes & pecns){
    os << "(" << pecns.first << ", " << pecns.second << ")";
    return os;
  }
};

struct PendingElement {
  const PendingTag tag;
  union{
    const EquationCurryNodes eq_cn;
    const PairEquationCurryNodes p_eq_cn;
  };
  PendingElement(CurryNode & lhs, CurryNode & rhs) : 
    tag(EQ), eq_cn(lhs, rhs) { }
  PendingElement(const EquationCurryNodes eq_cn) :
    tag(EQ), eq_cn(eq_cn) { }
  PendingElement(const PairEquationCurryNodes p_eq_cn) :
    tag(EQ_EQ), p_eq_cn(p_eq_cn) { }
  friend std::ostream & operator << (std::ostream & os, const PendingElement & pe){
    switch(pe.tag){
      case EQ:
        os << pe.eq_cn;
        break;
      case EQ_EQ:
        os << pe.p_eq_cn;
        break;
    }
    return os;
  }
};

struct EquationZ3Ids {
  unsigned lhs_id, rhs_id;
  EquationZ3Ids(unsigned lhs_id, unsigned rhs_id) :
    lhs_id(lhs_id), rhs_id(rhs_id) {}
  friend std::ostream & operator << (std::ostream & os, const EquationZ3Ids & ez3ids){
    os << ez3ids.lhs_id << " = " << ez3ids.rhs_id;
    return os;
  }
};

// We use a map here because
// id's for declarations are usually large
typedef std::map<unsigned, CurryNode*> CurryDeclarations;
typedef std::vector<CurryNode*>        VectorCurryNode;

typedef std::map<const CurryNode *, std::list<PredNode> > CurryPreds;

typedef std::list<PendingElement>         PendingElements;
typedef std::list<const PendingElement *> PendingPointers;
typedef std::list<EquationZ3Ids>          Z3EquationPointers;

typedef std::list<EquationZ3Ids> IdsToMerge;

#endif
