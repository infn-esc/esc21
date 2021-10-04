#include <chrono>
#include <array>
#include <iostream>
#include<random>
#include<memory>
#include<tuple>
#include<algorithm>
#include "benchmark.h"
#include<cassert>


std::mt19937 eng;
std::mt19937 eng2;
std::uniform_real_distribution<float> ggen(0.9f,1.1f);
std::uniform_real_distribution<float> pgen(32.f,64.f);
std::uniform_int_distribution<> cgen(2,997);
std::uniform_real_distribution<float> egen(128.f,64000.f);


int main() {

  using namespace std;
  auto start = chrono::high_resolution_clock::now();
  auto delta = start - start;

  constexpr int N=1000;

  using Matrix = std::array<std::array<float,N>,N>;
  auto pCalib = std::make_unique<Matrix>();
  for (auto & row : *pCalib ) for (auto & col : row) col = ggen(eng); 
  auto const & calib = *pCalib;
  benchmark::keep(calib);
  
  auto pPed = std::make_unique<Matrix>();
  for (auto & row : *pPed ) for (auto & col : row) col = pgen(eng);
  auto const & ped = *pPed;
  benchmark::keep(ped);

  // trash the cache
#ifdef TRASH
  auto cs = std::vector<char>(100*1000*1000);
  for (auto & c : cs) c='a'; 
#endif

  constexpr int nc=400;
  using Cell = std::tuple<short,short,float>;
  std::vector<Cell> data(nc*25);
  int s=0;
  for (int i=0; i<nc; ++i) {
   auto x = cgen(eng);
   auto y = cgen(eng);
   for (int j=x-2; j<x+3;++j) for (int k=y-2; k<y+3;++k)
     data[s++] = Cell(j,k,egen(eng));
  }
  assert(s==int(data.size()));
//  std::sort(data.begin(),data.end(),[](Cell a, Cell b){ return a<b;}); 
 
  delta += (chrono::high_resolution_clock::now()-start);
  benchmark::keep(data);
  int kp=5; benchmark::keep(kp);
  for (auto & cell : data) {
#ifdef PREF
//    if (kp++ == 5) { kp=0;
      auto const & plus10 = *((&cell)+25);
      __builtin_prefetch((&ped[std::get<0>(plus10)][std::get<1>(plus10)]));
      __builtin_prefetch((&calib[std::get<0>(plus10)][std::get<1>(plus10)]));
//    }
#endif
    std::get<2>(cell) = (std::get<2>(cell)-ped[std::get<0>(cell)][std::get<1>(cell)])*calib[std::get<0>(cell)][std::get<1>(cell)];
  }
  benchmark::keep(data);
  delta += (chrono::high_resolution_clock::now()-start);
  std::cout << " Computation took "
              << chrono::duration_cast<chrono::milliseconds>(delta).count()
              << " ms" << std::endl;


  return 0;
}
