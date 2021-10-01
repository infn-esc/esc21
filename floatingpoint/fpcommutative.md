---
title: Is sum commutative using floating-point operations?
layout: main
section: floatingpoint
---

In this simple example we:

1. fill one array with a random uniform distribution of floating-point values, 
2. copy this array in a second one,
3. sort the values in the second array (but not in the first array),
4. sum togheter all the values of the first array  
4. sum togheter all the values of the second array
5. given that the sum is commutative, we expect the two sums to be the same

       #include<algorithm>
       #include<cmath>
       #include<random>
       #include<vector>

       int main() {

          int seed = time(NULL);

          double match = 0.0;
          double tot1 = 0.0;
          double tot2 = 0.0;


          // Create vector v1 and fill it with a random uniform distribution:
          std::vector<double> v1;
          std::uniform_real_distribution<double> unif(-1.,1.);
          //std::uniform_real_distribution<double> unif(-100000000.,100000000.);
          std::mt19937 rng(seed);
          for (size_t i = 0; i < 1000; ++i) {
             v1.push_back(unif(rng));
          }
	
          // Duplicate v1
          std::vector<double> v2 = v1;
	
          // Sort v2 but not v1 
          std::sort(v2.begin(), v2.end());
	
          // tot1 is a sum reduction of vector v1 (unsorted)
          // tot2 is a sum reduction of vector v2 (sorted)
          for (size_t i = 0; i < v1.size(); ++i) {
             tot1 += v1[i];
             tot2 += v2[i];
          }

          double absDiff = std::abs(tot1-tot2);
	
          // Math says sum should be commutative, let's see if it is true:
          printf("v1 avg  \t= %.16e\n", tot1);
          printf("v2 avg  \t= %.16e\n", tot2);
          printf("AbsDiff \t= %.16e\n", absDiff);
          
          return 0;
          
       }

You can compile the code and run it, then try to answer to these questions:

1. Is the sum commutative? 
2. How do you justify the result? 
3. What if the random values are in an interval larger than [-1,1]?
4. Do you still believe your answer to be right?
  

