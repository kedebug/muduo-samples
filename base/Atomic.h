#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#include <boost/noncopyable.hpp>
#include <stdint.h>

namespace base
{

namespace detail
{

template<typename T>
class AtomicIntegerT : boost::noncopyable
{
public:
  AtomicIntegerT() : value_(0)
  {
  }

  T get() const
  {
    return __sync_val_compare_and_swap(const_cast<volatile T*>(&value_), 0, 0);
  }

  T getAndAdd(T x)
  {
    return __sync_fetch_and_add(&value_, x);
  }

  T addAndGet(T x)
  {
    return getAndAdd(x) + x;
  }

  T incrementAndGet()
  {
    return addAndGet(1);
  }

  void add(T x)
  {
    getAndAdd(x);
  }

  void increment()
  {
    incrementAndGet();
  }

  void decrement()
  {
    getAndAdd(-1);
  }

  T getAndSet(T newVal)
  {
    return __sync_lock_test_and_set(&value_, newVal);
  }
private:
  volatile T value_;
};

}

typedef detail::AtomicIntegerT<int32_t> AtomicInt32;
typedef detail::AtomicIntegerT<int64_t> AtomicInt64;
}

#endif
