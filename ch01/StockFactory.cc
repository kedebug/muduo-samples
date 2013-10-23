#include <base/Mutex.h>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <assert.h>
#include <stdio.h>

#include <map>

using std::string;

class Stock : boost::noncopyable
{
public:
  Stock(const string& name) 
    : name_(name)
  {
    printf("Stock[%p] %s\n", this, name_.c_str());
  }
  ~Stock()
  {
    printf("~Stock[%p] %s\n", this, name_.c_str());
  }
  const string& key()
  {
    return name_;
  }

private:
    string name_;
};

class StockFactory : public boost::enable_shared_from_this<StockFactory>,
                     boost::noncopyable
{
public:
  boost::shared_ptr<Stock> get(const string& key)
  {
    boost::shared_ptr<Stock> pStock;
    base::MutexLockGuard lock(mutex_);
    boost::weak_ptr<Stock>& wkStock = stocks_[key];
    pStock = wkStock.lock();
    if (!pStock) {
      pStock.reset(new Stock(key),
                   boost::bind(StockFactory::weakDeleteCallback,
                               boost::weak_ptr<StockFactory>(shared_from_this()),
                               _1));
      wkStock = pStock;
    }
    return pStock;
  }

private:
  static void weakDeleteCallback(boost::weak_ptr<StockFactory> wkFactory, Stock* stock)
  {
    printf("weakDeleteCallback[%p]\n", stock);
    boost::shared_ptr<StockFactory> factory(wkFactory.lock());
    if (factory)
      factory->removeStock(stock);
    else
      printf("StockFactory died.\n");
    delete stock;
  }

  void removeStock(Stock* stock)
  {
    base::MutexLockGuard lock(mutex_);
    stocks_.erase(stock->key());
  }

private:
  base::MutexLock mutex_;
  std::map<string, boost::weak_ptr<Stock> > stocks_;
};

void testLongLifeFactory()
{
  boost::shared_ptr<StockFactory> factory(new StockFactory);
  {
    boost::shared_ptr<Stock> stock1 = factory->get("NYSE:IBM");
    boost::shared_ptr<Stock> stock2 = factory->get("NYSE:IBM");
    assert(stock1 == stock2);
  }
}

void testShortLifeFactory()
{
  boost::shared_ptr<Stock> stock1;
  {
    boost::shared_ptr<StockFactory> factory(new StockFactory);
    stock1 = factory->get("NYSE:IBM");
    boost::shared_ptr<Stock> stock2 = factory->get("NYSE:IBM");
    assert(stock1 == stock2);
  }
}

int main() 
{
  testShortLifeFactory();
  testLongLifeFactory();
}
