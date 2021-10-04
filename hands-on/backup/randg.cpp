#include<cmath>
#include<random>
#include<iostream>
#include <chrono>



int main() {

 auto start = std::chrono::high_resolution_clock::now();
 auto delta=start-start;

 std::mt19937 reng;
 std::normal_distribution<float> gauss(10.,1.);

 int  n = (1000 * 1000 * 12.5);

 double m=0;
 double m2=0;
 long long nn=0;
 for (int i=0; i<10; ++i) {

   if (i>0) delta -= (std::chrono::high_resolution_clock::now()-start);
   for (int j=0; j<n; ++j) {
     auto t = gauss(reng);
     m+=t;
     m2 += t*t;
     ++nn;
   }
    if (i>0) delta += (std::chrono::high_resolution_clock::now()-start);

 }

 auto deltaF = std::chrono::duration_cast<std::chrono::milliseconds>(delta).count();
 std::cout << "time " << deltaF << " N " << nn << " ave " << m/nn << " rms " << sqrt((m2-m*m/nn)/(nn-1)) << std::endl;

return 0;

}
