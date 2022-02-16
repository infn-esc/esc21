---
title: Performance Portability With alpaka
layout: main
section: parallelism
---

The alpaka cheat sheet is a very useful source of information:
<a href="https://alpaka.readthedocs.io/en/0.7.0/basic/cheatsheet.html" target="_blank">https://alpaka.readthedocs.io/en/0.7.0/basic/cheatsheet.html</a>

```bash
$ cd esc21/hands-on/alpaka_exercises
```


Check that your environment is correctly configured to compile CUDA code by running:
```bash
module load compilers/gcc-9.2.0_sl7 boost_1_77_0_gcc8 compilers/cuda-11.2
$ nvcc --version
 nvcc: NVIDIA (R) Cuda compiler driver
 Copyright (c) 2005-2020 NVIDIA Corporation
 Built on Mon_Nov_30_19:08:53_PST_2020
 Cuda compilation tools, release 11.2, V11.2.67
 Build cuda_11.2.r11.2/compiler.29373293_0
```

Compile and run the `deviceQuery` application:
```bash
cd esc21/hands-on/cuda_exercises/utils/deviceQuery
make
```

You can get some useful information about the features and the limits that you will find on the device you will be running your code on. For example:
```
./deviceQuery Starting...

 CUDA Device Query (Runtime API) version (CUDART static linking)

Detected 4 CUDA Capable device(s)

Device 0: "Tesla V100-SXM2-32GB"
  CUDA Driver Version / Runtime Version          11.2 / 11.2
  CUDA Capability Major/Minor version number:    7.0
  Total amount of global memory:                 32510 MBytes (34089730048 bytes)
  (80) Multiprocessors, ( 64) CUDA Cores/MP:     5120 CUDA Cores
  GPU Max Clock rate:                            1530 MHz (1.53 GHz)
  Memory Clock rate:                             877 Mhz
  Memory Bus Width:                              4096-bit
  L2 Cache Size:                                 6291456 bytes
  Maximum Texture Dimension Size (x,y,z)         1D=(131072), 2D=(131072, 65536), 3D=(16384, 16384, 16384)
  Maximum Layered 1D Texture Size, (num) layers  1D=(32768), 2048 layers
  Maximum Layered 2D Texture Size, (num) layers  2D=(32768, 32768), 2048 layers
  Total amount of constant memory:               65536 bytes
  Total amount of shared memory per block:       49152 bytes
  Total shared memory per multiprocessor:        98304 bytes
  Total number of registers available per block: 65536
  Warp size:                                     32
  Maximum number of threads per multiprocessor:  2048
  Maximum number of threads per block:           1024
  Max dimension size of a thread block (x,y,z): (1024, 1024, 64)
  Max dimension size of a grid size    (x,y,z): (2147483647, 65535, 65535)
  Maximum memory pitch:                          2147483647 bytes
  Texture alignment:                             512 bytes
  Concurrent copy and kernel execution:          Yes with 5 copy engine(s)
  Run time limit on kernels:                     No
  Integrated GPU sharing Host Memory:            No
  Support host page-locked memory mapping:       Yes
  Alignment requirement for Surfaces:            Yes
  Device has ECC support:                        Enabled
  Device supports Unified Addressing (UVA):      Yes
  Device supports Managed Memory:                Yes
  Device supports Compute Preemption:            Yes
  Supports Cooperative Kernel Launch:            Yes
  Supports MultiDevice Co-op Kernel Launch:      Yes
  Device PCI Domain ID / Bus ID / location ID:   0 / 97 / 0
  Compute Mode:
     < Exclusive Process (many threads in one process is able to use ::cudaSetDevice() with this device) >
```


Some of you are sharing the same machine and some time measurements can be influenced by other users running at the very same moment. It can be necessary to run time measurements multiple times.

Next, download the alpaka library through `git`:

```bash
$ git clone -b 0.7.0 https://github.com/alpaka-group/alpaka.git
```

### alpaka-specific notes

In these exercises we are using alpaka in standalone mode (i.e., without any support of a build system). This requires special care with
regard to header files and compiler flags:

1. You need to include the standalone header file for the back-end you want to use. In this tutorial, this is always
   `#include <alpaka/standalone/GpuCudaRt.hpp>`.
2. You must tell `nvcc` where to find the alpaka header files you downloaded. This is done with the following compiler flag:
   `-I/path/to/your/copy/of/alpaka/include`. Note the trailing `/include` - this is a subdirectory of the alpaka directory
   you downloaded. It is important that `/include` is not omitted!
3. You also need to tell `nvcc` where to find the boost header files: add `-I ${BOOST_ROOT}/include` to the command line.
4. `nvcc` will produce a lot of (harmless) warnings when encountering the alpaka header files. Silence these warnings by passing this
   additional flag on the compiler command line: `-Xcudafe=--diag_suppress=esa_on_defaulted_function_ignored`.
4. The compiler command line should look similar to this:
   ```bash
   $ nvcc -std=c++14 -arch sm_70 -Ialpaka/include/ -I$BOOST_ROOT/include/ --expt-relaxed-constexpr --expt-extended-lambda
   ```

### Exercise 1. Modifying the computePi problem size.

In this exercise you will play around with the problem size of the computePi example. Change it to any value you like, but stay with
powers of 2 for now. How does this change affect the runtime of the program? How is the precision of the result changing?

### Exercise 2. Modifying the computePi kernel.

In its current version the kernel is restricted to powers of 2 in both problem size and the block size. By using what
you learned in both the CUDA session and the alpaka session modify the kernel in the following way:

1. It should work for any number of threads and blocks.
2. It should work for any problem size.

Hints:

* The workload has to be distributed between all threads in the grid.
* It is required to have a loop over some data elements inside the kernel. See the AXPY example in the slides for a hint on how to do this.
* It is possible to employ a technique called "grid-stride loop" for easy scalability. See
  [this](https://developer.nvidia.com/blog/cuda-pro-tip-write-flexible-kernels-grid-stride-loops/) NVIDIA blog entry on how to achieve this
  with CUDA.

### Challenge: Implementing AXPY from scratch 

In the alpaka lecture the classic AXPY algorithm has been used to illustrate the alpaka workflow. Now it is your turn to implement a working
alpaka-based version of the AXPY algorithm by writing both the kernel and the host program from scratch.

