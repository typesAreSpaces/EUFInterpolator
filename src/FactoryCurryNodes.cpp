#include "FactoryCurryNodes.h"

FactoryCurryNodes::FactoryCurryNodes(const unsigned & num_terms, const CurryDeclarations & curry_decl) :
  num_terms(num_terms), curry_decl(curry_decl), curry_predecessors()
{
  curry_nodes.resize(num_terms);
}

FactoryCurryNodes::~FactoryCurryNodes(){
  for(auto x : hash_table)
    delete x.second;
}

CurryNode * FactoryCurryNodes::queryCurryNode(unsigned id, std::string const & func_name,
    CurryNode * left, CurryNode * right){
  std::size_t index = 0;
  // We shouldnt distinguish if nodes have different ids
  // hash_combine(index, id, unsigned_hasher); 
  hash_combine(index, func_name, string_hasher);
  hash_combine(index, left, curry_hasher);
  hash_combine(index, right, curry_hasher);

  auto element = hash_table.find(index);
  if(element != hash_table.end()){
    return element->second;
  }
  else{
    CurryNode * new_element = new CurryNode(id, func_name, left, right);
    hash_table[index] = new_element;
    if(left)
      curry_predecessors[left].emplace_back(*new_element, LHS);
    if(right)
      curry_predecessors[right].emplace_back(*new_element, RHS);
    if(new_element->isReplaceable())
      to_replace.push_back(new_element);
    return new_element;
  }
}

CurryNode * FactoryCurryNodes::getCurryNode(std::string const &  func_name,
    CurryNode * left, CurryNode * right) const {
  std::size_t index = 0;
  hash_combine(index, func_name, string_hasher);
  hash_combine(index, left, curry_hasher);
  hash_combine(index, right, curry_hasher);
  return getCurryNode(index);
}

CurryNode * FactoryCurryNodes::getCurryNode(std::size_t index) const {
  auto element = hash_table.find(index);
  if(element != hash_table.end())
    return element->second;
  throw "Problem @ FactoryCurryNodes::getCurryNode(std::size_t). \
    Element not found.";
}

CurryNode * FactoryCurryNodes::getCurryNode(unsigned i) const {
  if(i >= size())
    throw "Error: out of bounds in FactoryCurryNodes::getCurryNodeById";
  if(curry_nodes[i] == nullptr)
    throw "Error: this node is a nullptr";
  return curry_nodes[i];
}

CurryNode * FactoryCurryNodes::z3IndexToCurryConstant(unsigned id) const {
  assert(id < curry_nodes.size());
  unsigned const_id = curry_nodes[id]->getConstId();
  return constantCurryNode(const_id);
}

CurryNode * FactoryCurryNodes::constantCurryNode(unsigned index) const {
  if(index >= curry_nodes.size())
    return getCurryNode(FRESH_PREFIX + std::to_string(index), nullptr, nullptr);
  auto element = curry_nodes[index];
  if(element->isReplaceable())
    return getCurryNode(FRESH_PREFIX + std::to_string(index), nullptr, nullptr);
  return element;
}

const unsigned FactoryCurryNodes::size() const {
  return curry_nodes.size();
}

unsigned FactoryCurryNodes::addExtraNodes(unsigned num){
  unsigned last_node_pos = curry_nodes.size(),
           new_last_node_pos = last_node_pos + num;
  curry_nodes.resize(new_last_node_pos);
  return new_last_node_pos;
}

void FactoryCurryNodes::updatePreds(CurryNode * from, CurryNode * to){
  curry_predecessors[to].splice(curry_predecessors[to].end(), curry_predecessors[from]);

  for(auto pred_pair : curry_predecessors[to]){
    switch(pred_pair.side_of_equation){
      case LHS:
        pred_pair.pred.updateLeft(to);
        break;
      case RHS:
        pred_pair.pred.updateRight(to);
        break;
    }
    if(pred_pair.pred.isReplaceable())
      to_replace.push_back(&pred_pair.pred);
  }  
  return;
}

void FactoryCurryNodes::updateZ3IdNotDefinedAndCommon(const Z3Subterms & subterms){
  for(auto x : hash_table){
    if(x.second->isDefined()){
      // We only update terms coming from the 
      // z3 ast
      if(x.second->getZ3Id() < subterms.size())
        x.second->updateCommon(subterms[x.second->getZ3Id()].is_common()); 
    }
    else
      // This lines effectively just sets
      // z3_id_defined to true since x.second->getId()
      // and x.second->z3_id are equal
      x.second->updateZ3Id(x.second->getId());
  }
  return;
}

void FactoryCurryNodes::curryficationHelper(z3::expr const & e, std::vector<bool> & visited){
  if(e.is_app()){
    if(visited[e.id()]) return;
    
    visited[e.id()] = true;
    unsigned num = e.num_args();
    auto f = e.decl();

    for(unsigned i = 0; i < num; i++)
      curryficationHelper(e.arg(i), visited);

    // Update curry_nodes
    if(num > 0){
      unsigned last_node_pos = curry_nodes.size();
      unsigned new_last_node_pos = addExtraNodes(num);

      // Case for first argument
      curry_nodes[last_node_pos] =
        queryCurryNode(last_node_pos,
            "apply",
            curry_decl.at(f.id()),
            curry_nodes[e.arg(0).id()]);
      // Case for the rest of the arguments
      for(unsigned i = 1; i < num; i++)
        curry_nodes[last_node_pos + i] =
          queryCurryNode(last_node_pos + i,
              "apply",
              curry_nodes[last_node_pos + i - 1],
              curry_nodes[e.arg(i).id()]);
      curry_nodes[e.id()] = curry_nodes[new_last_node_pos - 1];
    }
    else
      curry_nodes[e.id()] = curry_decl.at(f.id());

    return;
  }

  throw "Problem @ FactoryCurryNodes::curryficationHelper\
    (z3::expr const &, std::vector<bool> &). The z3::expr const & is not an app.";
}

void FactoryCurryNodes::curryfication(Z3Subterms const & e){
  std::vector<bool> visited(num_terms, false);
  for(auto it = e.begin(); it != e.end(); ++it)
    curryficationHelper(*it, visited);
  return;
}

void FactoryCurryNodes::flattening(PendingElements & pending_elements,
    PendingPointers & equations_to_merge,
    const Z3Subterms & subterms){

  // Update Z3 Ids
  unsigned const max_z3_id = num_terms;
  for(unsigned i = 0; i < max_z3_id; i++)
    if(curry_nodes[i] != nullptr)
      curry_nodes[i]->updateZ3Id(i);

  while(!to_replace.empty()){
    auto cur_curry_node = to_replace.back();
    to_replace.pop_back();

    unsigned last_node_pos = curry_nodes.size();
    curry_nodes.push_back(queryCurryNode(last_node_pos,
          FRESH_PREFIX + std::to_string(last_node_pos),
          nullptr, nullptr));
    CurryNode * new_constant = curry_nodes[last_node_pos];
    cur_curry_node->updateConstId(last_node_pos);
    // Update z3 id only if the index matches an actual
    // z3 terms (inside subterms)
    if(cur_curry_node->getZ3Id() < max_z3_id)
      new_constant->updateZ3Id(cur_curry_node->getZ3Id());
    // -----------------------------------------------------------------------------
    pending_elements.emplace_back(*cur_curry_node, *new_constant);
    equations_to_merge.push_back(&pending_elements.back());
    // -----------------------------------------------------------------------------
    updatePreds(cur_curry_node, new_constant);
  }

  // Any curry_node with z3_id_defined == false
  // doesn't match an original z3 id
  updateZ3IdNotDefinedAndCommon(subterms);
}

std::ostream & operator << (std::ostream & os, const FactoryCurryNodes & fcns){

  std::cout << "Number of original terms: " << fcns.num_terms << std::endl;

  for(unsigned i = 0; i < fcns.curry_nodes.size(); i++)
    if(fcns.curry_nodes[i] != nullptr)
      std::cout << "i: " << i << " Id: " << i << "\nThe node:\n" << *fcns.curry_nodes[i] << std::endl;

  os << "Size of FactoryCurryNodes: " << fcns.hash_table.size() << std::endl;
  return os;
}
