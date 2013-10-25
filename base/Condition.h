#ifndef _CONDITION_H_
#define _CONDITION_H_

#include <base/Mutex.h>

#include <boost/noncopyable.hpp>
#include <errno.h>

namespace base
{

class Condition : boost::noncopyable
{
public:
  explicit Condition(MutexLock& mutex)
    : mutex_(mutex)
  {
    pthread_cond_init(&cond_, NULL);
  }

  ~Condition()
  {
    pthread_cond_destroy(&cond_);
  }

  void wait()
  {
    pthread_cond_wait(&cond_, mutex_.getPthreadMutex());
  }

  bool waitForSeconds(int seconds)
  {
    struct timespec abstime;
    clock_gettime(CLOCK_REALTIME, &abstime);
    abstime.tv_sec += seconds;
    return ETIMEOUT == pthread_cond_timeout(&cond_, mutex_.getPthreadMutex(), &abstime);
  }

  void notify()
  {
    pthread_cond_signal(&cond_);
  }

  void notifyAll()
  {
    pthread_cond_broadcast(&cond_);
  }

private:
  MutexLock& mutex_;
  pthread_cond_t cond_;
};

}

#endif
