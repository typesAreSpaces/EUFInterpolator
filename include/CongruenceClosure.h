#ifndef _CONG_CLOSURE__
#define _CONG_CLOSURE__
#define DEBUG_DESTRUCTORS_CC false

#include <iostream>
#include <unordered_map>
#include <list>
#include <utility>
#include <z3++.h>
#include "Z3Subterms.h"
#include "UnionFindExplain.h"

class SignatureTable {
  std::unordered_map<std::size_t, unsigned> sig_table;
  UnionFindExplain &                        uf;
  std::hash<std::string>                    hash_string;
  std::hash<unsigned>                       hash_unsigned;
  
public:
  SignatureTable(UnionFindExplain & uf) : uf(uf){}
  ~SignatureTable(){
#if DEBUG_DESTRUCTORS_CC
    std::cout << "Done ~SignatureTable" << std::endl;
#endif
  }
  std::size_t hash_z3expr(const z3::expr & e){
    unsigned num_args = e.num_args();
    std::string name = e.decl().name().str();
    std::size_t seed = hash_string(name);
    seed ^= hash_unsigned(num_args) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    for(unsigned i = 0; i < num_args; i++)
      seed ^= hash_unsigned(uf.find(e.arg(i).id())) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
  }
  void enter(const z3::expr & e){
    sig_table[hash_z3expr(e)] = e.id();
  }
  void erase(const z3::expr & e){
    sig_table.erase(hash_z3expr(e));
  }
  unsigned query(const z3::expr & e){
    auto r = sig_table.find(hash_z3expr(e));
    if(r == sig_table.end())
      throw "Element not in the table";
    return r->second;
  }
  friend std::ostream & operator << (std::ostream & os, const SignatureTable & st){
    return os;
  }
};

class CongruenceClosure {
  friend class Hornsat;
protected:
  const Z3Subterms & subterms;
  UnionFindExplain & ufe;
  SignatureTable     sig_table;
 public:
  CongruenceClosure(const Z3Subterms &, UnionFindExplain &);
  virtual ~CongruenceClosure();
  UnionFindExplain & getUnionFindExplain() const;
  friend std::ostream & operator << (std::ostream &, const CongruenceClosure &);
};

#endif
