#include "ThreadPool.h"
#include <stdio.h>
#include <boost/bind.hpp>
#include <algorithm>

using namespace base;

ThreadPool::ThreadPool(const std::string& name)
  : mutex_(),
    cond_(mutex_),
    name_(name),
    running_(false)
{ }

ThreadPool::~ThreadPool()
{
  if (running_)
    stop();
}

void ThreadPool::start(int numThreads) 
{
  assert(threads_.empty());

  running_ = true;
  threads_.reserve(numThreads);
  
  for (int i = 0; i < numThreads; i++) {
    char id[32];
    snprintf(id, sizeof(id), "%d", i + 1);
    threads_.push_back(new base::Thread(
                        boost::bind(&ThreadPool::runInThread, this),
                        name_ + id));
  }
  std::for_each(threads_.begin(), threads_.end(), 
                boost::bind(&Thread::start, _1));
}

void ThreadPool::run(const Task& task)
{
  if (threads_.empty()) {
    task();
  } else {
    MutexLockGuard lock(mutex_);
    queue_.push_back(task);
    cond_.notify();
  }
}

ThreadPool::Task ThreadPool::take()
{
  MutexLockGuard lock(mutex_);
  while (queue_.empty() && running_)
    cond_.wait(); 
  Task task;
  if (!queue_.empty()) {
    task = queue_.front();
    queue_.pop_front();
  }
  return task;
}

void ThreadPool::runInThread()
{
  while (running_) {
    Task task(take());
    if (task)
      task();
  }
}

void ThreadPool::stop()
{
  running_ = false;
  cond_.notifyAll();
  std::for_each(threads_.begin(), threads_.end(),
                boost::bind(&Thread::start, _1));
}
