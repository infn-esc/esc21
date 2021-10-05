---
title: Introduction to CUDA
layout: main
section: parallelism
---


The CUDA Runtime API reference manual is a very useful source of information:
<a href="http://docs.nvidia.com/cuda/cuda-runtime-api/index.html" target="_blank">http://docs.nvidia.com/cuda/cuda-runtime-api/index.html</a>

```bash
$ cd esc21/hands-on/cuda_exercises
```


Check that your environment is correctly configured to compile CUDA code by running:
```bash
module load compilers/gcc-9.2.0_sl7
export PATH=/usr/local/cuda-11.2/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/cuda-11.2/lib64:$LD_LIBRARY_PATH
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


### Exercise 1. CUDA Memory Model
In this exercise you will learn what heterogeneous memory model means, by demonstrating the difference between host and device memory spaces.
1. Allocate device memory;
2. Copy the host array h_a to d_a on the device;
3. Copy the device array d_a to the device array d_b;
4. Copy the device array d_b to the host array h_a;
5. Free the memory allocated for d_a and d_b.
6. Compile and run the program by running:

```bash
$ nvcc cuda_mem_model.cu -o ex01
$ ./ex01
```

* Bonus: Measure the PCI Express bandwidth.

### Exercise 2. Launch a kernel
By completing this exercise you will learn how to configure and launch a simple CUDA kernel.

1. Allocate device memory;
2. Configure the kernel to run using a one-dimensional grid of one-dimensional blocks;
3. Each GPU thread should set one element of the array to:

   `d_a[i] = blockIdx.x + threadIdx.x + 42;`
4. Copy the results to the host memory;
5. Check the correctness of the results

### Exercise 3. Two-dimensional grid
M is a matrix of NxN integers.
1. Set N=4
2. Write a kernel that sets each element of the matrix to its linear index (e.g. M[2,3] = 2*N + 3), by making use of two-dimensional grid and blocks. (Two-dimensional means using the x and y coordinates).
3. Copy the result to the host and check that it is correct.
4. Try with a rectangular matrix 19x67. Hint: check the kernel launch parameters.


### Exercise 4. Measuring throughput and profiling with NVVP
The throughput of a kernel can be defined as the number of bytes read and written by a kernel in the unit of time.

The CUDA event API includes calls to create and destroy events, record events, and compute the elapsed time in milliseconds between two recorded events.

CUDA events make use of the concept of CUDA streams. A CUDA stream is simply a sequence of operations that are performed in order on the device. Operations in different streams can be interleaved and in some cases overlapped, a property that can be used to hide data transfers between the host and the device. Up to now, all operations on the GPU have occurred in the default stream, or stream 0 (also called the "Null Stream").

The peak theoretical throughput can be evaluated as well: if your device comes with a memory clock rate of 1GHz DDR (double data rate) and a 256-bit wide memory interface, the peak theoretical throughput can be computed with the following:

Throughput (GB/s)= Memory_rate(Hz) * memory_interface_width(byte) * 2 /10<sup>9</sup>

1. Compute the theoretical peak throughput of the device you are using.
2. Modify ex04.cu to give the measurement of actual throughput of the kernel.
3. Measure the throughput with a varying number of elements (in logarithmic scale). Before doing that write down what do you expect (you can also draw a diagram).
4. What did you find out? Can you give an explanation?
5. NVIDIA Visual Profiler can deliver vital feedback for optimizing your CUDA applications.
Run it and analyze ex04.

```bash
nvvp &
```

### Exercise 5. Parallel Reduction
Given an array `a[N]`, the reduction sum `Sum` of a is the sum of all its elements: `Sum=a[0]+a[1]+...a[N-1]`.
1. Implement a block-wise parallel reduction (using the shared memory).
2. For each block, save the partial sum.
3. Sum all the partial sums together.
4. Check the result comparing with the host result.
5. Measure the throughput of your reduction kernel using CUDA Events (see exercise 4)
6. Analyze your application using `nvvp`. Do you think it can be improved? How?
* Bonus: Can you implement a one-step reduction? Measure and compare the throughput of the two versions.
* Challenge: The cumulative sum of an array `a[N]` is another array `b[N]`, the sum of prefixes of `a`:
`b[i] = a[0] + a[1] + … + a[i]`. Implement a cumulative sum kernel assuming that the size of the input array is multiple of the block size.

### Challenge: Histogram

The purpose of this lab is to implement an efficient histogramming algorithm for an input array of integers within a given range.

Each integer will map into a single bin, so the values will range from 0 to (NUM_BINS - 1).

The histogram bins will use unsigned 32-bit counters that must be saturated at 127 (i.e. no roll back to 0 allowed).

The input length can be assumed to be less than 2ˆ32. `NUM_BINS` is fixed at 4096 for this lab.
This can be split into two kernels: one that does a histogram without saturation, and a final kernel that cleans up the bins if they are too large. These two stages can also be combined into a single kernel.

### Utility. Measuring time using CUDA Events
```C++
cudaEvent_t start, stop; float time;
cudaEventCreate(&start);  cudaEventCreate(&stop);
cudaEventRecord(start, 0);
square_array <<< n_blocks, block_size >>> (a_d, N);
cudaEventRecord(stop, 0);
cudaEventSynchronize(stop);
cudaEventElapsedTime(&time, start, stop);
std::cout << "Time for the kernel: " << time << " ms" << std::endl;
```



### Atomics <a href="https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#atomic-functions" target="_blank">[1]</a>

An atomic function performs a read-modify-write atomic operation on one 32-bit or 64-bit word residing in global or shared memory.
The operation is atomic in the sense that it is guaranteed to be performed without interference from other threads.


```C++
int atomicAdd(int* address, int val);
unsigned int atomicAdd(unsigned int* address,
                       unsigned int val);
unsigned long long int atomicAdd(unsigned long long int* address,
                                 unsigned long long int val);
float atomicAdd(float* address, float val);
double atomicAdd(double* address, double val);
__half2 atomicAdd(__half2 *address, __half2 val);
__half atomicAdd(__half *address, __half val);
```

reads the 16-bit, 32-bit or 64-bit word old located at the address address in global or shared memory, computes (old + val), and stores the result back to memory at the same address. These three operations are performed in one atomic transaction. The function returns old.



