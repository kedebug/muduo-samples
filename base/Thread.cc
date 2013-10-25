#include <base/Thread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <boost/weak_ptr.hpp>

namespace base
{
namespace CurrentThread
{
  __thread const char* t_threadName = "unknown";
}
}

namespace 
{
__thread pid_t t_cachedTid = 0;

pid_t gettid()
{
  return static_cast<pid_t>(::syscall(SYS_gettid));
}

struct ThreadData
{
  typedef base::Thread::ThreadFunc ThreadFunc;

  ThreadFunc func_;
  std::string name_;
  boost::weak_ptr<pid_t> wkTid_;

  ThreadData(const ThreadFunc& func,
             const std::string& name,
             const boost::shared_ptr<pid_t>& tid)
    : func_(func),
      name_(name),
      wkTid_(tid)
  { }

  void runInThread()
  {
    pid_t tid = base::CurrentThread::tid();
    boost::shared_ptr<pid_t> ptid = wkTid_.lock();

    if (ptid) {
      *ptid = tid;
      ptid.reset();
    }

    base::CurrentThread::t_threadName = name_.c_str();
    func_();
    base::CurrentThread::t_threadName = "main";
  }
};

void* startThread(void* obj)
{
  ThreadData* data = static_cast<ThreadData*>(obj);
  data->runInThread();
  delete data;
  return NULL;
}

}

pid_t base::CurrentThread::tid()
{
  if (t_cachedTid == 0) {
    t_cachedTid = gettid();
  }
  return t_cachedTid;
}

const char* base::CurrentThread::name()
{
  return t_threadName;
}

bool base::CurrentThread::isMainThread()
{
  return tid() == ::getpid();
}

using namespace base;

AtomicInt32 Thread::numCreated_;

Thread::Thread(const ThreadFunc& func, const std::string& name)
  : started_(false),
    joined_(false),
    pthreadId_(0),
    tid_(new pid_t(0)),
    func_(func),
    name_(name)
{
  numCreated_.increment();
}

Thread::~Thread()
{
  if (started_ && !joined_) {
    pthread_detach(pthreadId_);
  }
}

void Thread::start()
{
  assert(!started_);
  started_ = true;

  ThreadData* data = new ThreadData(func_, name_, tid_);
  if (pthread_create(&pthreadId_, NULL, &startThread, data)) {
    started_ = false;
    delete data;
    abort();
  }
}

void Thread::join()
{
  assert(started_);
  assert(!joined_);
  joined_ = true;
  pthread_join(pthreadId_, NULL);
}
