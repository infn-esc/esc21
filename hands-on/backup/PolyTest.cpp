//
// compile with
//  c++-52 -std=c++14 -O2 -Wall PolyTest.cpp -fopt-info-vec
//  change -O2 in -O3 then in -Ofast
//  add -funroll-loops
//  add -DESTRIN to switch to ESTRIN evaluation 
//

#include <chrono>
#include "benchmark.h"
#include <iostream>


// degree 6 polynomial (from "exp" expansion) 
inline float poly6(float y) {
constexpr float p[] = {float(0x2.p0),float(0x2.p0),float(0x1.p0),float(0x5.55523p-4),float(0x1.5554dcp-4),float(0x4.48f41p-8),float(0xb.6ad4p-12)};
#ifndef ESTRIN  // HORNER 
  float r =  p[0] + 
        y * (p[1] + 
             y * (p[2] + 
                  y * (p[3] + 
                       y * (p[4] + 
                            y * (p[5] + 
                                 y * p[6]))))) ;
#else // ESTRIN does seem to save a cycle or two
  float p56 = p[5] + y * p[6];
  float p34 = p[3] + y * p[4];
  float y2 = y*y; // +0.f;
  float p12 = p[1] + y; // *1.f; // By chance we save one operation here! Funny.
  float p36 = p34 + y2*p56;
  float p16 = p12 + y2*p36;
  float r =  p[0] + y*p16;
#endif
  return r;
}


void comp(float * b, float const * a, int N) {
  for (int i=0; i<N; ++i) {
    b[i] = poly6(a[i]);
  }
}

void init(float * x, int N, float y) {
   for ( int i = 0; i < N; ++i ) x[i]=y;
}


float * alloc(int N) {
  return new float[N];

}


int main() {

    using namespace std;
    auto start = chrono::high_resolution_clock::now();
    auto delta = start - start;


   int N = 1024;

   int size = N*8; // 64;
   float * a = alloc(size);
   float * r = alloc(size);

   init(a,size,1.3458f);

   for (int j=0; j<500000; ++j) {
     delta -= (chrono::high_resolution_clock::now()-start);
     benchmark::touch(a);
     comp(r,a,size);
     benchmark::keep(r);
     delta += (chrono::high_resolution_clock::now()-start);
    }
    std::cout<<" Computation took "
              << chrono::duration_cast<chrono::milliseconds>(delta).count()
              << " ms" << std::endl;
 
  return 0;
}
