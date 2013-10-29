#ifndef _BLOCKING_QUEUE_H_
#define _BLOCKING_QUEUE_H_

#include "base/Mutex.h"
#include "base/Condition.h"

#include <boost/noncopyable.hpp>
#include <deque>
#include <assert.h>

namespace base
{

template<typename T>
class BlockingQueue : boost::noncopyable 
{
public:
  BlockingQueue() 
    : mutex_(),
      cond_(mutex_),
      deque_()
  { }

  void put(const T& x)
  {
    MutexLockGuard lock(mutex_);
    deque_.push_back(x);
    cond_.notify();
  }

  T take()
  {
    MutexLockGuard lock(mutex_);
    while (deque_.empty()) {
      cond_.wait();
    }
    assert(!deque_.empty());
    T front(deque_.front());
    deque_.pop_front();
    return front;
  }

  size_t size() const
  {
    MutexLockGuard lock(mutex_);
    return deque_.size();
  }
private:
  mutable MutexLock mutex_;
  Condition cond_;
  std::deque<T> deque_;
};

}
#endif
