#include "Explanation.h"

Explanation::Explanation(z3::context & ctx) :
  result(ctx), current_ids()
{
}

Explanation::~Explanation()
{
}

void Explanation::add(z3::expr_vector const & explanation){
  for(auto const & element : explanation)
    if(!(IsMember(current_ids, element))){
      current_ids.insert(element.id());
      result.push_back(element);
    }
  return; 
}

