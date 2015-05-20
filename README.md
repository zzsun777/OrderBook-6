# OrderBook
Orderbook for an instrument with both bid and ask tables.

OrderBook uses unordered_map (hashmap) to store the Order mapped using the id. This would help retrieve the Order
information in constant time. 

Also uses two vectors to store the OrderList which is the grouping of the price and the list of orders for the same
price.
