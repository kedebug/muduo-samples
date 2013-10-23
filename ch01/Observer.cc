#include <stdio.h>
#include <algorithm>
#include <vector>

class Observable;
class Observer;

class Observer
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
  void register_(Observer* s);
  void unregister(Observer* s);
  void notifyObservers()
  {
    for (size_t i = 0; i < observers_.size(); i++) {
      Observer* s = observers_[i];
      if (s) {
        s->update();
      }
    }
  }

private:
  std::vector<Observer*> observers_;
};

Observer::~Observer()
{
  subject_->unregister(this);
}

void Observer::observe(Observable* s)
{
  s->register_(this);
  subject_ = s;
}

void Observable::register_(Observer* s)
{
  observers_.push_back(s);
}

void Observable::unregister(Observer* s)
{
  std::vector<Observer*>::iterator it = std::find(observers_.begin(), observers_.end(), s);
  if (it != observers_.end()) {
    std::swap(*it, observers_.back());
    observers_.pop_back();
  }
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
  Foo* p = new Foo;
  Observable subject;
  p->observe(&subject);
  subject.notifyObservers();
  delete p;
}
