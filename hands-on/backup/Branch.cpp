//
// compile with
// c++ -O2 -Wall -fopt-info-vec -march=native Branch.cpp
//
//  change -O2 in -Ofast
//  add -funroll-loops  ??
//
// remove/add radomness in the init
//
// change the way the conditional code is expressed
//  add -DBRANCH_ALGO=branchless2
//
// Note: since gcc72 all branch implementations produces similar assembly
//

#include <chrono>
#include <array>
#include <iostream>
#include "benchmark.h"
#include<algorithm>

#ifndef BRANCH_ALGO
#define BRANCH_ALGO branch
#warning "using default branch algorithm"
#endif

inline float branch(float x, float y, float z) {
  float ret=0;
  if (x<0 && y<0 && z<0)  ret=x;
  else if(y>0 || z>2.f) ret=y;
  else if(x>y && z<y) ret=z;
  return ret;
}

inline float branch2(float x, float y, float z) {
  if (x<0 && y<0 && z<0)  return x;
  if(y>0 || z>2.f) return y;
  if(x>y && z<y) return z;
  return 0;
}


inline float branchless(float x, float y, float z) {
  return
    (x<0) & (y<0) & (z<0) ?  x : 
    (  (y>0) | (z>2.f) ? y :
       ( (x>y) & (z<y) ? z : 0 )
       );
}

// gcc will produce branchless code only with vectorization...
// scalar code is even slower than above
inline float branchless2(float x, float y, float z) {
  auto r1 = (x>y) & (z<y) ? z : 0;
  auto r2 = (y>0) | (z>2.f) ? y : r1;
  return (x<0) & (y<0) & (z<0) ?  x : r2;
}


void init(float * x, int N, float y) {
   for ( int i = 0; i < N; ++i ) x[i]=y+i-float(N/2); // try to remove -float(N/2) 
#ifndef NO_RANDOM
   std::random_shuffle(x,x+N);                       // and or this
#endif
}


float * alloc(int N) {
  return new float[N];

}


#include<iostream>
int main() {

    using namespace std;
    auto start = chrono::high_resolution_clock::now();
    auto delta = start - start;

   int N = 1024;

   int size = N*N;
   float * a = alloc(size);
   float * b = alloc(size);
   float * c = alloc(size);
   float * r = alloc(size);

  init(c,size,0.f);
  init(a,size,1.3458f);
  init(b,size,2.467f);


  for (int i=0; i<1000; ++i) {
    delta -= (chrono::high_resolution_clock::now()-start);
    benchmark::touch(a);
    benchmark::touch(b);
    benchmark::touch(c);
    for(int j=0;j<size; ++j) 
      r[j]=BRANCH_ALGO(a[j],b[j],c[j]);
    benchmark::keep(r);
    delta += (chrono::high_resolution_clock::now()-start);
    }
    std::cout <<" Computation took "
              << chrono::duration_cast<chrono::milliseconds>(delta).count()
              << " ms" << std::endl;


  return 0;

}

