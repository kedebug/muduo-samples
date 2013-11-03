#include "AsyncLogging.h"
#include "LogFile.h"

#include <stdio.h>

using namespace base;
using std::string;

AsyncLogging::AsyncLogging(const string& basename,
                           size_t rollSize,
                           int flushInterval)
  : flushInterval_(flushInterval),
    running_(false),
    basename_(basename),
    rollSize_(rollSize),
    thread_(boost::bind(&AsyncLogging::threadFunc, this), "Logging"),
    mutex_(),
    cond_(mutex_),
    latch_(1),
    currentBuffer_(new Buffer),
    nextBuffer_(new Buffer),
    buffers_() {

    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
}

void AsyncLogging::append(const char *logline, int len) {
    MutexLockGuard lock(mutex_);
    if (currentBuffer_->avail() > len) {
        currentBuffer_->append(logline, len);
    } else {
        buffers_.push_back(currentBuffer_.release());
        if (nextBuffer_) 
            currentBuffer_ = boost::ptr_container::move(nextBuffer_);
        else 
            currentBuffer_.reset(new Buffer);

        currentBuffer_->append(logline, len);
        cond_.notify();
    }    
}

void AsyncLogging::threadFunc() {
    assert(running_ == true);
    latch_.countDown();

    LogFile output(basename_, rollSize_, false);

    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    BufferVector buffers2write;

    newBuffer1->bzero();
    newBuffer2->bzero();
    buffers2write.reserve(16);
     
    while (running_) {
        assert(newBuffer1 && newBuffer1->length() == 0); 
        assert(newBuffer2 && newBuffer2->length() == 0); 

        {
            MutexLockGuard lock(mutex_);
            if (buffers_.empty())
                cond_.waitForSeconds(flushInterval_);

            buffers_.push_back(currentBuffer_.release());
            currentBuffer_ = boost::ptr_container::move(newBuffer1);
            buffers2write.swap(buffers_);

            if (!nextBuffer_) 
                nextBuffer_ = boost::ptr_container::move(newBuffer2);
        }
        
        if (buffers2write.size() > 25) {
            char buf[64];
            snprintf(buf, sizeof(buf), "Dropped log message, %zd larger buffers\n",
                     buffers2write.size() - 2);
            fputs(buf, stderr);
            output.append(buf, strlen(buf));
            buffers2write.erase(buffers2write.begin() + 2, buffers2write.end());
        }

        for (size_t i = 0; i < buffers2write.size(); i++) {
            output.append(buffers2write[i].data(), buffers2write[i].length());
        }
        
        if (buffers2write.size() > 2) 
            buffers2write.resize(2);

        if (!newBuffer1) {
            newBuffer1 = buffers2write.pop_back();
            newBuffer1->reset();
        }
        if (!newBuffer2) {
            newBuffer2 = buffers2write.pop_back();
            newBuffer2->reset();
        }

        buffers2write.clear();
        output.flush();
    }
    output.flush();
}
