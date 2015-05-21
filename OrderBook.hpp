#ifndef _ORDER_BOOK_HPP_
#define _ORDER_BOOK_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <set>

using namespace std;

namespace SIDE
{
  const char BID = 'B';
  const char OFFER = 'O';
}

struct Order
{
  int id_;
  char side_;
  double price_;
  int quantity_;

  Order(int id, char side, double price, int quantity):id_(id), side_(side), price_(price), quantity_(quantity)
  {
  }
};

/*
 * @brief : OrderBook is the class to maintain and manage the orders
 * for a particular instrument.
 */

class OrderBook
{
private:
 
  typedef boost::shared_ptr<Order> OrderPtr;

  // forward declare OrderList
  struct OrderList;
  typedef boost::shared_ptr<OrderList> OrderListPtr;
  
  // maintain an orderlist based on the price it holds.
  struct OrderList
  {
    double price_;
    int totalQty_;
    list<OrderPtr> orderList_;

    OrderList(OrderPtr& orderPtr):price_(orderPtr->price_), totalQty_(orderPtr->quantity_)
    {
      orderList_.push_back(orderPtr);
    }
  };

  struct AscendCompare
  {
    bool operator() (const OrderListPtr& OLP1, const OrderListPtr& OLP2)
    {
      return comparator(OLP1->price_, OLP2->price_);
    }

    greater<double> comparator;    
  };

  struct DescendCompare
  {
    bool operator() (const OrderListPtr& OLP1, const OrderListPtr& OLP2)
    {
      return !comparator(OLP1->price_, OLP2->price_);
    }
    
    greater<double> comparator;
  };

  // store id to orderPtr hash map for constant time lookup of order based on id's
  typedef boost::unordered_map<int, OrderPtr> OrderIdHashMap;

  // have a set to maintain distinct sorted price
  typedef set<OrderListPtr, AscendCompare> AscendOrderSet;
  typedef set<OrderListPtr, DescendCompare> DescendOrderSet;  

  // store price to orderlist hash map for constant time lookup for existing price
  typedef boost::unordered_map<double, OrderListPtr> OrderListHashMap; 

  AscendOrderSet bidSet_; // will be a max set to keep the best price on top
  DescendOrderSet offerSet_; // will be a min set to keep the best ask price on top
  OrderListHashMap bidOrderHashMap_;
  OrderListHashMap offerOrderHashMap_;  
  OrderIdHashMap orderIdHashMap_;

  // do not copy
  OrderBook(const OrderBook& );
  OrderBook& operator=(const OrderBook&);

  void addOrUpdateSet(OrderPtr& orderPtr, char side);
  bool deleteFromSet(OrderPtr& orderPtr, char side);

public:
  OrderBook() {}
  ~OrderBook() {}
  
  bool enterOrder(int id, char side, double price, int quantity);
  bool getOrderFromId(int id, Order& record);
  bool modifyOrder(int id, int quantity);
  bool deleteOrder(int id);
  void printOrderBook();
  double getPriceForSideAndLevel(char side, int level = 0);
  double getQuantityForSideAndLevel(char side, int level = 0);
};

#endif // _ORDER_BOOK_HPP_
