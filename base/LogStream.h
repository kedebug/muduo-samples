#ifndef _LOG_STREAM_H_
#define _LOG_STREAM_H_

#include <base/Types.h>

#include <string.h>
#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

namespace base {

namespace detail {

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

template<int SIZE>
class FixedBuffer : boost::noncopyable {
public:
    FixedBuffer()
      : cur_(data_)
    { }
    
    void append(const char *buf, size_t len) {
        if (implicit_cast<size_t>(avail()) > len) {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }
    char *current() {
        return cur_;
    }
    int avail() const {
        return static_cast<int>(end() - cur_);
    }
    int length() const {
        return static_cast<int>(cur_ - data_);
    }
    void add(size_t len) {
        cur_ += len;
    }
    void reset() {
        cur_ = data_;
    }
    void bzero() {
        ::bzero(data_, sizeof(data_));
    }

private:
    const char *end() const {
        return data_ + sizeof(data_);
    }
    char *cur_;
    char data_[SIZE];
};

} // namespace detail

class LogStream : boost::noncopyable {
    typedef LogStream self;
public:
    typedef detail::FixedBuffer<detail::kSmallBuffer> Buffer;

    self& operator << (bool v) {
        buffer_.append(v ? "1" : "0", 1);
        return *this;
    }
    self& operator << (short);
    self& operator << (unsigned short);
    self& operator << (int);
    self& operator << (unsigned int);
    self& operator << (long);
    self& operator << (unsigned long);
    self& operator << (long long);
    self& operator << (unsigned long long);

    self& operator << (float v) {
        *this << static_cast<double>(v);
        return *this;
    } 
    self& operator << (double v);

    self& operator << (char v) {
        buffer_.append(&v, 1);
        return *this;
    }
    self& operator << (const char *v) {
        buffer_.append(v, strlen(v));
        return *this;
    }
    self& operator << (const string& v) {
        buffer_.append(v.c_str(), v.size());
        return *this;
    }
private:
    template<typename T>
    void formatInteger(T);

    Buffer buffer_;
    static const int kMaxNumericSize = 32;
};

} // namespace base


#endif // _LOG_STREAM_H_
