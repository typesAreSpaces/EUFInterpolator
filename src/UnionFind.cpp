#include "UnionFind.h"

UnionFind::UnionFind() : size(0)
{
}

UnionFind::UnionFind(unsigned size) : 
  representative(size, 0), rank(size, 1), size(size)
{
  for(unsigned i = 0; i < size; i++)
    representative[i] = i;
}

UnionFind::UnionFind(UnionFind::EqClass array[], unsigned size) :
  representative(array, array + size), rank(size, 1), size(size)
{
}

UnionFind::UnionFind(const UnionFind & other) :
  representative(other.representative),
  rank(other.rank), size(other.size)
{
}

UnionFind::~UnionFind(){
#if DEBUG_DESTRUCTOR_UF
  std::cout << "Done ~UnionFind" << std::endl;
#endif
}

// The first argument becomes the new
// representative, always
void UnionFind::combine(UnionFind::EqClass x, UnionFind::EqClass y){
  assert(x < size && y < size);
  if(find(x) == find(y))
    return;
  representative[find(y)] = find(x);
  rank[find(x)] += rank[find(y)];
  return;
}

void UnionFind::merge(UnionFind::EqClass x, UnionFind::EqClass y){
  assert(x < size && y < size);
  if(find(x) == find(y))
    return;
  link(find(x), find(y));
  return;
}

void UnionFind::link(UnionFind::EqClass x, UnionFind::EqClass y){
  if(rank[x] >= rank[y]){
    representative[y] = x;
    rank[x] += rank[y];
    return;
  }
  representative[x] = y;
  rank[y] += rank[x];
  return;
}

UnionFind::EqClass UnionFind::find(UnionFind::EqClass x){
  assert(x < size);
  if(x != representative[x])
    representative[x] = find(representative[x]);
  return representative[x];
}

bool UnionFind::greater(UnionFind::EqClass x, UnionFind::EqClass y){
  return rank[x] > rank[y];
}

UnionFind::iterator UnionFind::begin(UnionFind::EqClass m_element){
  auto r = iterator(this, m_element, 0);
  if(find(0) != m_element) ++r;
  return r;
}

UnionFind::iterator UnionFind::end(UnionFind::EqClass m_element){ 
  return iterator(this, m_element, size);
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

const unsigned UnionFind::getSize() const { 
  return size; 
}

const unsigned UnionFind::getRank(UnionFind::EqClass i) { 
  return rank[find(i)]; 
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
