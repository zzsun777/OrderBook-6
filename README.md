# OrderBook
Orderbook for an instrument with both bid and ask tables.

OrderBook uses unordered_map (hashmap) to store the Order mapped using the id. This would help retrieve the Order
information in constant time. 

Uses two sets to contain the bid price list and offer price list. Since Sets are BST we will be obtaining a long
complexity for insertion and deletion.

For deleting order for an existing price the complexity will be constant time since we won't change the set and
only update the OrderListPtr's totalQty (since there are other open orders for that price). Only in case of deleting
the last existing order we will delete the OrderListPtr from the Set.

Here the complexity is the highest of O(n) (where n is the number of distinct prices for a side) when traversing the
set for publishing the list of open prices. Or even for getting a price at nth level.
