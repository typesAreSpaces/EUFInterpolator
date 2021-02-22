#include "UnionFind.h"

UnionFind::UnionFind() : size(0){
};

UnionFind::UnionFind(unsigned size) : representative(size, 0), rank(size, 1), size(size){
  for(unsigned i = 0; i < size; i++)
    representative[i] = i;
}

UnionFind::UnionFind(EqClass array[], unsigned size) :
  representative(array, array + size), rank(size, 1), size(size){
}

UnionFind::UnionFind(const UnionFind & other) :
  representative(other.representative),
  rank(other.rank), size(other.size){
}

UnionFind::~UnionFind(){
#if DEBUG_DESTRUCTOR_UF
  std::cout << "Done ~UnionFind" << std::endl;
#endif
};

// The first argument becomes the new
// representative, always
void UnionFind::combine(EqClass x, EqClass y){
  assert(x < size && y < size);
  if(find(x) == find(y))
    return;
  representative[find(y)] = find(x);
  rank[find(x)] += rank[find(y)];
  return;
}

void UnionFind::merge(EqClass x, EqClass y){
  assert(x < size && y < size);
  if(find(x) == find(y))
    return;
  link(find(x), find(y));
  return;
}

void UnionFind::link(EqClass x, EqClass y){
  if(rank[x] >= rank[y]){
    representative[y] = x;
    rank[x] += rank[y];
    return;
  }
  representative[x] = y;
  rank[y] += rank[x];
  return;
}

EqClass UnionFind::find(EqClass x){
  assert(x < size);
  if(x != representative[x])
    representative[x] = find(representative[x]);
  return representative[x];
}

bool UnionFind::greater(EqClass x, EqClass y){
  return rank[x] > rank[y];
}

void UnionFind::resize(unsigned sz){
  representative.resize(sz);
  rank.resize(sz);
  for(unsigned i = size; i < sz; i++){
    representative[i] = i;
    rank[i] = 1;
  }
  size = sz;
}

bool UnionFind::operator ==(const UnionFind & other){
  if(size != other.size)
    return false;
  for(unsigned i = 0; i < size; i++){
    if(representative[i] != other.representative[i])
      return false;
    if(rank[i] != other.rank[i])
      return false;
  }
  return true;
}

std::ostream & operator << (std::ostream & os, UnionFind & uf){
  for(unsigned i = 0; i < uf.representative.size(); ++i)
    os << "ID: " << i
       << " Representative: " << uf.representative[i]
       << " Rank:  " << uf.rank[i]
       << std::endl;
  os << "Size " << uf.size << std::endl;
  os << "(Remaider) The current representatives are not compressed.";
  return os;
}
