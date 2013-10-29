#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include "base/Condition.h"
#include "base/Mutex.h"
#include "base/Thread.h"

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <deque>

namespace base {

class ThreadPool : boost::noncopyable {
public:
  typedef boost::function<void ()> Task; 
  
  explicit ThreadPool(const std::string& name = std::string());
  ~ThreadPool();

  void run(const Task& f);
  void start(int numThreads);
  void stop();

private:
  void runInThread();
  Task take();

  MutexLock mutex_;
  Condition cond_;
  std::string name_;
  boost::ptr_vector<base::Thread> threads_;
  std::deque<Task> queue_;
  bool running_;
};  // ThreadPool class


}   // namespace base

#endif
