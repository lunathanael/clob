/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#include <chrono>
#include <memory>
#include <utility>

#include "clob/LimitOrder.h"
#include "clob/Market.h"

namespace clob {

std::string Market::get_exchange_name() const { return exchange_name; }

std::string Market::get_exchange_ticker() const { return exchange_ticker; }

std::size_t Market::get_num_stocks() const { return stocks.size(); }

bool Market::add_stock(const std::string &stock_name,
                       const std::string &stock_ticker) {
  stocks.emplace_back(stock_name, stock_ticker,
                      static_cast<StockId_t>(get_num_stocks()));
  order_books.emplace_back();
  return true;
}

bool Market::add_stock(std::string &&stock_name, std::string &&stock_ticker) {
  stocks.emplace_back(std::move(stock_name), std::move(stock_ticker),
                      static_cast<StockId_t>(get_num_stocks()));
  order_books.emplace_back();
  return true;
}

template <OrderType order_type>
LimitOrderId_t Market::add_order(const StockId_t stock_id, const price_t price,
                                 const quantity_t quantity) {
  auto ns = std::chrono::system_clock::now().time_since_epoch().count();
  static auto &orders =
      ternary<order_type == OrderType::Bid>(bid_orders, ask_orders);
  const LimitOrderId_t order_id = ternary<order_type == OrderType::Bid>(
      bid_orders.size(), ask_orders.size() + ask_order_flag);
  orders.emplace_back(
      std::make_unique<LimitOrder<order_type>>(order_id, ns, price, quantity));
  if (stock_id >= order_books.size()) {
    orders.back()->is_cancelled = true;
    return order_id;
  }
  order_books[stock_id].add_order(orders.back().get());
  return order_id;
}

template LimitOrderId_t Market::add_order<OrderType::Bid>(const StockId_t,
                                                          const price_t,
                                                          const quantity_t);
template LimitOrderId_t Market::add_order<OrderType::Ask>(const StockId_t,
                                                          const price_t,
                                                          const quantity_t);

template <OrderType order_type>
bool Market::cancel_order(LimitOrderId_t order_id) {
  LimitOrder<order_type> *order;
  if constexpr (order_type == OrderType::Bid) {
    if (order_id >= bid_orders.size()) {
      return false;
    }
    order = bid_orders[order_id].get();
  } else {
    order_id ^= ask_order_flag;
    if (order_id >= ask_orders.size()) {
      return false;
    }
    order = ask_orders[order_id].get();
  }
  if (order->is_cancelled || order->filled_quantity == order->quantity) {
    return false;
  }
  order->is_cancelled = true;
  return true;
}

template bool
Market::cancel_order<OrderType::Bid>(const LimitOrderId_t order_id);
template bool
Market::cancel_order<OrderType::Ask>(const LimitOrderId_t order_id);

template <OrderType order_type>
const LimitOrder<order_type> *
Market::query_order(LimitOrderId_t order_id) const {
  static auto &orders =
      ternary<order_type == OrderType::Bid>(bid_orders, ask_orders);
  if constexpr (order_type == OrderType::Ask)
    order_id ^= ask_order_flag;
  if (order_id >= orders.size()) {
    return nullptr;
  }
  return orders[order_id].get();
}

template const LimitOrder<OrderType::Bid> *
Market::query_order<OrderType::Bid>(const LimitOrderId_t order_id) const;

template const LimitOrder<OrderType::Ask> *
Market::query_order<OrderType::Ask>(const LimitOrderId_t order_id) const;

const OrderBook *Market::get_order_book(const StockId_t stock_id) const {
  if (stock_id >= order_books.size()) {
    return nullptr;
  }
  return &order_books[stock_id];
}

} // namespace clob