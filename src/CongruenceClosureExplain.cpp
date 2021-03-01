#include "CongruenceClosureExplain.h"
#include "Hornsat.h"

// This constructor is meant to 
// clone the structure of the cce 
// (i.e. subterms and factory of curry nodes)
// with a clearn ufe (i.e. equivalence classes
// are just singletons)
CongruenceClosureExplain::CongruenceClosureExplain(
    Hornsat * hsat, CongruenceClosureExplain const & cce, 
    UnionFindExplain & ufe
    ) : 
  CongruenceClosure(cce.subterms, ufe),
  hsat(hsat),
  pending_elements(), equations_to_merge(), pending_to_propagate(),
  factory_curry_nodes(cce.factory_curry_nodes),
  lookup_table(), use_list()
{
  use_list.resize(factory_curry_nodes.size());

#if DEBUG_CONSTRUCT_CCE
  std::cout << factory_curry_nodes << std::endl;
  std::cout << factory_curry_nodes.num_terms << std::endl;
  std::cout << "(Re-)Making aliases in a cloned CongruenceClosureExplain" << std::endl;
#endif

  for(unsigned i = 0; i < factory_curry_nodes.num_terms; ++i){
    try {
      CurryNode * original_node = factory_curry_nodes.getCurryNode(i);
      CurryNode * constant_node = factory_curry_nodes.z3IndexToCurryConstant(i);
#if DEBUG_CONSTRUCT_CCE
      std::cout << "Index: " << i << std::endl;
      std::cout << *original_node << std::endl;
      std::cout << *constant_node << std::endl;
#endif
      pending_elements.emplace_back(*original_node, *constant_node);
      equations_to_merge.push_back(&pending_elements.back());
    }
    catch(char const * e){
#if DEBUG_CONSTRUCT_CCE
      std::cout << e << std::endl;
#endif
    }
  }
  merge();
#if DEBUG_CONSTRUCT_CCE
  std::cout << "Done CongruenceClosureExplain(clone) constructor" << std::endl;
#endif
}

CongruenceClosureExplain::CongruenceClosureExplain(Z3Subterms const & subterms,
    UnionFindExplain & ufe, FactoryCurryNodes & factory_curry_nodes_, 
    CurryNode::IdsToMerge const & ids_to_merge) :
  CongruenceClosure(subterms, ufe), 
  hsat(nullptr),
  pending_elements(), equations_to_merge(), pending_to_propagate(),
  factory_curry_nodes((
        factory_curry_nodes_.curryfication(subterms), 
        // NOTE: flattening also fully defines const_id and z3_id for each curry node.
        factory_curry_nodes_.flattening(pending_elements, equations_to_merge, subterms),
        factory_curry_nodes_)), 
  lookup_table(), use_list() 
{

  // There is an element in ufe for each element
  // in the curry_nodes. There
  // might be repeated elements in these collection
  // but they are unique pointers
  ufe     .resize(factory_curry_nodes.size());
  use_list.resize(factory_curry_nodes.size());

  // Process input-equations defined by user
  // using the constant ids
  for(auto x : ids_to_merge){
    auto const_lhs = factory_curry_nodes.z3IndexToCurryConstant(x.lhs_id),
         const_rhs = factory_curry_nodes.z3IndexToCurryConstant(x.rhs_id);
    // We can replace pending_to_propage by equations_to_merge, both work fine
    pushPending(pending_to_propagate, CurryNode::EquationCurryNodes(*const_lhs, *const_rhs));
  }

  merge();
  propagate();

#if DEBUG_SANITY_CHECK
  std::cout << ufe << std::endl;
  std::cout << factory_curry_nodes << std::endl;
#endif
#if DEBUG_CONSTRUCT_CCE
  std::cout << "Done CongruenceClosureExplain(original) constructor" << std::endl;
#endif
}

CongruenceClosureExplain::~CongruenceClosureExplain(){
#if DEBUG_DESTRUCTORS_CC
  std::cout << "Done ~CongruenceClosureExplain" << std::endl;
#endif
}

void CongruenceClosureExplain::pushPending(CurryNode::PendingPointers & pending_pointers, 
    const CurryNode::PendingElement & pe){
  pending_elements.push_back(pe);
  pending_pointers.push_back(&pending_elements.back());
}

void CongruenceClosureExplain::merge(){
  while(!equations_to_merge.empty()){
    const auto equations_to_merge_it = equations_to_merge.back();
    equations_to_merge.pop_back();

    switch(equations_to_merge_it->tag) {
      case CurryNode::EQ: 
        merge(equations_to_merge_it->eq_cn);
        break;
      case CurryNode::EQ_EQ:
        throw "Problem @ CongruenceClosureExplain::merge(). \
          This method cannot take as input a PairEquation.";
    }
  }
}

void CongruenceClosureExplain::merge(const CurryNode::EquationCurryNodes & equation){
  const auto & lhs = equation.lhs;

  switch(equation.kind_equation) {
    case CurryNode::CONST_EQ:
      {
#if DEBUG_MERGE
        const auto & rhs = equation.rhs;
        assert(lhs.isConstant() && rhs.isConstant());
        std::cout << "@merge. Merging constants" << std::endl
          << lhs << " and " << rhs << std::endl;
#endif
        pushPending(pending_to_propagate, equation);
        propagate();
      }
      break;
    case CurryNode::APPLY_EQ: 
      {
#if DEBUG_MERGE
        const auto & rhs = equation.rhs;
        assert(lhs.isReplaceable() && rhs.isConstant());
        std::cout << "@merge. Merging apply equations" << std::endl
          << lhs << " and " << rhs << std::endl;
#endif
        auto repr_lhs_first_argument = ufe.find(lhs.getLeftId()), repr_lhs_second_argument = ufe.find(lhs.getRightId());
        const CurryNode::EquationCurryNodes * element_found = lookup_table.query(repr_lhs_first_argument, repr_lhs_second_argument);

        if(element_found != nullptr){
#if DEBUG_MERGE
          std::cout << "@merge : Element found in lookup_table "
            << element_found << std::endl;
#endif
          pushPending(
              pending_to_propagate, 
              CurryNode::PairEquationCurryNodes(equation, *element_found));
          propagate();
        }
        else{
          lookup_table.enter(repr_lhs_first_argument, repr_lhs_second_argument, &equation);
          use_list[repr_lhs_first_argument].push_back(&equation);
          use_list[repr_lhs_second_argument].push_back(&equation);
#if DEBUG_MERGE
          std::cout << "@merge: the element wasnt in the lookup table" << std::endl
            << "------------------------------------------" << std::endl
            << "Index lhs " << lhs.getLeftId()
            << "[repr:" << repr_lhs_first_argument << "] has this in UseList "
            << std::endl << equation << std::endl
            << "Index rhs " << lhs.getRightId()
            << "[repr:" << repr_lhs_second_argument << "] has this in UseList "
            << std::endl << equation << std::endl
            << "------------------------------------------" << std::endl;
#endif
        }
      }
      break;
  }
}

void CongruenceClosureExplain::propagate(){
  while(!pending_to_propagate.empty()){
    const auto pending_element = pending_to_propagate.back();
    pending_to_propagate.pop_back();

#if DEBUG_PROPAGATE
    std::cout << "|--------------------------------------------------------" << std::endl
      << "@propagate. Taking this element " << *pending_element << std::endl
      << "--------------------------------------------------------|" << std::endl;
#endif     

    const CurryNode & a = (pending_element->tag == CurryNode::EQ) ? pending_element->eq_cn.lhs : pending_element->p_eq_cn.first.rhs;
    const CurryNode & b = (pending_element->tag == CurryNode::EQ) ? pending_element->eq_cn.rhs : pending_element->p_eq_cn.second.rhs;
    UnionFind::EqClass repr_a = ufe.find(a.getId()), repr_b = ufe.find(b.getId());
    bool repr_a_is_common = (repr_a >= subterms.size()) ? false : subterms[repr_a].is_common();
    bool repr_b_is_common = (repr_b >= subterms.size()) ? false : subterms[repr_b].is_common();

#if DEBUG_PROPAGATE
    std::cout << "|------------------------------------------" << std::endl
      << "@propagate. To merge these two inside ufe: " << std::endl
      << a << "(" << repr_a << ")" << std::endl
      << b << "(" << repr_b << ")" << std::endl
      << "------------------------------------------|" << std::endl;
#endif

    if(repr_a != repr_b) {
      //// Invariant: The representative is choosen as follows:
      // If a' and b' are both common or uncommon, then break ties using their rank
      // Otherwise, pick the node who is common
      if((repr_a_is_common && repr_b_is_common) || (!repr_a_is_common && !repr_b_is_common)){
        if(ufe.getRank(repr_a) <= ufe.getRank(repr_b))
          propagateAux(a, b, repr_a, repr_b, *pending_element);
        else
          propagateAux(b, a, repr_b, repr_a, *pending_element);
      }
      else{
        if(repr_b_is_common)
          propagateAux(a, b, repr_a, repr_b, *pending_element);
        else
          propagateAux(b, a, repr_b, repr_a, *pending_element);
      }
    }
  }
}

void CongruenceClosureExplain::propagateAux(const CurryNode & a, const CurryNode & b,
    UnionFind::EqClass repr_a, UnionFind::EqClass repr_b,
    const CurryNode::PendingElement & pending_element){
  UnionFind::EqClass old_repr_a = repr_a;

  // -------------------------------------------------------------
  // If there is a Hornsat element in the CongruenceClosureExplain
  // then perform a unionupdate to update the queue structure in 
  // Hornsat
  if(hsat){
    // The following is the original number of terms
    unsigned num_original_terms = factory_curry_nodes.num_terms;
    auto it = ufe.begin(repr_a);
    auto end = ufe.end(repr_a);
    for(; it != end; ++it){ 
      UnionFind::EqClass u = factory_curry_nodes.getCurryNode(*it)->getZ3Id();
      if(u < num_original_terms)
        hsat->unionupdate(u, repr_b); 
    }
  }
  // -------------------------------------------------------------
  ufe.combine(b.getId(), a.getId(), &pending_element);

  for(auto equation = use_list[old_repr_a].begin(); equation != use_list[old_repr_a].end(); ){
    UnionFind::EqClass c1 = (*equation)->lhs.getLeftId(), c2 = (*equation)->lhs.getRightId();
    UnionFind::EqClass repr_c1 = ufe.find(c1), repr_c2 = ufe.find(c2);
    const CurryNode::EquationCurryNodes * element_found = lookup_table.query(repr_c1, repr_c2);

    if(element_found != nullptr){

#if DEBUG_PROPAGATE_AUX
      std::cout << "|------------------------------------------------" << std::endl
        << "@propagate. To add these to pending_to_propagate" << std::endl
        << "(" << **equation << ", " << std::endl
        << *element_found << ")" << std::endl
        << "-------------------------------------------------|" << std::endl;
#endif

      pushPending(pending_to_propagate, CurryNode::PairEquationCurryNodes(**equation, *element_found));
    }
    else{

#if DEBUG_PROPAGATE_AUX
      std::cout << "|-------------------------------------------------" << std::endl
        << "@propagate. Element not found in the lookup_table" << std::endl
        << "adding " << **equation << std::endl
        << "to the lookup_table and moving it to the use_list of " << repr_b << std::endl
        << "--------------------------------------------------------|" << std::endl;
#endif

      use_list[repr_b].push_back(*equation);
      lookup_table.enter(repr_c1, repr_c2, *equation);
    }
    equation = use_list[old_repr_a].erase(equation);
  }
  assert(use_list[old_repr_a].empty());
}

UnionFind::EqClass CongruenceClosureExplain::highestNode(UnionFind::EqClass a, UnionFind & uf){
  return uf.find(a);
}

UnionFind::EqClass CongruenceClosureExplain::nearestCommonAncestor(UnionFind::EqClass a, UnionFind::EqClass b, UnionFind & uf_extra){
  return uf_extra.find(ufe.commonAncestor(a, b));
}

CurryNode::PendingPointers CongruenceClosureExplain::explain(UnionFind::EqClass x, UnionFind::EqClass y){
  CurryNode::PendingPointers ans;
  if(ufe.find(x) != ufe.find(y))
    return ans; 
  UnionFind local_uf(ufe.getSize());
  UnionFindExplain::ExplainEquations pending_proofs;

  pending_proofs.emplace_back(x, y);
  while(!pending_proofs.empty()){
    auto current_equation = pending_proofs.back();
    pending_proofs.pop_back();

    auto common_ancestor_x_y = nearestCommonAncestor(
        current_equation.source, current_equation.target, 
        local_uf);
    
    explainAlongPath(current_equation.source, 
        common_ancestor_x_y, local_uf, pending_proofs, ans);
    explainAlongPath(current_equation.target, 
        common_ancestor_x_y, local_uf, pending_proofs, ans);
  }
  return ans;
}

void CongruenceClosureExplain::explainAlongPath(UnionFind::EqClass a, UnionFind::EqClass c, 
    UnionFind & uf_extra, UnionFindExplain::ExplainEquations & pending_proofs, CurryNode::PendingPointers & ans){
  a = highestNode(a, uf_extra);
  while(a != c){
    auto b = ufe.parentProofForest(a);
    auto current_label = ufe.getLabel(a);

    switch(current_label->tag){
      case CurryNode::EQ_EQ:
        {
          auto first_equation = current_label->p_eq_cn.first,
               second_equation = current_label->p_eq_cn.second;
          UnionFind::EqClass a1 = first_equation.lhs.getLeftId(), a2 = first_equation.lhs.getRightId(),
                  b1 = second_equation.lhs.getLeftId(), b2 = second_equation.lhs.getRightId();
          pending_proofs.emplace_back(a1, b1);
          pending_proofs.emplace_back(a2, b2);
        }
      case CurryNode::EQ:
        ans.push_back(current_label);
        break;
    }
    uf_extra.combine(b, a);
    a = highestNode(b, uf_extra);
  }
}

std::ostream & CongruenceClosureExplain::giveExplanation(
    std::ostream & os, UnionFind::EqClass lhs, UnionFind::EqClass rhs){

  os << "Explain " << lhs << ", " << rhs << std::endl; 
  auto explanation = explain(lhs, rhs);
  if(explanation.size() == 0)
    return os << lhs << " and " << rhs << " belong to different equivalent classes" << std::endl;
  unsigned num = 1;
  for(auto z : explanation){
    os << "Label " << num++ << ":" << std::endl;
    os << *z << std::endl;
  }
  return os;
}

bool CongruenceClosureExplain::areSameClass(UnionFind::EqClass x, UnionFind::EqClass y){
  return this->find(x) == this->find(y);
}

bool CongruenceClosureExplain::areSameClass(z3::expr const & x, z3::expr const & y){
  return areSameClass(x.id(), y.id());
}

UnionFind::EqClass CongruenceClosureExplain::constantId(UnionFind::EqClass i){
  return factory_curry_nodes.getCurryNode(i)->getConstId();
}

UnionFind::EqClass CongruenceClosureExplain::find(UnionFind::EqClass i){
  return ufe.find(constantId(i));
}

z3::expr CongruenceClosureExplain::z3Repr(z3::expr const & e){
  auto node = factory_curry_nodes
    .getCurryNode(this->find(e.id()))->getZ3Id();
  assert(node < factory_curry_nodes.num_terms);
  return subterms[node];
}

void CongruenceClosureExplain::merge(UnionFind::EqClass x, UnionFind::EqClass y){
  // For the record: I was merging the representatives
  // of x and y. This is not a good idea since we
  // want to trace the original equations, the latter
  // makes as the original equation the equation between
  // representatives. This was fixed by merging the original
  // constant nodes.
  merge(std::move(CurryNode::EquationCurryNodes(
        *factory_curry_nodes.curry_nodes[constantId(x)],
        *factory_curry_nodes.curry_nodes[constantId(y)])));
}

void CongruenceClosureExplain::merge(z3::expr const & e1, z3::expr const & e2){
  this->merge(e1.id(), e2.id());
}

CurryNode::PendingPointers CongruenceClosureExplain::explain(const z3::expr & lhs, const z3::expr & rhs){
  return explain(constantId(lhs.id()), constantId(rhs.id()));
}

std::ostream & CongruenceClosureExplain::giveExplanation(std::ostream & os, z3::expr const & lhs, z3::expr const & rhs){
  os << "Explain " << lhs << ", " << rhs << std::endl; 
  auto explanation = explain(lhs, rhs);
  if(explanation.size() == 0)
    return os 
      << lhs << " and " << rhs 
      << " belong to different equivalent classes" << std::endl;
  unsigned num = 1;
  for(auto z : explanation){
    os << "Label " << num++ << ":" << std::endl;
    os << *z << std::endl;
  }
  return os;
}

z3::expr_vector CongruenceClosureExplain::z3Explain(
    const z3::expr & lhs, const z3::expr & rhs){
  z3::expr_vector ans(lhs.ctx());
  auto explanation = explain(lhs, rhs);
  for(auto const & z : explanation){
    switch(z->tag){
      case CurryNode::EQ:
        ans.push_back(subterms[z->eq_cn.lhs.getZ3Id()] 
            == subterms[z->eq_cn.rhs.getZ3Id()]);
      case CurryNode::EQ_EQ:
        break;
    } 
  }
  return ans;
} 

std::ostream & CongruenceClosureExplain::z3Explanation(std::ostream & os, const z3::expr & lhs, const z3::expr & rhs){
  os << "Explain " << lhs << ", " << rhs << std::endl; 
  auto explanation = z3Explain(lhs, rhs);
  if(explanation.size() == 0)
    return os << lhs << ", " << rhs << " belong to different equivalent classes" << std::endl;
  unsigned num = 1;
  for(auto const & z : explanation){
    os << "Label " << num++ << ":" << std::endl;
    os << z << std::endl;
    //os << subterms[z.lhs_id] << " = " << subterms[z.rhs_id] << std::endl;
  }
  return os;
} 

std::ostream & operator << (std::ostream & os, const CongruenceClosureExplain & cce){
  return os;
}
