#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <iostream>
#include <iterator>
#include <tuple>

std::default_random_engine e { std::random_device{}() };
std::uniform_int_distribution<> d;

#ifndef EXTSIZE
#define EXTSIZE 0
#endif

constexpr int const ExtSize = EXTSIZE;

struct S
{
  int n;
  char ext[ExtSize];
};

int main()
{
  int const N = 10000000;

  std::vector<S> v;
  std::generate_n(std::back_inserter(v), N, [] { return S { d(e) }; });

  auto start = std::chrono::high_resolution_clock::now();

  std::sort(v.begin(), v.end(), [](S const& l, S const& r) { return l.n < r.n; });

  auto delta = std::chrono::high_resolution_clock::now() - start;
  std::cout << std::chrono::duration<double>(delta).count() << " s\n";
}
