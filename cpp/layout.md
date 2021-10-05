---
title: Data layout
layout: main
category: cpp
---

The goal of this exercise is to appreciate the impact on performance of the physical design of data structures, i.e.
their layout in memory.

Inspect, build and run the following programs:

* `sort_packed.cpp`
* `sort_cold.cpp`
* `aos.cpp`
* `aos_impr.cpp`
* `soa.cpp`

Vary the build parameter `EXTSIZE` and record the differences in execution time. Check also the output of `perf`.

The commands to build and run the code are:

```shell
[student@esc ~]$ cd esc/hands-on/cpp
[student@esc cpp]$ g++ -O3 -DEXTSIZE=8 -o sort_cold sort_cold.cpp
[student@esc cpp]$ ./sort_cold
[student@esc cpp]$ perf stat -d ./sort_cold
```

Similarly for the other programs.
