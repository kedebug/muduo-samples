#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <boost/noncopyable.hpp>
#include <base/Thread.h>
#include <assert.h>
#include <pthread.h>

namespace base
{

class MutexLock : boost::noncopyable
{
public:
  MutexLock()
    : holder_(0)
  {
    pthread_mutex_init(&mutex_, NULL);
  }

  void lock()
  {
    pthread_mutex_lock(&mutex_);
    holder_ = CurrentThread::tid();
  }

  void unlock()
  {
    holder_ = 0;
    pthread_mutex_unlock(&mutex_);
  }

  bool isLockedByThisThread()
  {
    return holder_ == CurrentThread::tid();
  }

  void assertLocked()
  {
    assert(isLockedByThisThread());
  }

  pthread_mutex_t* getPthreadMutex()
  {
    return &mutex_;
  }

  ~MutexLock()
  {
    assert(holder_ == 0);
    pthread_mutex_destroy(&mutex_);
  }

private:
  pthread_mutex_t mutex_;
  pid_t holder_;
};

class MutexLockGuard : boost::noncopyable
{
public:
  explicit MutexLockGuard(MutexLock& mutex) : mutex_(mutex)
  {
    mutex_.lock();
  }
  
  ~MutexLockGuard()
  {
    mutex_.unlock();
  }
  
private:
  MutexLock& mutex_;
};

}

#endif
