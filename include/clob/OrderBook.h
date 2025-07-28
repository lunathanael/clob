/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#pragma once

#include <queue>
#include <unordered_set>
#include <vector>

#include "clob/LimitOrder.h"

namespace clob {

class OrderBook {
public:
  std::unordered_set<LimitOrder::id_t> pending_orders;
  std::priority_queue<LimitOrder, std::vector<LimitOrder>, LimitOrder::PriceTimeQueuePriority::BidCmp> bids;
  std::priority_queue<LimitOrder, std::vector<LimitOrder>, LimitOrder::PriceTimeQueuePriority::AskCmp> asks;

  /**
   * @brief Add a bid order to the order book.
   *
   * @param order The order to add.
   */
  void add_bid_order(LimitOrder &&order);

  /**
   * @brief Add an ask order to the order book.
   *
   * @param order The order to add.
   */
  void add_ask_order(LimitOrder &&order);

  /**
   * @brief Cancel an order from the order book.
   *
   * @param order_id The ID of the order to cancel.
   *
   * @return True if the order was found and cancelled, false otherwise.
   */
  bool cancel_order(const LimitOrder::id_t order_id);
};

} // namespace clob