/* Copyright 2019-2021 Benjamin Worpitz, Erik Zenker, Jan Stephan,
 *                     Sergei Bastrakov
 *
 * This file exemplifies usage of Alpaka.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED “AS IS” AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL ISC BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <alpaka/standalone/GpuCudaRt.hpp>
#include <alpaka/alpaka.hpp>

#include <cstdint>
#include <iostream>
#include <random>

// Structure with memory buffers for inputs (x, y) and
// outputs (inside) of the kernel
struct Points {
    float * x;
    float * y;
    bool * inside;
};

// Alpaka kernel defines operations to be executed concurrently on a device
// It is a C++ functor: the entry point is operator()
struct PixelFinderKernel {
    // Alpaka accelerator is the required first parameter for all kernels.
    // It is provided by alpaka automatically.
    // For portability its type is a template parameter.
    // ALPAKA_FN_ACC prefix is required for all functions that run on device.
    // Kernels are required to be const and return void
    template<typename Acc>
    ALPAKA_FN_ACC void operator()(Acc const & acc,
        Points points, float r) const {
        // This function body will be executed by all threads concurrently
        using namespace alpaka;

        // Thread index in the grid (among all threads)
        std::uint32_t gridThreadIdx = getIdx<Grid, Threads>(acc)[0];

        // Read inputs for the current threads to work on
        // For simplicity we assume the total number of threads
        // is equal to the number of points
        float x = points.x[gridThreadIdx];
        float y = points.y[gridThreadIdx];

        // Note acc parameter to sqrt, same for other math functions
        float d = sqrt(acc, x * x + y * y);

        // Compute and write output
        bool isInside = (d <= r);
        points.inside[gridThreadIdx] = isInside;

        /* Exercise 2: Modify this kernel to make it more generic in terms of problem size and
           work division among grid, blocks and threads. */
    }
};

int main() {
    // For code brevity, all alpaka API is in namespace alpaka
    using namespace alpaka;

    // Define dimensionality and type of indices to be used in kernels
    using Dim = DimInt<1>;
    using Idx = std::uint32_t;

    // Define alpaka accelerator type, which corresponds to the underlying programming model
    using Acc = AccGpuCudaRt<Dim, Idx>;
    // Other options instead of AccCpuOmp2Blocks are
    // - AccGpuCudaRt
    // - AccCpuThreads
    // - AccCpuFibers
    // - AccCpuOmp2Threads
    // - AccCpuOmp4
    // - AccCpuTbbBlocks
    // - AccCpuSerial

    // Select the first device available on a system, for the chosen accelerator
    auto const device = getDevByIdx<Acc>(0u);

    // Define type for a queue with requested properties:
    // in this example we require the queue to be blocking the host side
    // while operations on the device (kernels, memory transfers) are running
    using MyQueue = Queue<Acc, property::Blocking>;
    // Create a queue for the device
    auto queue = MyQueue{device};

    /* Exercise 1: Change the value of n. Stay with powers of 2 for now.
       How does this affect the runtime and the precision of pi? */
    // Number of points
    std::uint32_t n = 1 << 18;

    // Circle radius
    float r = 10.0f;

    // Create a device for host for memory allocation, using the first CPU available
    auto devHost = getDevByIdx<DevCpu>(0u);

    // Allocate memory on the host side:
    // the first template parameter is data type of buffer elements,
    // the second is internal indexing type
    Vec<Dim, Idx> bufferExtent{n};
    auto xBufferHost = allocBuf<float, Idx>(devHost, bufferExtent);
    auto yBufferHost = allocBuf<float, Idx>(devHost, bufferExtent);
    auto insideBufferHost = allocBuf<bool, Idx>(devHost, bufferExtent);

    // Get raw pointers to memory buffers on host and put into a structure
    Points pointsHost;
    pointsHost.x = getPtrNative(xBufferHost);
    pointsHost.y = getPtrNative(yBufferHost);
    pointsHost.inside = getPtrNative(insideBufferHost);

    // Generate input x, y randomly in [0, r]
    std::random_device rd;
    std::mt19937 generator{rd()};
    std::uniform_real_distribution<float> distribution(0.0f, r);
    for (auto idx = 0u; idx < n; idx++)
    {
        pointsHost.x[idx] = distribution(generator);
        pointsHost.y[idx] = distribution(generator);
    }

    // Allocate memory on the device side,
    // note symmetry to host
    auto xBufferAcc = allocBuf<float, Idx>(device, bufferExtent);
    auto yBufferAcc = allocBuf<float, Idx>(device, bufferExtent);
    auto insideBufferAcc = allocBuf<bool, Idx>(device, bufferExtent);

    // Get raw pointers to memory buffers device host and put into a structure,
    // note symmetry to host
    Points pointsAcc;
    pointsAcc.x = getPtrNative(xBufferAcc);
    pointsAcc.y = getPtrNative(yBufferAcc);
    pointsAcc.inside = getPtrNative(insideBufferAcc);

    // Start time measurement
    auto start = std::chrono::steady_clock::now();

    // Copy x, y buffers from host to device
    memcpy(queue, xBufferAcc, xBufferHost, bufferExtent);
    memcpy(queue, yBufferAcc, yBufferHost, bufferExtent);

    // Define kernel execution configuration of blocks,
    // threads per block, and elements per thread
    std::uint32_t threadsPerBlock = 256;
    std::uint32_t blocksPerGrid = n / threadsPerBlock;
    std::uint32_t elementsPerThread = 1;
    using WorkDiv = WorkDivMembers<Dim, Idx>;
    auto workDiv = WorkDiv{blocksPerGrid, threadsPerBlock, elementsPerThread};

    // Instantiate the kernel object
    PixelFinderKernel pixelFinderKernel;
    // Create a task to run the kernel with the given work division;
    // creating a task does not put it for execution
    auto taskRunKernel = createTaskKernel<Acc>(workDiv, pixelFinderKernel, pointsAcc, r);

    // Enqueue the kernel execution task.
    // The kernel's operator() will be run concurrently
    // on the device associated with the queue.
    enqueue(queue, taskRunKernel);

    // Copy inside buffer from device to host
    memcpy(queue, insideBufferHost, insideBufferAcc, bufferExtent);

    // Wait until all operations in the queue are finished.
    // This call is redundant for a blocking queue
    // Here use alpaka:: because of an issue on macOS
    alpaka::wait(queue);

    // Compute Pi on host
    std::uint32_t P = 0;
    for (std::uint32_t i = 0; i < n; ++i)
    {
        if (pointsHost.inside[i])
            ++P;
    }
    float pi = 4.f * P / n;

    // Finish time measurements
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    // Output results
    std::cout << "Computed pi is " << pi << "\n";
    std::cout << "Execution time: " << duration.count() << " ms" << std::endl;

    return 0;
}

