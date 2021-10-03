#include <chrono>
#include <iostream>

void heap()
{
  int* m = new int;
  delete m;
}

int main()
{
  int const N = 100000000;

  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i != N; ++i) {
    heap();
  }

  auto stop = std::chrono::high_resolution_clock::now();

  std::cout << N << " iterations: "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(stop -
                start).count()/N
            << " ns\n";
}
