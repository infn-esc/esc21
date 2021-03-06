#include "../architecture/benchmark.h"
#include <array>
#include <bitset>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>

#include "fastSinCos.h"
#include "simpleSinCos.h"

inline int diff(float x, float y)
{
  int i;
  memcpy(&i, &x, sizeof(int));
  int j;
  memcpy(&j, &y, sizeof(int));
  return std::abs(i - j);
}

int main()
{
  // float ff = 16.f*std::numeric_limits<float>::min();
  float ff = std::numeric_limits<float>::epsilon();
  std::cout << "min " << ff << std::endl;
  int mi;
  memcpy(&mi, &ff, sizeof(int));
  ff = M_PI;
  int mx;
  memcpy(&mx, &ff, sizeof(int));

  for (float p = -ff; p <= ff; p += 0.2f)
    std::cout << simpleSin(p) << ' ' << simpleCos(p) << std::endl;

  {
    int mxDiff       = 0;
    long long avDiff = 0;
    long long n      = 0;
    float fDiff      = 0;
    auto loop        = [&](int i) {
      float p;
      memcpy(&p, &i, sizeof(int));
      auto s  = std::sin(p);
      auto c  = std::cos(p);
      auto as = simpleSin(p);
      auto ac = simpleCos(p);
      auto rs = std::abs(as - s);
      auto rc = std::abs(ac - c);
      auto sd = diff(s, as);
      auto cd = diff(c, ac);
      avDiff += sd + cd;
      n += 2;
      mxDiff = std::max(mxDiff, std::max(sd, cd));
      fDiff  = std::max(fDiff, std::max(rs, rc));
    };
    for (auto i = mi; i <= mx; i += 10)
      loop(i);
    std::cout << n << " Simple diffs " << mxDiff << " " << double(avDiff) / n
              << std::endl;
    std::cout << fDiff << std::endl;
  }

  {
    int mxDiff       = 0;
    long long avDiff = 0;
    long long n      = 0;
    float fDiff      = 0;
    auto loop        = [&](int i) {
      float p;
      memcpy(&p, &i, sizeof(int));
      auto s  = std::sin(p);
      auto c  = std::cos(p);
      auto as = fast_sinf(p);
      auto ac = fast_cosf(p);
      auto rs = std::abs(as - s);
      auto rc = std::abs(ac - c);
      auto sd = diff(s, as);
      auto cd = diff(c, ac);
      avDiff += sd + cd;
      n += 2;
      mxDiff = std::max(mxDiff, std::max(sd, cd));
      fDiff  = std::max(fDiff, std::max(rs, rc));
    };
    for (auto i = mi; i <= mx; i += 10)
      loop(i);
    std::cout << n << " fast diffs " << mxDiff << " " << double(avDiff) / n
              << std::endl;
    std::cout << fDiff << std::endl;
  }

  // timing
  auto start = std::chrono::high_resolution_clock::now();
  auto delta = start - start;

  constexpr int N = 1 << 8;
  std::cout << "working with batch of " << N << " angles" << std::endl;
  std::array<float, N> p;
  std::array<float, N> x;
  std::array<float, N> y;

  auto load = [&](int i, float q) {
    p[i] = q;
    for (int j = 1; j < 8; ++j)
      p[i + j] = p[i + j - 1] + float(M_PI / 4.);
  };
  auto comp = [&](int i) {
    y[i] = simpleSin(p[i]);
    x[i] = simpleCos(p[i]);
  };

  auto compf = [&](int i) {
    y[i] = fast_sinf(p[i]);
    x[i] = fast_cosf(p[i]);
  };

  delta = start - start;
  for (auto kk = 0; kk < 100; ++kk)
    for (float zz = -M_PI; zz < (-M_PI + M_PI / 4. - 0.001); zz += 4.e-7f) {
      for (auto j = 0; j < N; j += 8) {
        zz += 4.e-7f;
        load(j, zz);
      }
      delta -= (std::chrono::high_resolution_clock::now() - start);
      benchmark::touch(p);
      for (auto j = 0; j < N; ++j)
        comp(j);
      benchmark::keep(x);
      benchmark::keep(y);
      delta += (std::chrono::high_resolution_clock::now() - start);
    }

  std::cout
      << "Simple Computation took "
      << std::chrono::duration_cast<std::chrono::milliseconds>(delta).count()
      << " ms" << std::endl;
  double deltaS =
      std::chrono::duration_cast<std::chrono::milliseconds>(delta).count();

  delta = start - start;
  delta = start - start;
  for (auto kk = 0; kk < 100; ++kk)
    for (float zz = -M_PI; zz < (-M_PI + M_PI / 4. - 0.001); zz += 4.e-7f) {
      for (auto j = 0; j < N; j += 8) {
        zz += 4.e-7f;
        load(j, zz);
      }
      delta -= (std::chrono::high_resolution_clock::now() - start);
      benchmark::touch(p);
      for (auto j = 0; j < N; ++j)
        compf(j);
      benchmark::keep(x);
      benchmark::keep(y);
      delta += (std::chrono::high_resolution_clock::now() - start);
    }

  std::cout
      << "fast Computation took "
      << std::chrono::duration_cast<std::chrono::milliseconds>(delta).count()
      << " ms" << std::endl;
  double deltaF =
      std::chrono::duration_cast<std::chrono::milliseconds>(delta).count();

  std::cout << "f/s " << deltaF / deltaS << std::endl;

  return 0;
}
