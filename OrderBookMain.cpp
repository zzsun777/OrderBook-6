#include "OrderBook.hpp"

int main()
{
  OrderBook orderBook;

  orderBook.enterOrder(2134, SIDE::BID, 21.3, 123);
  orderBook.enterOrder(2135, SIDE::BID, 24.6, 123);
  orderBook.enterOrder(2136, SIDE::BID, 12.9, 123);
  orderBook.enterOrder(2137, SIDE::BID, 7.7, 123);
  orderBook.enterOrder(2138, SIDE::BID, 21.3, 123);
  orderBook.enterOrder(2139, SIDE::BID, 19.8, 123);
  orderBook.enterOrder(2140, SIDE::BID, 11.5, 123);

  orderBook.enterOrder(2234, SIDE::OFFER, 21.3, 123);
  orderBook.enterOrder(2235, SIDE::OFFER, 24.6, 123);
  orderBook.enterOrder(2236, SIDE::OFFER, 12.9, 123);
  orderBook.enterOrder(2237, SIDE::OFFER, 7.7, 123);
  orderBook.enterOrder(2238, SIDE::OFFER, 21.3, 123);
  orderBook.enterOrder(2239, SIDE::OFFER, 19.8, 123);
  orderBook.enterOrder(2240, SIDE::OFFER, 11.5, 123);
  
  orderBook.printOrderBook();
  orderBook.deleteOrder(2134);
  orderBook.printOrderBook();

  orderBook.modifyOrder(2139, 12);
  orderBook.printOrderBook();
  orderBook.deleteOrder(2139);
  orderBook.printOrderBook();

  cout << orderBook.getPriceForSideAndLevel(SIDE::BID, 1) << endl;
  cout << orderBook.getQuantityForSideAndLevel(SIDE::BID, 1) << endl;  
}
