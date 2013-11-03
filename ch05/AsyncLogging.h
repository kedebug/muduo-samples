#ifndef _ASYNC_LOGGING_H_
#define _ASYNC_LOGGING_H_

#include <base/Mutex.h>
#include <base/Thread.h>
#include <base/Condition.h>
#include <base/CountDownLatch.h>

#include "LogStream.h"

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace base {

class AsyncLogging : boost::noncopyable {
public:
    AsyncLogging(const string& basename, 
                 size_t rollSize,
                 int flushInterval = 3);

    ~AsyncLogging() {
        if (running_)
            stop();
    }
    void start() {
        running_ = true;
        thread_.start();
        latch_.wait();
    }
    void stop() {
        running_ = false;
        cond_.notify();
        thread_.join();
    }
    void append(const char *logline, int len);
private:
    AsyncLogging(const AsyncLogging&);
    // void operator=(const AsyncLogging&);

    typedef detail::FixedBuffer<detail::kLargeBuffer> Buffer;
    typedef boost::ptr_vector<Buffer> BufferVector;
    typedef BufferVector::auto_type BufferPtr;

    void threadFunc();

    const int         flushInterval_;
    bool              running_;
    string            basename_;
    size_t            rollSize_;
    Thread            thread_;
    MutexLock         mutex_;
    Condition         cond_;
    CountDownLatch    latch_;
    BufferPtr         currentBuffer_;
    BufferPtr         nextBuffer_;
    BufferVector      buffers_;
};

} // namespace base


#endif // _ASYNC_LOGGING_H_
