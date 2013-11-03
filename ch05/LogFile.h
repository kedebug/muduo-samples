#ifndef _LOG_FILE_H_
#define _LOG_FILE_H_

#include <base/Mutex.h>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <string>
using std::string;

namespace base {

class LogFile : boost::noncopyable {
public:
    LogFile(const string& basename,
            size_t rollSize,
            bool threadSafe = true,
            int flushInterval = 3);
    ~LogFile();

    void append(const char *logline, int len);
    void flush();

private:
    void append_unlocked(const char *logline, int len);
    void rollFile();
    static string getLogFileName(const string& basename, time_t *now);
    
    const string basename_;
    const size_t rollSize_;
    const int flushInterval_;

    int count_;

    boost::scoped_ptr<base::MutexLock> mutex_;
    time_t startOfPeriod_;
    time_t lastRoll_;
    time_t lastFlush_;
    class File;
    boost::scoped_ptr<File> file_;

    const static int kRollPerSeconds_ = 60 * 60 * 24;
};

} // namespace base

#endif // _LOG_FILE_H_
