#include "simpleSinCos.h"
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

  struct P
  {
    std::array<float, N> phi, r;
  };

  P p;

  auto start = std::chrono::high_resolution_clock::now();
  auto delta = start - start;

  // outer loop just to make timing "macroscopical"
  for (int j = 0; j < 1000; ++j) {
    for (int i = 0; i < N; ++i) {
      p.phi[i] = -M_PI + 2. * M_PI * rgen(eng);
      p.r[i]   = rgen(eng);
    }
    constexpr int NBin = 100;
    struct Hist
    {
      int bin[NBin + 1][NBin + 1] = {0};
    };
    float binWidthI = NBin / 2.;

    Hist h;

    delta -= (std::chrono::high_resolution_clock::now() - start);
    benchmark::touch(p);
    // the real loop
    int stride = 16;
    for (int i = 0; i < N; i += stride) { // eventually account for N not being
                                          // a multiple of stride....
      int xbin[stride], ybin[stride];
      for (int k = 0; k < stride; ++k) {
        float s, c;
        simpleSinCos(p.phi[i + k], s, c);
        auto x  = p.r[i + k] * c;
        auto y  = p.r[i + k] * s;
        xbin[k] = (x + 1.f) * binWidthI;
        ybin[k] = (y + 1.f) * binWidthI;
      }
      // assert(xbin>=0 && ybin>=0);
      // assert(xbin<101 && ybin<101);
      for (int k = 0; k < stride; ++k)
        ++h.bin[xbin[k]][ybin[k]];
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
