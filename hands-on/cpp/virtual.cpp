// compile with
//  c++ -O2 -Wall -Wextra virtual.cpp
//
//  * comment out the random
//  * try to change the "pattern" in the vector of pointers
//  * use adhoc RTTI with -DADHOC_RTTI
//  * remove "final"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

struct Base
{
  virtual ~Base()            = default;
  virtual float comp() const = 0;
  virtual float data() const = 0;
  int type;
};

struct A : public Base
{
  explicit A(float ix)
      : x(ix)
  {
    type = 1;
  }
  float comp() const override
  {
    return x + x;
  }
  static float doComp(float z)
  {
    return z + z;
  }
  float data() const final
  {
    return x;
  }

  float x;
};

struct B final : public Base
{
  explicit B(float ix)
      : x(ix)
  {
    type = 2;
  }
  float comp() const override
  {
    return -x;
  }
  static float doComp(float z)
  {
    return -z;
  }
  float data() const override
  {
    return x;
  }

  float x;
};

struct C final : public A
{
  explicit C(float ix)
      : A(ix)
  {
    type = 3;
  }
  float comp() const override
  {
    return x;
  }

  static float doComp(float z)
  {
    return z;
  }
};

int main()
{
  constexpr int size = 1000 * 10;

  std::vector<C> va(size, C(3.14));
  std::vector<B> vb(size, B(7.1));
  std::vector<Base const*> pa;
  pa.reserve(2 * size);
  int i = 0;
  for (auto const& a : va) {
    pa.push_back(&a);
    pa.push_back(&vb[i++]);
  }

#ifdef RANDOM
  std::shuffle(pa.begin(), pa.end(),
               std::default_random_engine{std::random_device{}()});
#endif

  float c = 0;

#ifdef ADHOC_RTTI
  std::cout << "using ad-hoc RTTI\n";
  for (int i = 0; i < 20000; ++i) {
    // here we know that can be only either C or B
    for (auto const& p : pa) {
      c += p->type == 3 ? static_cast<C const*>(p)->comp()
                        : static_cast<B const*>(p)->comp();
    }
  }
#else
  std::cout << "using virtual function\n";
  for (int i = 0; i < 20000; ++i) {
    for (auto const& p : pa) {
      c += p->comp();
    }
  }
#endif
  std::cout << "sum = " << c << '\n';
}
