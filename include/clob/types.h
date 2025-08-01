/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#pragma once

#include <cstdint>
#include <utility>

namespace clob {

using LimitOrderId_t = uint_fast64_t;
using StockId_t = uint_fast64_t;
using timestamp_ns_t = uint_fast64_t;
using price_t = uint_fast32_t;
using quantity_t = uint_fast32_t;
using balance_t = int_fast64_t;

enum class OrderType { Bid, Ask };

template <bool condition, typename T1, typename T2>
inline constexpr decltype(auto) ternary(T1 &&t1, T2 &&t2) {
  if constexpr (condition) {
    return std::forward<T1>(t1);
  } else {
    return std::forward<T2>(t2);
  }
}

} // namespace clob