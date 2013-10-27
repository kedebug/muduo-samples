#include <base/Mutex.h>
#include <boost/shared_ptr.hpp>
#include <map>
#include <string>
#include <vector>

using std::string;

class CustomData : boost::noncopyable
{
public:
  CustomData() 
    : data_(new Map)
  { }

  int query(const string& customer, const string& stock) const;

private:
  typedef std::pair<string, int> Entry;
  typedef std::vector<Entry> EntryList;
  typedef std::map<string, EntryList> Map;
  typedef boost::shared_ptr<Map> MapPtr;

  void update(const string& customer, const EntryList& entries);
  MapPtr getData() const
  {
    base::MutexLockGuard lock(mutex_);
    return data_;
  }

  mutable base::MutexLock mutex_;
  MapPtr data_;
};

int CustomData::query(const string& customer, const string& stock) const
{
  MapPtr data = getData();
  
  // todo with data
  return 0;
}

void CustomData::update(const string& customer, const EntryList& entries)
{
  base::MutexLockGuard lock(mutex_);
  if (!data_.unique()) {
    MapPtr newData(new Map(*data_));
    newData.swap(data_);
  }
  (*data_)[customer] = entries;
}

int main()
{
  CustomData data;
}

