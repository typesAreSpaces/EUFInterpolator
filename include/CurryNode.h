#ifndef _CURRY_NODE_
#define _CURRY_NODE_ 
#define FULL_OSTREAM true

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <unordered_map>
#include "Util.h"
#include "z3++.h"

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

  enum KindEquation { CONST_EQ, APPLY_EQ  };
  enum PendingTag { EQ, EQ_EQ };
  enum SideOfEquation { LHS, RHS } ;

  private:
  struct PredNode {
    CurryNode & pred;
    CurryNode::SideOfEquation const side_of_equation;

    PredNode(CurryNode & pred, SideOfEquation const);
  };

  struct EquationZ3Ids {
    unsigned lhs_id, rhs_id;
    EquationZ3Ids(unsigned, unsigned);
  };

  typedef std::list<EquationZ3Ids> Z3EquationPointers;

  public:
  struct EquationCurryNodes {
    CurryNode const & lhs, & rhs;
    KindEquation kind_equation;

    EquationCurryNodes(CurryNode &, CurryNode &);
    EquationCurryNodes(CurryNode &, CurryNode &, KindEquation);
    friend std::ostream & operator << (std::ostream &, EquationCurryNodes const &);
  };

  struct PairEquationCurryNodes {
    EquationCurryNodes const & first, & second;

    PairEquationCurryNodes(
        EquationCurryNodes const &, 
        EquationCurryNodes const &);
    friend std::ostream & operator << (
        std::ostream &, 
        PairEquationCurryNodes const &);
  };

  struct PendingElement {
    const PendingTag tag;
    union{
      const EquationCurryNodes eq_cn;
      const PairEquationCurryNodes p_eq_cn;
    };
    PendingElement(CurryNode &, CurryNode &);
    PendingElement(EquationCurryNodes const);
    PendingElement(PairEquationCurryNodes const);
    friend std::ostream & operator << (std::ostream &, PendingElement const &);
  };

  // We use a map here because
  // id's for declarations are usually large
  typedef std::map<unsigned, CurryNode*> CurryDeclarations;
  typedef std::vector<CurryNode*>        VectorCurryNode;

  typedef std::map<CurryNode const *, std::list<PredNode> > CurryPreds;

  typedef std::list<PendingElement>         PendingElements;
  typedef std::list<PendingElement const *> PendingPointers;

  typedef std::list<EquationZ3Ids> IdsToMerge;
};

#endif
