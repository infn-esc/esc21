#include "../architecture/benchmark.h"
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>

std::mt19937 eng;
std::mt19937 eng2;
std::uniform_real_distribution<double> rgen(0., 1.);

template<typename T>
void put(std::ostream& co, T x)
{
  unsigned const char* out = (unsigned const char*)(&x);
  for (int i = 0; i < sizeof(T); ++i)
    co << out[i];
}

int main()
{
  constexpr int N = 1 << 14;
  std::cout << "working with batch of " << N << " particles" << std::endl;

  struct Point
  {
    float phi, r;
  };
  struct Points
  {
    std::array<Point, N> p;
  };

  Points points;

  auto start = std::chrono::high_resolution_clock::now();
  auto delta = start - start;

  // outer loop just to make timing "macroscopical"
  for (int j = 0; j < 1000; ++j) {
    for (auto& p : points.p) {
      p.phi = -M_PI + 2. * M_PI * rgen(eng);
      p.r   = rgen(eng);
    }
    constexpr int NBin = 100;
    struct Hist
    {
      int bin[NBin + 1][NBin + 1] = {0};
    };
    float binWidth = 2. / NBin;

    Hist h;

    delta -= (std::chrono::high_resolution_clock::now() - start);
    benchmark::touch(points);
    // the real loop
    for (auto const& p : points.p) {
      auto x   = p.r * std::cos(p.phi);
      auto y   = p.r * std::sin(p.phi);
      int xbin = (x + 1.f) / binWidth;
      int ybin = (y + 1.f) / binWidth;
      // assert(xbin>=0 && ybin>=0);
      // assert(xbin<101 && ybin<101);
      ++h.bin[xbin][ybin];
    }
    benchmark::keep(h);
    delta += (std::chrono::high_resolution_clock::now() - start);
    //    std::cout << '.';
  }
  std::cout
      << " Computation took "
      << std::chrono::duration_cast<std::chrono::milliseconds>(delta).count()
      << " ms" << std::endl;

  return 0;
}
