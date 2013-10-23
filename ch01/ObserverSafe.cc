#include <stdio.h>
#include <algorithm>
#include <vector>
#include <base/Mutex.h>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

class Observable;
class Observer;

class Observer : public boost::enable_shared_from_this<Observer>
{
public:
  virtual ~Observer();
  virtual void update() = 0;
  void observe(Observable* s);

protected:
  Observable* subject_;
};

class Observable
{
public:
  void register_(boost::weak_ptr<Observer> x);

  void notifyObservers()
  {
    base::MutexLockGuard lock(mutex_);
    Iterator it = observers_.begin();
    while (it != observers_.end()) {
      boost::shared_ptr<Observer> p(it->lock());
      if (p) {
        ++it;
        p->update();
      } else {
        observers_.erase(it);
      }
    }
  }

private:
  mutable base::MutexLock mutex_;
  std::vector<boost::weak_ptr<Observer> > observers_;
  typedef std::vector<boost::weak_ptr<Observer> >::iterator Iterator;
};

Observer::~Observer()
{
  // subject_->unregister(this);
}

void Observer::observe(Observable* s)
{
  s->register_(shared_from_this());
  subject_ = s;
}

void Observable::register_(boost::weak_ptr<Observer> x)
{
  base::MutexLockGuard lock(mutex_);
  observers_.push_back(x);
}

class Foo : public Observer
{
public:
  virtual void update() 
  {
    printf("Foo::update() %p\n", this);
  }
};

int main()
{
  Observable subject;
  {
    boost::shared_ptr<Foo> foo(new Foo);
    foo->observe(&subject);
    subject.notifyObservers(); 
  }
  subject.notifyObservers();
}
