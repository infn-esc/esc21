---
title: Floating-Point Optimizations
layout: main
section: floatingpoint
---

Compile the example my-math.c with different compilation flags:

       gcc my-math.c -o my-math
       gcc -O3 my-math.c -o my-optimized-math
       gcc -Ofast my-math.c -o my-very-optimized-math

Run the different executables and check the results... what happened?

1. What have you done setting -Ofast? 
2. Try: "man gcc" to find out
3. Which one of the different flags activated by -Ofast is changing the result?
4. Try to activate each of them:

       gcc -O3 -feachflag my-math.c -o my-custom-optimized-math
