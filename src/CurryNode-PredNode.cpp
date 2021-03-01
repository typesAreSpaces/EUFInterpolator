#include "CurryNode.h"

CurryNode::PredNode::PredNode(CurryNode & pred, SideOfEquation const side_of_equation) :
  pred(pred), side_of_equation(side_of_equation)
{
}
