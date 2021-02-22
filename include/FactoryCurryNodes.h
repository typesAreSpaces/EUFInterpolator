#ifndef _FAC_CURRY_NODES_
#define _FAC_CURRY_NODES_
#define FRESH_PREFIX "fresh_"

#include <iostream>
#include <unordered_map>
#include <z3++.h>
#include "Z3Subterms.h"
#include "CurryNode.h"

class FactoryCurryNodes {
  friend class CongruenceClosureExplain;

  std::hash<unsigned>                         unsigned_hasher;
  std::hash<std::string>                      string_hasher;
  std::hash<CurryNode*>                       curry_hasher;
  std::unordered_map<std::size_t, CurryNode*> hash_table;

  unsigned const & num_terms;

  VectorCurryNode           curry_nodes;
  const CurryDeclarations & curry_decl;
  CurryPreds                curry_predecessors;
  std::list<CurryNode *>    to_replace;

  unsigned addExtraNodes(unsigned);
  void     updatePreds(CurryNode *, CurryNode *);
  void     updateZ3IdNotDefinedAndCommon(const Z3Subterms &);
  void     curryficationHelper(z3::expr const &, std::vector<bool> &);
  void     curryfication(Z3Subterms const &);
  void     flattening(PendingElements &, PendingPointers &, const Z3Subterms &);

  CurryNode * getCurryNode(std::string const &, CurryNode *, CurryNode *) const;
  CurryNode * getCurryNode(std::size_t)   const;
  CurryNode * constantCurryNode(unsigned) const;

  public:
  FactoryCurryNodes(const unsigned &, const CurryDeclarations &);
  ~FactoryCurryNodes();

  CurryNode * queryCurryNode(unsigned, std::string const &, CurryNode *, CurryNode *);

  CurryNode * getCurryNode(unsigned)           const;
  CurryNode * z3IndexToCurryConstant(unsigned) const;

  const unsigned size() const;

  friend std::ostream & operator << (std::ostream &, const FactoryCurryNodes &);
};

#endif
