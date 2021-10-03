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

using Ext = char[ExtSize];

class Particle
{
  Vec position_;
  Ext ext_;
 public:
  Particle(Vec const& p) : position_(p) {}
  void translate(Vec const& t) { position_ += t; }
};

using Particles = std::vector<Particle>;

void translate(Particles& particles, Vec const& t)
{
  std::for_each(particles.begin(), particles.end(), [=](Particle& p) {
      p.translate(t);
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
  particles.reserve(N);
  std::generate_n(back_inserter(particles), N, [] { return Particle{ {d(e), d(e)} }; });

  std::cout << test(particles).count() << " s\n";
}
