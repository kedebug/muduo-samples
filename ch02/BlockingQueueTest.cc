
#include <base/Thread.h>
#include <base/BlockingQueue.h>
#include <base/CountDownLatch.h>

#include <boost/ptr_vector.hpp>
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
      threads_.push_back(new Thread(
          boost::bind(Test::threadFunc, this), std::string(name)));
    }

  }

  void run(int times)
  {
     
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
      running = (task == "stop");
    }

    printf("tid=%d, %s stopped\n",
           base::CurrentThread::tid(),
           base::CurrentThread::name());
  }

  base::BlockingQueue<std::string> queue_;
  base::CountDownLatch latch_;
  boost::ptr_vector<base::Thread> threads_;
}
