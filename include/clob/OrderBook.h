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
  std::priority_queue<LimitOrder *, std::vector<LimitOrder *>,
                      LimitOrder::PriceTimeQueuePriority::BidCmp>
      bids;
  std::priority_queue<LimitOrder *, std::vector<LimitOrder *>,
                      LimitOrder::PriceTimeQueuePriority::AskCmp>
      asks;

  /**
   * @brief Match the orders in the order book.
   */
  template <LimitOrder::OrderType order_type>
  void match_orders(LimitOrder *new_order);

public:
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

  /**
   * @brief Get the top bid order from the order book.
   *
   * @return The top bid order.
   */
  const LimitOrder *get_best_bid_order() const;

  /**
   * @brief Get the top ask order from the order book.
   * Assumes the ask order book is not empty.
   *
   * @return The top ask order.
   */
  const LimitOrder *get_best_ask_order() const;

  /**
   * @brief Get the number of bid orders.
   *
   * @return The number of bid orders.
   */
  std::size_t bids_size() const { return bids.size(); }

  /**
   * @brief Get the number of ask orders.
   *
   * @return The number of ask orders.
   */
  std::size_t asks_size() const { return asks.size(); }
};

} // namespace clob