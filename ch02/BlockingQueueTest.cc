
#include <base/Thread.h>
#include <base/BlockingQueue.h>
#include <base/CountDownLatch.h>

#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <string>
#include <stdio.h>

class Test
{
public:
  explicit Test(int numThreads)
    : latch_(numThreads),
      threads_(numThreads)
  {
    for (int i = 0; i < numThreads; i++) {
      char name[32];
      snprintf(name, sizeof(name), "work thread: %d", i);
      threads_.push_back(new base::Thread(
                              boost::bind(&Test::threadFunc, this), 
                              std::string(name)));
    }
    for_each(threads_.begin(),
             threads_.end(), 
             boost::bind(&base::Thread::start, _1));
  }

  void run(int times)
  {
    printf("wait for the count down latch\n");
    latch_.wait();
    printf("all threads started\n");

    for (int i = 0; i < times; i++) {
      char buf[32];
      snprintf(buf, sizeof(buf), "hello %d", i);
      queue_.put(std::string(buf));
      printf("tid=%d, put data = %s, size = %zd\n",
              base::CurrentThread::tid(),
              buf,
              queue_.size());
    }
  }

  void joinAll()
  {
    for (size_t i = 0; i < threads_.size(); i++)
      queue_.put("stop");

    for_each(threads_.begin(), 
             threads_.end(), 
             boost::bind(&base::Thread::join, _1));
  }
private:

  void threadFunc()
  {
    printf("tid=%d, %s started\n",
           base::CurrentThread::tid(),
           base::CurrentThread::name());

    latch_.countDown();

    bool running = true;
    while (running) {
      std::string task(queue_.take());
      printf("tid=%d, get data = %s, size = %zd\n",
             base::CurrentThread::tid(),
             task.c_str(),
             queue_.size());
      running = (task != "stop");
    }

    printf("tid=%d, %s stopped\n",
           base::CurrentThread::tid(),
           base::CurrentThread::name());
  }

  base::BlockingQueue<std::string> queue_;
  base::CountDownLatch latch_;
  boost::ptr_vector<base::Thread> threads_;
};

int main()
{
  printf("pid=%d, tid=%d\n", ::getpid(), base::CurrentThread::tid());
  Test t(8);
  t.run(1000);
  t.joinAll();
  printf("numbers of created threads %d\n", base::Thread::numCreated());
}
