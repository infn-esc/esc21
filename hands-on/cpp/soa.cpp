#include <vector>
#include <array>
#include <algorithm>
#include <random>
#include <chrono>
#include <iostream>
#include <iterator>
#include <memory>
#include <cassert>

std::default_random_engine e { std::random_device{}() };
std::uniform_real_distribution<> d;

struct Vec
{
  double x, y;
  Vec& operator+=(Vec const& o) {
    x += o.x;
    y += o.y;
    return *this;
  }
};

#ifndef EXTSIZE
#define EXTSIZE 0
#endif

constexpr int const ExtSize = EXTSIZE;

using Ext = std::array<char, ExtSize>;

struct Particles
{
  std::vector<Vec> positions;
  std::vector<Ext> exts;
};

void translate(Particles& particles, Vec const& t)
{
  auto& positions = particles.positions; 
  std::for_each(positions.begin(), positions.end(), [=](Vec& p) {
      p += t;
    });
}

std::chrono::duration<double> test(Particles& particles)
{
  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i != 1000; ++i) {
    Vec t { d(e), d(e) };
    translate(particles, t);
  }

  return std::chrono::high_resolution_clock::now() - start;
}

int main()
{
  int const N = 1000000;

  Particles particles;

  auto& positions = particles.positions;
  positions.reserve(N);
  std::generate_n(std::back_inserter(positions), N, []() -> Vec { return {d(e), d(e)}; });

  auto& exts = particles.exts;
  exts.reserve(N);
  std::generate_n(std::back_inserter(exts), N, [] { return Ext{}; });

  assert(positions.size() == exts.size());

  std::cout << test(particles).count() << " s\n";
}
