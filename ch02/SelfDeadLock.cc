#include <base/Mutex.h>
#include <stdio.h>

class Request
{
public:
  void process() __attribute__ ((noinline))
  {
    printf("Request::process\n");
    base::MutexLockGuard lock(mutex_);
    print();
  }
  
  void print() const __attribute__ ((noinline))
  {
    printf("Request::print\n");
    base::MutexLockGuard lock(mutex_);
    printf("Request::print end\n");
  }
private:
  mutable base::MutexLock mutex_;
};  

int main()
{
  Request req;
  req.process();
}
