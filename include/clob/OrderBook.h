/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#pragma once

#include <queue>
#include <vector>

#include "clob/LimitOrder.h"
#include "clob/types.h"

namespace clob {

class OrderBook {
  std::priority_queue<LimitOrder<OrderType::Bid> *,
                      std::vector<LimitOrder<OrderType::Bid> *>,
                      LimitOrder<OrderType::Bid>::PriceTimeQueuePriority>
      bids;
  std::priority_queue<LimitOrder<OrderType::Ask> *,
                      std::vector<LimitOrder<OrderType::Ask> *>,
                      LimitOrder<OrderType::Ask>::PriceTimeQueuePriority>
      asks;

  /**
   * @brief Match the orders in the order book.
   */
  template <OrderType order_type>
  void match_orders(LimitOrder<order_type> *new_order);

public:
  /**
   * @brief Add a bid order to the order book.
   *
   * @param order The order to add.
   */
  template <OrderType order_type> void add_order(LimitOrder<order_type> *order);

  /**
   * @brief Get the top bid order from the order book.
   *
   * @return The top bid order.
   */
  template <OrderType order_type>
  const LimitOrder<order_type> *get_best_order() const;

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