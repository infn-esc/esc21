#include <cstdint>
#include <cmath>
#include <limits>
#include <algorithm>
#include <cstring>
#include <array>


namespace approx_math {
  union binary32 {
    binary32() : ui32(0) {};
    binary32(float ff) : f(ff) {};
    binary32(int32_t ii) : i32(ii){}
    binary32(uint32_t ui) : ui32(ui){}
    
    uint32_t ui32; /* unsigned int */                
    int32_t i32; /* Signed int */                
    float f;
  };
}
/*  Quick and dirty, branchless, log implementations
    Author: Florent de Dinechin, Aric, ENS-Lyon 
    All right reserved
*/
template<int DEGREE>
inline float approx_logf_P(float p);
// degree =  5   => absolute accuracy is  16 bits
template<>
inline float approx_logf_P<5>(float y) {
  return  y * (float(0xf.ff652p-4) + y * (-float(0x8.0048ap-4) + y * (float(0x5.72782p-4) + y * (-float(0x4.20904p-4) + y * float(0x2.1d7fd8p-4))))) ;
}
// degree =  8   => absolute accuracy is  24 bits
template<>
inline float approx_logf_P<8>(float y) {
   return  y * ( float(0x1.00000cp0) + y * (float(-0x8.0003p-4) + y * (float(0x5.55087p-4) + y * ( float(-0x3.fedcep-4) + y * (float(0x3.3a1dap-4) + y * (float(-0x2.cb55fp-4) + y * (float(0x2.38831p-4) + y * (float(-0xf.e87cap-8) )))))))) ;
}
template<int DEGREE>
inline float unsafe_logf_impl(float x) {
  using namespace approx_math;

  binary32 xx,m;
  xx.f = x;
  
  // as many integer computations as possible, most are 1-cycle only, and lots of ILP.
  int e= (((xx.i32) >> 23) & 0xFF) -127; // extract exponent
  m.i32 = (xx.i32 & 0x007FFFFF) | 0x3F800000; // extract mantissa as an FP number
  
  int adjust = (xx.i32>>22)&1; // first bit of the mantissa, tells us if 1.m > 1.5
  m.i32 -= adjust << 23; // if so, divide 1.m by 2 (exact operation, no rounding)
  e += adjust;           // and update exponent so we still have x=2^E*y
  
  // now back to floating-point
  float y = m.f -1.0f; // Sterbenz-exact; cancels but we don't care about output relative error
  // all the computations so far were free of rounding errors...

  // the following is based on Sollya output
  float p = approx_logf_P<DEGREE>(y);
  

  constexpr float Log2=0xb.17218p-4; // 0.693147182464599609375 
  return float(e)*Log2+p;

}

template<int DEGREE>
inline float unsafe_logf(float x) {
  return unsafe_logf_impl<DEGREE>(x);
}





#define likely(x) (__builtin_expect(x, true))



inline 
float erfinv_like(float w) {
  w = w - 2.500000f;
  float p = 2.81022636e-08f;
  p = 3.43273939e-07f + p*w;
  p = -3.5233877e-06f + p*w;
  p = -4.39150654e-06f + p*w;
  p = 0.00021858087f + p*w;
  p = -0.00125372503f + p*w;
  p = -0.00417768164f + p*w;
  p = 0.246640727f + p*w;
  p = 1.50140941f + p*w;
  return p;
}

inline 
float erfinv_unlike(float w) {
  w = std::sqrt(w) - 3.000000f;
  float p = -0.000200214257f;
  p = 0.000100950558f + p*w;
  p = 0.00134934322f + p*w;
  p = -0.00367342844f + p*w;
  p = 0.00573950773f + p*w;
  p = -0.0076224613f + p*w;
  p = 0.00943887047f + p*w;
  p = 1.00167406f + p*w;
  p = 2.83297682f + p*w;
  return p;
}



inline float erfinv(float x) {
  float w, p;
  w = -  unsafe_logf<8>((1.0f-x)*(1.0f+x));
  if likely( w < 5.0f )
    p =   erfinv_like(w);
  else 
    p = erfinv_unlike(w);
  return p*x;
}


inline
void erfinV(float * __restrict__ x, float const *  __restrict__ ur, uint32_t N) {
  float * b = (float *)(__builtin_assume_aligned (x, 32));
  float const * r = (float const *)(__builtin_assume_aligned (ur, 32));
  
  bool t[N];
  for (int i=0;i<N;++i) {
    float w = -  unsafe_logf<8>((1.0f-r[i])*(1.0f+r[i]));
    t[i] = w > 5.0f;
    b[i] = std::sqrt(2.f)*r[i]*erfinv_like(w);
  }
  
  for (int i=0;i<N;++i) {
    if(t[i]) {
      float w = -  unsafe_logf<8>((1.0f-r[i])*(1.0f+r[i]));
      b[i] = std::sqrt(2.f)*r[i]*erfinv_unlike(w);
    }
  }
}


