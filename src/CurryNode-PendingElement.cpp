#include "CurryNode.h"

CurryNode::PendingElement::PendingElement(CurryNode & lhs, CurryNode & rhs) : 
  tag(EQ), eq_cn(lhs, rhs) 
{
}

CurryNode::PendingElement::PendingElement(CurryNode::EquationCurryNodes const eq_cn) :
  tag(EQ), eq_cn(eq_cn) 
{
}

CurryNode::PendingElement::PendingElement(
    CurryNode::PairEquationCurryNodes const p_eq_cn) :
  tag(EQ_EQ), p_eq_cn(p_eq_cn) 
{
}

std::ostream & operator << (std::ostream & os, CurryNode::PendingElement const & pe){
  switch(pe.tag){
    case CurryNode::EQ:
      os << pe.eq_cn;
      break;
    case CurryNode::EQ_EQ:
      os << pe.p_eq_cn;
      break;
  }
  return os;
}

