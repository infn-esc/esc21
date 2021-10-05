#include <random>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <functional>

std::vector<int> make_vector_rvo(int N, std::function<int()> gen)
{
  std::vector<int> result;

  result.reserve(N);
  std::generate_n(std::back_inserter(result), N, gen);

  return result;
}

void make_vector_out(std::vector<int>& result, int N, std::function<int()> gen);

int main(int argc, char* argv[])
{
  int const N = (argc > 1) ? std::atoi(argv[1]) : 1000000;

  std::random_device rd;
  std::default_random_engine eng{rd()};

  int const MAX_N = 100;
  std::uniform_int_distribution<int> dist{1, MAX_N};

  std::function<int()> gen = [&]{ return dist(eng); };

  {
    auto t0 = std::chrono::steady_clock::now();
    std::vector<int> v = make_vector_rvo(N, gen);
    auto t1 = std::chrono::steady_clock::now();
    std::cout << "rvo: " << N << " random ints in "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count()
              << " ms\n";
  }

  // std::vector<int> v;
  // make_vector_out(v, N, gen);
};
