//  cat /proc/meminfo | grep -i anon
//  ps -eo pid,command,rss,vsz | grep a.out
// strace -e trace=memory ./a.out
#include<iostream>
#include<cstdint>
#include<vector>
#include<array>
#include<memory>
#include<chrono>

#include "memory_usage.h"

auto start = std::chrono::high_resolution_clock::now();

void stop(const char * m) {
  auto delta = std::chrono::high_resolution_clock::now()-start;
  std::cout.imbue(std::locale("en_US.UTF8"));
  std::cout << m;
  std::cout << " elapsted time " << std::chrono::duration_cast<std::chrono::nanoseconds>(delta).count() << std::endl;
  std::cout << "allocated so far " << memory_usage::allocated();
  std::cout << " deallocated so far " << memory_usage::deallocated() << " bytes" << std::endl;
  std::cout << "total live " << memory_usage::totlive() << " bytes" << std::endl;
  memory_usage::statm statm; statm.fill();
  std::cout << "statm "; statm.print(std::cout) << std::endl;
  char c;
  std::cout << "continue?";
  std::cin  >> c;
  std::cout.imbue(std::locale());
  start = std::chrono::high_resolution_clock::now();
}


void __attribute__ ((noinline))
touch(int * v, uint32_t size) {

  auto stride = std::max(1U,size/100);
  std::cout << "access elements each " << stride << std::endl;
  auto e = v+size;
  for(;v<e; v+=stride) (*v)=1;

}


void cArray(size_t N) {  

//    auto v = std::make_unique<int[]>(N);
    auto v = new int[N];
    stop("carray: after create");
    v[0]=1;
    stop("carray: after assign element 0");
    touch(v,N);
    stop("carray: after touch");

    delete [] v;
}


template<typename T>
struct W {
  W(){}
  W(T t):v(t){}
  W & operator=(T const & t) { v=t;}
  T v;
};


void cppVectorResize(size_t N) { 

    std::vector<int> v;
    v.reserve(N);
    std::cout << "size,capacity " << v.size() << ' ' << v.capacity() << std::endl;
    stop("cppVector after reserve");
    v.resize(N);
    std::cout << "size,capacity " << v.size() << ' ' << v.capacity() << std::endl;
    stop("cppVector after resize");
    v[0]=1;
    stop("cppVector after assign 0");
    touch(v.data(),N);
    stop("cppVector after touch");
    std::cout << "size,capacity " << v.size() << ' ' << v.capacity() << std::endl;
 
}


void cppVectorFill(size_t N) { 

    std::vector<int> v;
    // v.reserve(N);
    std::cout << "size,capacity " << v.size() << ' ' << v.capacity() << std::endl;
    stop("cppVector after reserve");
    for (size_t i=0; i<N; i+=N/10000) v.push_back(i); 
    std::cout << "size,capacity " << v.size() << ' ' << v.capacity() << std::endl;
    stop("cppVector after push_back");

}




int main() {

  // set locale to print large numbers with commas
  std::locale::global(std::locale("en_US.UTF8"));
  std::cout.imbue(std::locale());

  std::cout << "jemalloc counters are " << (memory_usage::is_available() ? "" : "NOT ") << "available" << std::endl;

  stop("start");

  constexpr size_t N = 200*1000*1000;

  cArray(N/2);
  stop("after cArray");
  cppVectorFill(N);
  stop("after cppFill");
  cppVectorResize(N);
  stop("after cppVectorResize");
  
  stop("stop");

  return 0;
}
