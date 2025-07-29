/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#include <utility>

#include "clob/OrderBook.h"

namespace clob {

void OrderBook::add_bid_order(LimitOrder *order) { bids.push(order); }

void OrderBook::add_ask_order(LimitOrder *order) { asks.push(order); }

} // namespace clob