#include <base/Thread.h>
#include <base/Mutex.h>
#include <set>
#include <stdio.h>

class Request;

class Inventory
{
public:
  void add(Request* req)
  {
    base::MutexLockGuard lock(mutex_);
    requests_.insert(req);
  }

  void remove(Request* req)
  {
    base::MutexLockGuard lock(mutex_);
    requests_.erase(req);
  }

  void printAll() const;

private:
  mutable base::MutexLock mutex_;
  std::set<Request*> requests_;
};

Inventory g_inventory;

class Request
{
public:
  void process()
  {
    base::MutexLockGuard lock(mutex_);
    g_inventory.add(this);
  }
  ~Request()
  {
    base::MutexLockGuard lock(mutex_);
    sleep(1);
    g_inventory.remove(this);
  }
  void print()
  {
    base::MutexLockGuard lock(mutex_);
    printf("Request[%p]\n", this);
  }
private:
  base::MutexLock mutex_;
};

void Inventory::printAll() const
{
  base::MutexLockGuard lock(mutex_);
  sleep(1);
  for (std::set<Request*>::const_iterator it = requests_.begin();
       it != requests_.end();
       it++)
  {
    (*it)->print();
  }
}

void threadFunc()
{
  Request* req = new Request;
  req->process();
  delete req;
}

int main()
{
  base::Thread t(threadFunc);
  t.start();
  usleep(500 * 1000);
  g_inventory.printAll();
  t.join();
}
