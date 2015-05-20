#include "OrderBook.hpp"

bool OrderBook::enterOrder(int id, char side, double price, int quantity)
{
  // if id already exists return error
  if (orderIdHashMap_.find(id) != orderIdHashMap_.end())
    {
      cerr << "Order with Id already exists" << endl;
      return false;
    }

  // else add the order to the hash map and also add and update the heap based on the side (if valid side)
  bool status = true;
  switch(side)
    {
    case SIDE::BID:
      orderIdHashMap_[id] = OrderPtr(new Order(id, side, price, quantity));
      addOrUpdateHeap(bidHeap_, orderIdHashMap_[id], SIDE::BID);
      break;
    case SIDE::OFFER:
      orderIdHashMap_[id] = OrderPtr(new Order(id, side, price, quantity));
      addOrUpdateHeap(offerHeap_, orderIdHashMap_[id], SIDE::OFFER);
      break;
    default:
      cerr << "Invalid SIDE received. Order Creation failed!!!" << endl;
      status = false;
    }

  return status;
}


bool OrderBook::getOrderFromId(int id, Order& record)
{
  OrderIdHashMap::iterator iter = orderIdHashMap_.find(id);

  if (iter != orderIdHashMap_.end())
    {
      record = *iter->second.get();
      return true;
    }

  return false;
}

bool OrderBook::modifyOrder(int id, int quantity)
{
  // sanity check on quantity
  if (quantity <= 0)
    {
      cerr << "Invalid quantity sent. Order modification failed!!!" << endl;
      return false;
    }
  
  OrderIdHashMap::iterator iter = orderIdHashMap_.find(id);

  if (iter != orderIdHashMap_.end())
    {
      // now that we have the order, need to update the orderIdHashMap and also the orderListHashMap based on price
      int quantityDiff = quantity - iter->second->quantity_;
      iter->second->quantity_ = quantity;

      OrderListHashMap& orderListHashMap = (iter->second->side_ == SIDE::BID)? bidOrderHashMap_ : offerOrderHashMap_;
      // update the quantity diff on the OrderList total quantity
      orderListHashMap[iter->second->price_]->totalQty_ = orderListHashMap[iter->second->price_]->totalQty_ + quantityDiff;
      return true;
    }

  return false;
}

void OrderBook::printOrderBook()
{
  cout << "Printing Bid OrderBook" << endl;
  for (OrderHeap::iterator iter = bidHeap_.begin(); iter != bidHeap_.end(); ++iter)
    {
      cout << (*iter)->price_ << " : " << (*iter)->totalQty_ << endl;
    }

  cout << "Printing Offer OrderBook" << endl;
  for (OrderHeap::iterator iter = offerHeap_.begin(); iter != offerHeap_.end(); ++iter)
    {
      cout << (*iter)->price_ << " : " << (*iter)->totalQty_ << endl;
    }
}

double OrderBook::getPriceForSideAndLevel(char side, int level /*= 0*/)
{
  double price = 0.0;

  switch(side)
    {
    case SIDE::BID:
      if (level < bidHeap_.size())
	price = bidHeap_[level]->price_;
      break;
    case SIDE::OFFER:
      if (level < offerHeap_.size())
	price = offerHeap_[level]->price_;
      break;
    default:
      cerr << "Invalid side given!!" << endl;
    }

  return price;
}


double OrderBook::getQuantityForSideAndLevel(char side, int level /*= 0*/)
{
  int totalQty = 0;

  switch(side)
    {
    case SIDE::BID:
      if (level < bidHeap_.size())
	totalQty = bidHeap_[level]->totalQty_;
      break;
    case SIDE::OFFER:
      if (level < offerHeap_.size())
	totalQty = offerHeap_[level]->totalQty_;
      break;
    default:
      cerr << "Invalid side given!!" << endl;
    }

  return totalQty;
}

bool OrderBook::deleteOrder(int id)
{
  OrderIdHashMap::iterator iter = orderIdHashMap_.find(id);

  if (iter != orderIdHashMap_.end())
    {
      orderIdHashMap_.erase(iter);
      // found the element. now delete from the heap and then delete from this hash map
      return deleteFromHeap((iter->second->side_ == SIDE::BID) ? bidHeap_ : offerHeap_, iter->second, iter->second->side_);
    }

  return false;
}

void OrderBook::addOrUpdateHeap(OrderHeap& heap, OrderPtr& orderPtr, char side)
{
  OrderListHashMap& orderListHashMap = (side == SIDE::BID)? bidOrderHashMap_ : offerOrderHashMap_;  
  OrderListHashMap::iterator iter = orderListHashMap.find(orderPtr->price_);
  if (iter != orderListHashMap.end())
    {
      // price already exists
      iter->second->totalQty_ = iter->second->totalQty_ + orderPtr->quantity_;
      iter->second->orderList_.push_back(orderPtr);
    }
  else
    {
      // new price .. need to heapify
      orderListHashMap[orderPtr->price_] = OrderListPtr(new OrderList(orderPtr));
      heap.push_back(orderListHashMap[orderPtr->price_]);
      push_heap(heap.begin(), heap.end(), (side == SIDE::BID) ? (OrderList::lessThan) : (OrderList::greaterThan));
    }
}

bool OrderBook::deleteFromHeap(OrderHeap& heap, OrderPtr& orderPtr, char side)
{
  OrderListHashMap& orderListHashMap = (side == SIDE::BID)? bidOrderHashMap_ : offerOrderHashMap_;  
  OrderListHashMap::iterator iter = orderListHashMap.find(orderPtr->price_);

  if (iter != orderListHashMap.end())
    {
      iter->second->totalQty_ = iter->second->totalQty_ - orderPtr->quantity_; // reduce the quantity of the order from totalQty
      iter->second->orderList_.remove(orderPtr);

      if (iter->second->totalQty_ == 0) // remove the empty orderListPtr from the heap and heapify again
	{
	  OrderHeap::iterator ptr = find(heap.begin(), heap.end(), iter->second);
	  heap.erase(ptr);

	  // improve here by only heapifying the sub trees	  
	  make_heap(heap.begin(), heap.end(), (side == SIDE::BID) ? (OrderList::lessThan) : (OrderList::greaterThan));
	  orderListHashMap.erase(iter); // remove the element off the hashmap too since no quantity .. could keep it if chances of the same price are more (save the creation and erase times... for now deleting it
	}

      return true;
    }

  return false;
}
