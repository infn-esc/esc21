//
// compile with
//  c++-52 -std=c++14 -O2 -Wall Virtual.cpp -fopt-info-vec
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
};

#include "AnyOf.h"
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
  std::random_shuffle(pa.begin(), pa.end());

  float c = 0;
#ifdef ADHOC_RTTI
  std::cout << "using ad-hoc RTTI" << std::endl;
  for (int i = 0; i < 20000;
       ++i) { // here we know that can be only either C or B
    for (auto const& p : pa)
      c += (*p).type == 3 ? static_cast<C const*>(p)->comp()
                          : static_cast<B const*>(p)->comp();
  }
#elif VARIANT
  using BorC = AnyOf<Base, B, C>;
  std::vector<BorC> anys;
  anys.reserve(2 * size);
  for (int i = 0; i < size; ++i) {
    anys.emplace_back(C(3.14));
    anys.emplace_back(B(7.1));
  }
  std::random_shuffle(anys.begin(), anys.end());

  std::cout << "using variant" << std::endl;
  for (int i = 0; i < 20000; ++i) {
    for (auto const& p : anys)
      c += p.get<Base>()->type == 3 ? p.get<C>()->comp() : p.get<B>()->comp();
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
