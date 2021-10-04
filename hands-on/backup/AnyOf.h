#ifndef ANYOF_H
#define ANYOF_H
//
//  a simple class to store in place Objects from types derived from a common base type
//  multiple inheritance is not supported
//  relies on the base type to have a virtual destructor
//  It does not have any additional memory or time overhead w/r/t in place new and static_cast
//
//
//

#include<cmath>
#include<tuple>
#include<functional>
#include<type_traits>
#include <cstring>
#include <cassert>


namespace any_details {

  template<typename T>
  inline constexpr T vmax(T a, T b) { return (a>b) ? a : b;}
  
  template<class TupleType, int N=std::tuple_size<TupleType>::value>
  struct maxSizeT {
    typedef typename std::tuple_element<N-1,TupleType>::type Elem;
    enum { valueSize = vmax(sizeof(Elem), (long unsigned int)(maxSizeT<TupleType,N-1>::valueSize)),
	   valueAlign = vmax(alignof(Elem), (long unsigned int)(maxSizeT<TupleType,N-1>::valueAlign)) };
  };
  
  template<class TupleType>
  struct maxSizeT<TupleType, 0>  {
    enum  { valueSize = sizeof(typename std::tuple_element<0,TupleType>::type),
	    valueAlign = alignof(typename std::tuple_element<0,TupleType>::type)};
  };
  

  template <typename TupleType, typename S, int i = std::tuple_size<TupleType>::value - 1>
  struct tuple_index
  {
    enum
      {
        value = std::is_same<typename std::tuple_element<i, TupleType>::type, S>::value ?
	i :
	tuple_index<TupleType, S, i-1>::value
      };
  };
  
  template <typename TupleType, typename S>
  struct tuple_index<TupleType, S, -1>
  {
    enum { value = -1 };
  };
  
  template<class Head, typename ... Tail>
  struct maxSize {
    enum { valueSize = vmax(sizeof(Head), (long unsigned int)(maxSize<Tail...>::valueSize)),
	   valueAlign = vmax(alignof(Head), (long unsigned int)(maxSize<Tail...>::valueAlign)),
    };
  };
  
  template<class T>
  struct maxSize<T>  {
    enum  { valueSize = sizeof(T),valueAlign = alignof(T) };
  };
  
}
  
template<typename P, typename...C>
struct AnyOf {
  // using aligned_union_t = typename std::aligned_union<4,C...>::type;
  enum { size = any_details::maxSize<C...>::valueSize, align = any_details::maxSize<C...>::valueAlign};
  using aligned_union_t = typename std::aligned_storage<size,align>::type;


  bool empty() const { void * vtp;  memcpy(&vtp,&mem,sizeof(vtp)); return nullptr==vtp;}
  void zeroit() { void * vtp=nullptr; memcpy(&mem,&vtp,sizeof(vtp));}

  AnyOf() {zeroit();}

  /* does not teally work
  template<typename T>
  struct Tag { using type=T;};  
  template<typename T, typename ... Args>
  AnyOf(T, Args&&... args){
    new(get()) typename T::type(std::forward<Args...>(args...));
  }
  */  

  void destroy() { if(!empty()) get()->~P();}
  template<typename T> 
  explicit AnyOf(T const & t) noexcept  {
    new(get()) T(t); 
  }

  template<typename T> 
  explicit AnyOf(T && t)  noexcept {
    new(get()) T(std::move(t)); 
  }
  template<typename T> 
  void reset(T const & t) noexcept {
    destroy();
    new(get()) T(t);
  }
  template<typename T> 
  void reset(T && t) noexcept {
    destroy();
    new(get()) T(std::move(t));
  }
  
  AnyOf(AnyOf&&rh) noexcept : mem(std::move(rh.mem)) { rh.zeroit();}

  AnyOf& operator=(AnyOf&&rh) noexcept {
    if ((&rh)==this) return *this;
    destroy();
    mem= std::move(rh.mem);
    rh.zeroit();
    return *this;
  }
  
  ~AnyOf() { destroy(); }
  AnyOf(AnyOf const&) = delete;
  AnyOf& operator=(AnyOf const&) = delete; 


  template<typename T=P> 
  T * get() { return (T*)&mem;}
  template<typename T=P> 
  T const * get() const { return (T const*)&mem;}
  P & operator()() { return *get(); }
  P const & operator()() const { return *get();}
  aligned_union_t mem;
};


#endif
