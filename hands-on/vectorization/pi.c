// module load compilers/gcc-9.2.0_sl7
//
// gcc pi.c -o pi
// gcc -O3 pi.c -o pi
// gcc -O3 -march=native pi.c -o pi
// gcc -Ofast -march=native pi.c -o pi
// gcc -O3 -ffast-math -march=native pi.c -o pi
//
// Try also:
// -ftree-vectorize -fopt-info-vec-optimized pi.c -o pi
// -funroll-loops
//
// Look what's happening:
// objdump -d pi | less

// #include <x86intrin.h>
#include <math.h>
#include <stdio.h>
#include <sys/time.h>

typedef double data_t;

//data_t pi(const long int num_steps) {
static inline data_t pi(const long int num_steps) {

   const data_t one = 1.0;
   const data_t half = 0.5;
   const data_t four = 4.0;

   const data_t step =  one / (data_t)num_steps;
   
   data_t sum = 0;

   long int i;

   for (i=0; i< num_steps; i++) {

     data_t x = ( (data_t)i + half ) * step;

     sum += four / (one + x*x);

   }

   return step * sum;

 }


int main() {

   // Make sure this is a multiple of 4:
   const long int num_steps = 40000000;
//   const long int num_steps = 4*512*1024;
//   const long int num_steps = 4*1024;

   struct timeval start, stop;
   double time;

   // Start measuring time
   gettimeofday (&start, NULL);

   // Run compute function
   data_t result = pi(num_steps);

   // Stop measuring time
   gettimeofday (&stop, NULL);

   time = (double)(stop.tv_sec - start.tv_sec) + ((double)(stop.tv_usec - start.tv_usec)*1.0e-6);

   printf("Computed PI: %.16f Time to compute it: %.03f sec.\n", result, time );
   printf("Actual   PI: %.16f \n", M_PI );
   printf("Difference : %.16f \n", (M_PI-result) );


   return 0;
 }

