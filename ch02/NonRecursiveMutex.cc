#include <base/Mutex.h>
#include <vector>

class Foo
{
public:
  void doit() const;
};

base::MutexLock mutex;
std::vector<Foo> foos;

void post(const Foo& f)
{
  base::MutexLockGuard lock(mutex);
  foos.push_back(f);
}

void traverse()
{
  std::vector<Foo>::const_iterator it;
  base::MutexLockGuard lock(mutex);
  for (it = foos.begin(); it != foos.end(); it++) {
    it->doit();
  }
}

void Foo::doit() const
{
  Foo f;
  post(f);
}

int main()
{
  Foo f;
  post(f);
  traverse();
}
