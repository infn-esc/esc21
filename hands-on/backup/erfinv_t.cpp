#include "erfinv.h"

#include<cmath>
#include<ext/random>
#include<iostream>
#include <chrono>



int main() {

 auto start = std::chrono::high_resolution_clock::now();
 auto delta=start-start;


 __gnu_cxx::sfmt19937 reng;
 float norm = 2.f/float(reng.max());
 
 int  n = (1000 * 1000 * 12.5);
 constexpr int NV = 1024*8;
 
 double m=0;
 double m2=0;
 long long nn=0;
 for (int i=0; i<10; ++i) {

   if (i>0) delta -= (std::chrono::high_resolution_clock::now()-start);
   for (int j=0; j<n; j+=NV) {
     alignas(32) float r[NV];
     alignas(32) float t[NV];
     for (int k=0; k<NV; ++k) r[k]= 1.f - norm*float(reng());
     erfinV(t,r,NV);
     for (int k=0; k<NV; ++k) {
       t[k] +=10.0f;
       m+=t[k];
       m2 += t[k]*t[k];
     }
     nn+=NV;
   }
   if (i>0) delta += (std::chrono::high_resolution_clock::now()-start);

 }

 auto deltaF = std::chrono::duration_cast<std::chrono::milliseconds>(delta).count();
 std::cout << "time " << deltaF << " N " << nn << " ave " << m/nn << " rms " << sqrt((m2-m*m/nn)/(nn-1)) << std::endl;

 return 0;

}
