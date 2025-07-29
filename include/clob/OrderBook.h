/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#pragma once

#include <queue>
#include <vector>

#include "clob/LimitOrder.h"

namespace clob {

class OrderBook {
public:
  std::priority_queue<LimitOrder *, std::vector<LimitOrder *>,
                      LimitOrder::PriceTimeQueuePriority::BidCmp>
      bids;
  std::priority_queue<LimitOrder *, std::vector<LimitOrder *>,
                      LimitOrder::PriceTimeQueuePriority::AskCmp>
      asks;

  /**
   * @brief Add a bid order to the order book.
   *
   * @param order The order to add.
   */
  void add_bid_order(LimitOrder *order);

  /**
   * @brief Add an ask order to the order book.
   *
   * @param order The order to add.
   */
  void add_ask_order(LimitOrder *order);
};

} // namespace clob