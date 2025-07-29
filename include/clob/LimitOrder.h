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
class LimitOrder {
public:
  enum class OrderType { Bid, Ask };
  using id_t = uint_fast64_t;

  class PriceTimeQueuePriority {
  public:
    struct BidCmp {
      constexpr inline bool operator()(const LimitOrder *o1,
                                       const LimitOrder *o2) const {
        return o1->price < o2->price ||
               (o1->price == o2->price && o1->timestamp > o2->timestamp);
      }
    };

    struct AskCmp {
      constexpr inline bool operator()(const LimitOrder *o1,
                                       const LimitOrder *o2) const {
        return o1->price > o2->price ||
               (o1->price == o2->price && o1->timestamp > o2->timestamp);
      }
    };
  };

  id_t id;
  timestamp_ns_t timestamp;
  price_t price;
  quantity_t quantity;
};

} // namespace clob