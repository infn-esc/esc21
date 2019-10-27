#ifndef memory_usage_h
#define memory_usage_h

#include <cstdint>
#include <iosfwd>

namespace memory_usage {
  bool     is_available();
  uint64_t allocated();
  uint64_t deallocated();
  uint64_t totlive();

 struct statm {
   long long vss;
   long long rss;
   long long shared;
   long long text;
   long long data;

   void fill();
   std::ostream & print(std::ostream & co) const;
 };

};

#endif // memory_usage_h
