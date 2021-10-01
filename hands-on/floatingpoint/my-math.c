//gcc -O0 unsafe-math.c -o safe-math
//gcc -O3 unsafe-math.c -o safe-math
//gcc -Ofast unsafe-math.c -o safe-math
//gcc -O3 -funsafe-math-optimizations unsafe-math.c -o unsafe-math

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
 
typedef float data_t;

int main() {

  struct timeval start, stop;
  double time;

  data_t sum = 0.0;
  data_t d = 0.0;

  // Stop measuring time
  gettimeofday (&start, NULL);

  for (size_t i=1; i<=10000000; i++) {	
    d = ((data_t)i / 3) + ((data_t)i / 7);
    sum += d;
  }

  // Stop measuring time
  gettimeofday (&stop, NULL);

  time = (double)(stop.tv_sec - start.tv_sec) + ((double)(stop.tv_usec - start.tv_usec)*1.0e-6);

  printf("Result is: %.20e Time to compute it: %.03f sec.\n", sum, time );
 
  return 0;

}

