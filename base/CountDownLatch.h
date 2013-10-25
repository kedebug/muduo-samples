#ifndef _COUNT_DOWN_LATCH_H_
#define _COUNT_DOWN_LATCH_H_

#include <base/Mutex.h>
#include <base/Condition.h>

#include <boost/noncopyable.hpp>

namespace base
{

class CountDownLatch : boost::noncopyable
{
public:
  explicit CountDownLatch(int count) 
    : mutex_(),
      cond_(mutex_),
      count_(count)
  { }

  void countDown()
  {
    MutexLockGuard lock(mutex_);
    --count;
    if (count == 0)
      cond_.notifyAll();
  }

  void wait()
  {
    MutexLockGuard lock(mutex_);
    while (count > 0)
      cond_.wait();
  }

  int getCount() const 
  {
    MutexLockGuard lock(mutex_);
    return count_;
  }
private:
  mutable MutexLock mutex_;
  Condition cond_;
  int count_;
};

}

#endif
