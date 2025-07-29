/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#include <utility>

#include "clob/OrderBook.h"

namespace clob {

void OrderBook::add_bid_order(LimitOrder *order) {
  pending_orders.insert(order->id);
  bids.push(order);
}

void OrderBook::add_ask_order(LimitOrder *order) {
  pending_orders.insert(order->id);
  asks.push(order);
}

bool OrderBook::cancel_order(const LimitOrder::id_t order_id) {
  return pending_orders.erase(order_id) > 0;
}

} // namespace clob