#ifndef _UTIL_
#define _UTIL_

#include "z3++.h"

class CurryNode;

namespace Util {
  bool compareEquation(const z3::expr &, const z3::expr &);
  bool compareTerm(const z3::expr &, const z3::expr &);

  extern std::hash<unsigned>    unsigned_hasher;
  extern std::hash<std::string> string_hasher;
  extern std::hash<CurryNode*>  curry_hasher;

  template <class T>
    inline void hash_combine(
        std::size_t & seed, 
        T const & v, 
        std::hash<T> const & hasher){
      seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
}

#endif
