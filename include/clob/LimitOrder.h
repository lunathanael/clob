/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#pragma once

#include "clob/types.h"

namespace clob {

/**
 * @brief A class representing a limit order
 *
 * @details Includes the id, timestamp, price, and quantity of the order.
 */
template <OrderType order_type> class LimitOrder {
public:
  class PriceTimeQueuePriority {
  public:
    constexpr inline bool operator()(const LimitOrder<order_type> *o1,
                                     const LimitOrder<order_type> *o2) const {
      if constexpr (order_type == OrderType::Bid) {
        return o1->price < o2->price ||
               (o1->price == o2->price && o1->timestamp > o2->timestamp);
      } else {
        return o1->price > o2->price ||
               (o1->price == o2->price && o1->timestamp > o2->timestamp);
      }
    }
  };

  LimitOrderId_t id;
  timestamp_ns_t timestamp;
  balance_t balance;
  price_t price;
  quantity_t quantity;
  quantity_t filled_quantity;
  bool is_cancelled;

  explicit LimitOrder(const LimitOrderId_t id, const timestamp_ns_t timestamp,
                      const price_t price, const quantity_t quantity)
      : id(id), timestamp(timestamp), balance(0), price(price),
        quantity(quantity), filled_quantity(0), is_cancelled(false) {}
};

} // namespace clob