#include "LogStream.h"

#include <algorithm>
#include <stdio.h>

namespace base {
namespace detail {

template<typename T>
size_t convert(char buf[], T value) {
    T v = value > 0 ? value : -value;
    char *p = buf;

    do {
        int lsd = static_cast<int>(v % 10);
        v /= 10;
        *p++ = lsd + '0';
    } while (v != 0);

    if (value < 0)
        *p++ = '-';

    *p = '\0';
    std::reverse(buf, p);
    return p - buf;
}

} // namespace detail
} // namespace base

using base::LogStream;
using base::detail::convert;

template<typename T>
void LogStream::formatInteger(T v) {
    if (buffer_.avail() >= kMaxNumericSize) {
        size_t len = convert(buffer_.current(), v);
        buffer_.add(len);
    }
}

LogStream& LogStream::operator << (short v) {
    *this << static_cast<int>(v);
    return *this;
}

LogStream& LogStream::operator << (unsigned short v) {
    *this << static_cast<unsigned int>(v);
    return *this;
}

LogStream& LogStream::operator << (int v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator << (unsigned int v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator << (long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator << (unsigned long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator << (long long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator << (unsigned long long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator << (double v) {
    if (buffer_.avail() > kMaxNumericSize) {
        int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
        buffer_.add(len);
    }
    return *this;
}
