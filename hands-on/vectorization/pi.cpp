#include <x86intrin.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <type_traits>

// c++ -Wall -Ofast pi.cpp  -fopt-info-vec -march=native
//  then add -mprefer-vector-width=512

float pi(int num_steps)
{
  float step = 1.0f / float(num_steps);
  float sum  = 0;
  for (int i = 0; i < num_steps; i++) {
    // for (int i=num_steps-1; i>=0; --i){
    auto x = (float(i) + 0.5f) * step;
    sum += 4.0f / (1.0f + x * x);
  }
  return step * sum;
}

#include <chrono>
#include <iostream>

void go(int num_steps)
{
  // std::accumulate(   ,0.f,[&]{})
  std::cout << "nsteps, step " << num_steps << ' ' << 1. / num_steps
            << std::endl;
  auto start      = std::chrono::high_resolution_clock::now();
  auto res        = pi(num_steps);
  auto total_time = std::chrono::high_resolution_clock::now() - start;
  std::cout << "pi = " << res << " in " << total_time.count() << std::endl;
}

int main()
{
  auto total_time = std::chrono::high_resolution_clock::duration{};

  constexpr int num_steps = 64 * 1024 * 1024;

  go(num_steps);
  go(num_steps / 2);
  go(num_steps / 16);
  go(num_steps / 64);
  go(num_steps / 512);

  return 0;
}
