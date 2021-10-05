//
// compile with
//  c++ -O2 -Wall Virtual.cpp -fopt-info-vec
//
//  comment out the random_shuffle
//  try to change the "pattern" in the vector of pointers
//  use adhoc RTTI with -DADHOC_RTTI
//   (then remove "final")
//
//
//  change -O2 in -Ofast
//  add -funroll-loops  ??

#include <cmath>

// base class
struct Base
{
  virtual ~Base()
  {}
  virtual float comp() const = 0;
  virtual float data() const = 0;
  int type; // should be an enum
};

// derived classes
struct A : public Base
{
  A()
  {}
  explicit A(float ix)
      : x(ix)
  {
    type = 1;
  }
  ~A()
  {}
  float comp() const override
  {
    return x + x;
  }
  static float doComp(float z)
  {
    return z + z;
  }
  virtual float data() const final
  {
    return x;
  }

  float x;
};

struct B final : public Base
{
  B()
  {}
  explicit B(float ix)
      : x(ix)
  {
    type = 2;
  }
  ~B()
  {}
  float comp() const override
  {
    return -x;
  }
  static float doComp(float z)
  {
    return -z;
  }
  virtual float data() const override
  {
    return x;
  }

  float x;
};

struct C final : public A
{
  C()
  {}
  explicit C(float ix)
      : A(ix)
  {
    type = 3;
  }
  ~C()
  {}
  float comp() const override
  {
    return x;
  }

  static float doComp(float z)
  {
    return z;
  }
};

#include <algorithm>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

int main()
{
  int size = 1000 * 10;

  std::vector<C> va(size, C(3.14));
  std::vector<B> vb(size, B(7.1));
  std::vector<Base const*> pa;
  pa.reserve(2 * size);
  int i = 0;
  for (auto const& a : va) {
    pa.push_back(&a);
    pa.push_back(&vb[i++]);
  }
#ifndef NORANDOM
  std::random_shuffle(pa.begin(), pa.end());
#endif

  float c = 0;
#ifdef ADHOC_RTTI
  std::cout << "using ad-hoc RTTI" << std::endl;
  for (int i = 0; i < 20000;
       ++i) { // here we know that can be only either C or B
    for (auto const& p : pa)
      c += (*p).type == 3 ? static_cast<C const*>(p)->comp()
                          : static_cast<B const*>(p)->comp();
  }
#elif USE_SOA
  // SOA
  std::vector<int> types(pa.size());
  std::vector<float> data(pa.size());
  for (auto i = 0U; i < pa.size(); ++i) {
    types[i] = pa[i]->type;
    data[i]  = pa[i]->data();
  }
  std::cout << "using SOA" << std::endl;
  for (int i = 0; i < 20000;
       ++i) { // here we know that can be only either C or B
    for (auto j = 0U; j < data.size(); ++j)
      c += types[j] == 3 ? C::doComp(data[j]) : B::doComp(data[j]);
  }
#else
  std::cout << "using virtual function" << std::endl;
  for (int i = 0; i < 20000; ++i) {
    for (auto const& p : pa)
      c += p->comp();
  }
#endif

  return int(c);
}
