#include <base/mutex.h>
#include <iostream>

int main()
{
  base::MutexLock lock;
  base::MutexLockGuard mutex(lock);
  std::cout << "hello, world." << std::endl;
  return 0;
}
