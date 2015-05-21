#include "OrderBook.hpp"

bool OrderBook::enterOrder(int id, char side, double price, int quantity)
{
  // if id already exists return error
  if (orderIdHashMap_.find(id) != orderIdHashMap_.end())
    {
      cerr << "Order with Id already exists" << endl;
      return false;
    }

  // else add the order to the hash map and also add and update the set based on the side (if valid side)
  bool status = true;
  switch(side)
    {
    case SIDE::BID:
      orderIdHashMap_[id] = OrderPtr(new Order(id, side, price, quantity));
      addOrUpdateSet(orderIdHashMap_[id], SIDE::BID);
      break;
    case SIDE::OFFER:
      orderIdHashMap_[id] = OrderPtr(new Order(id, side, price, quantity));
      addOrUpdateSet(orderIdHashMap_[id], SIDE::OFFER);
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
  for (AscendOrderSet::iterator iter = bidSet_.begin(); iter != bidSet_.end(); ++iter)
    {
      cout << (*iter)->price_ << " : " << (*iter)->totalQty_ << endl;
    }

  cout << "Printing Offer OrderBook" << endl;
  for (DescendOrderSet::iterator iter = offerSet_.begin(); iter != offerSet_.end(); ++iter)
    {
      cout << (*iter)->price_ << " : " << (*iter)->totalQty_ << endl;
    }
}

double OrderBook::getPriceForSideAndLevel(char side, int level /*= 0*/)
{
  double price = 0.0;

  if (level < 0)
    return price;
  
  switch(side)
    {
    case SIDE::BID:
      if (level < bidSet_.size())
	{
	  AscendOrderSet::iterator iter;
	  iter = bidSet_.begin();
	  advance(iter, level);
	  price = (*iter)->price_;
	}
      break;
    case SIDE::OFFER:
      if (level < offerSet_.size())
	{
	  DescendOrderSet::iterator iter;
	  iter = offerSet_.begin();
	  advance(iter, level);
	  price = (*iter)->price_;
	}
      break;
    default:
      cerr << "Invalid side given!!" << endl;
    }

  return price;
}


double OrderBook::getQuantityForSideAndLevel(char side, int level /*= 0*/)
{
  int totalQty = 0;

  if (level < 0)
    return totalQty;
  
  switch(side)
    {
    case SIDE::BID:
      if (level < bidSet_.size())
	{
	  AscendOrderSet::iterator iter;
	  iter = bidSet_.begin();
	  advance(iter, level);
	  totalQty = (*iter)->totalQty_;
	}
      break;
    case SIDE::OFFER:
      if (level < offerSet_.size())
	{
	  DescendOrderSet::iterator iter;
	  iter = offerSet_.begin();
	  advance(iter, level);
	  totalQty = (*iter)->totalQty_;
	}
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
      // found the element. now delete from the set and then delete from this hash map
      return deleteFromSet(iter->second, iter->second->side_);
    }

  return false;
}

void OrderBook::addOrUpdateSet(OrderPtr& orderPtr, char side)
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
      // new price .. need to bst insert into set (logn)
      orderListHashMap[orderPtr->price_] = OrderListPtr(new OrderList(orderPtr));
      if (side == SIDE::BID)
	bidSet_.insert(orderListHashMap[orderPtr->price_]); // logn insert cost since bst
      else
	offerSet_.insert(orderListHashMap[orderPtr->price_]); // logn insert cost since bst
    }
}

bool OrderBook::deleteFromSet(OrderPtr& orderPtr, char side)
{
  OrderListHashMap& orderListHashMap = (side == SIDE::BID)? bidOrderHashMap_ : offerOrderHashMap_;  
  OrderListHashMap::iterator iter = orderListHashMap.find(orderPtr->price_);

  if (iter != orderListHashMap.end())
    {
      iter->second->totalQty_ = iter->second->totalQty_ - orderPtr->quantity_; // reduce the quantity of the order from totalQty
      iter->second->orderList_.remove(orderPtr);

      if (iter->second->totalQty_ == 0) // remove the empty orderListPtr from the set (logn) since bst
	{
	  if (side == SIDE::BID)
	    bidSet_.erase(iter->second);
	  else
	    offerSet_.erase(iter->second);

	  orderListHashMap.erase(iter); // remove the element off the hashmap too since no quantity .. could keep it if chances of the same price are more (save the creation and erase times... for now deleting it
	}

      return true;
    }

  return false;
}
