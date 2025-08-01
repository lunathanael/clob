/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#include <type_traits>

#include "clob/OrderBook.h"

namespace clob {

template <OrderType order_type>
void OrderBook::match_orders(LimitOrder<order_type> *new_order) {
  static constexpr auto other_order_type =
      ternary<order_type == OrderType::Bid>(OrderType::Ask, OrderType::Bid);
  static constexpr const balance_t balance_sign{
      ternary<order_type == OrderType::Bid>(1, -1)};
  auto &order_book = ternary<order_type == OrderType::Bid>(asks, bids);
  auto &new_order_book = ternary<order_type == OrderType::Bid>(bids, asks);

  if (new_order->is_cancelled) {
    return;
  }

  quantity_t order_q{}, new_order_q{new_order->quantity};
  while (order_book.size() > 0 && new_order_q != 0) {
    const auto &order = order_book.top();
    if (order->is_cancelled) {
      order_book.pop();
      continue;
    }

    if (ternary<order_type == OrderType::Bid>(
            order->price > new_order->price, order->price < new_order->price)) {
      break;
    }

    order_q = order->quantity - order->filled_quantity;

    if (order_q <= new_order_q) {
      new_order->balance -= balance_sign * order_q * order->price;
      order->balance += balance_sign * order_q * order->price;
      new_order->filled_quantity += order_q;
      new_order_q -= order_q;
      order->filled_quantity = order->quantity;
      order_book.pop();
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
    new_order_book.push(new_order);
  }
}

template <OrderType order_type>
void OrderBook::add_order(LimitOrder<order_type> *order) {
  match_orders(order);
}

template void
OrderBook::add_order<OrderType::Bid>(LimitOrder<OrderType::Bid> *order);
template void
OrderBook::add_order<OrderType::Ask>(LimitOrder<OrderType::Ask> *order);

template <OrderType order_type>
const LimitOrder<order_type> *OrderBook::get_best_order() const {
  auto &order_book = ternary<order_type == OrderType::Bid>(bids, asks);
  return order_book.size() > 0 ? order_book.top() : nullptr;
}

template const LimitOrder<OrderType::Bid> *
OrderBook::get_best_order<OrderType::Bid>() const;
template const LimitOrder<OrderType::Ask> *
OrderBook::get_best_order<OrderType::Ask>() const;

} // namespace clob