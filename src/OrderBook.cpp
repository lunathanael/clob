/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#include <type_traits>

#include "clob/OrderBook.h"

namespace clob {

template <LimitOrder::OrderType order_type>
void OrderBook::match_orders(LimitOrder *new_order) {
  using order_book_t =
      std::conditional_t<order_type == LimitOrder::OrderType::Bid,
                         decltype(asks), decltype(bids)>;
  using new_order_book_t =
      std::conditional_t<order_type == LimitOrder::OrderType::Bid,
                         decltype(bids), decltype(asks)>;
  order_book_t *order_book;
  new_order_book_t *new_order_book;
  constexpr const balance_t balance_sign{order_type == LimitOrder::OrderType::Bid ? 1 : -1};
  if constexpr (order_type == LimitOrder::OrderType::Bid) {
    order_book = &asks;
    new_order_book = &bids;
  } else {
    order_book = &bids;
    new_order_book = &asks;
  }

  if (new_order->is_cancelled) {
    return;
  }

  LimitOrder *order;
  quantity_t order_q{}, new_order_q{new_order->quantity};
  while (order_book->size() > 0 && new_order_q != 0) {
    order = order_book->top();
    if (order->is_cancelled) {
      order_book->pop();
      continue;
    }

    if constexpr (order_type == LimitOrder::OrderType::Bid) {
      if (order->price > new_order->price) {
        break;
      }
    } else {
      if (order->price < new_order->price) {
        break;
      }
    }

    order_q = order->quantity - order->filled_quantity;

    if (order_q <= new_order_q) {
      new_order->balance -= balance_sign * order_q * order->price;
      order->balance += balance_sign * order_q * order->price;
      new_order->filled_quantity += order_q;
      new_order_q -= order_q;
      order->filled_quantity = order->quantity;
      order_book->pop();
    } else {
      new_order->balance -= balance_sign * new_order_q * order->price;
      order->balance += balance_sign * new_order_q * order->price;
      order->filled_quantity += new_order_q;
      new_order->filled_quantity = new_order->quantity;
      new_order_q = 0;
      return;
    }
  }

  if (new_order_q != 0) {
    new_order_book->push(new_order);
  }
}

void OrderBook::add_bid_order(LimitOrder *order) {
  match_orders<LimitOrder::OrderType::Bid>(order);
}

void OrderBook::add_ask_order(LimitOrder *order) {
  match_orders<LimitOrder::OrderType::Ask>(order);
}

const LimitOrder *OrderBook::get_best_bid_order() const {
  return bids.empty() ? nullptr : bids.top();
}

const LimitOrder *OrderBook::get_best_ask_order() const {
  return asks.empty() ? nullptr : asks.top();
}

} // namespace clob