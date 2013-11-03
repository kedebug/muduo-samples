#include "LogFile.h"
#include <stdio.h>
#include <time.h>

using namespace base;
using std::string;

class LogFile::File : boost::noncopyable {
public:
    explicit 
    File(const string& filename)
      : fp_(::fopen(filename.c_str(), "ae")),
        writtenBytes_(0) {
        ::setbuffer(fp_, buffer_, sizeof(buffer_));
    }
    ~File() {
        ::fclose(fp_);
    }

    void append(const char *logline, const size_t len) {
        size_t remain = len;
        while (remain > 0) {
            int n = write(logline, len);
            if (n == 0) {
                int err = ::ferror(fp_);
                if (err) 
                    fprintf(stderr, "LogFile::File::append() failed\n");
                break;
            }
            writtenBytes_ += n;
            remain -= n;
        }
    }

    void flush() {
        ::fflush(fp_);
    }

    size_t writtenBytes() {
        return writtenBytes_;
    }
private:
    size_t write(const char *logline, size_t len) {
        return ::fwrite_unlocked(logline, 1, len, fp_);
    }

    FILE *fp_;
    char buffer_[64*1024];
    size_t writtenBytes_;
};

LogFile::LogFile(const string& basename,
                 size_t rollSize,
                 bool threadSafe,
                 int flushInterval) 
  : basename_(basename),
    rollSize_(rollSize),
    flushInterval_(flushInterval),
    count_(0),
    mutex_(threadSafe ? new MutexLock : NULL),
    startOfPeriod_(0),
    lastRoll_(0),
    lastFlush_(0) {
    
    rollFile();
}

LogFile::~LogFile() {
}

void LogFile::rollFile() { 
    time_t now = 0;
    string filename = getLogFileName(basename_, &now);
    time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;
    
    if (now > lastRoll_) {
        lastRoll_ = now;
        lastFlush_ = now;
        startOfPeriod_ = start;
        file_.reset(new File(filename));
    }
}

string LogFile::getLogFileName(const string& basename,
                               time_t *now) {
    string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

    char timebuf[32];
    struct tm tm;
    *now = ::time(NULL);
    ::gmtime_r(now, &tm);
    ::strftime(timebuf, sizeof(timebuf), ".%Y%m%d-%H%M%S.", &tm);
    filename += timebuf;
    filename += ".log";

    return filename;
}

void LogFile::flush() {
    if (mutex_) {
        MutexLockGuard lock(*mutex_);
        file_->flush();
    } else {
        file_->flush();
    }
}

void LogFile::append(const char *logline, int len) {
    if (mutex_) {
        MutexLockGuard lock(*mutex_);
        append_unlocked(logline, len);
    } else {
        append_unlocked(logline, len);
    }
}

void LogFile::append_unlocked(const char *logline, int len) {
    file_->append(logline, len);

    if (file_->writtenBytes() > rollSize_) {
        rollFile();
    } else {
        if (count_ > 1024) {
            count_ = 0;
            time_t now = ::time(NULL);
            time_t thisPriod = now / kRollPerSeconds_ * kRollPerSeconds_;
            if (thisPriod != startOfPeriod_) {
                rollFile();
            } else if (now - lastFlush_ > flushInterval_) {
                lastFlush_ = now;
                file_->flush();
            }
        } else {
            ++count_;
        }
    }
}
