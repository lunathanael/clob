/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#include <chrono>
#include <utility>

#include "clob/Market.h"
#include "clob/Stock.h"

namespace clob {

std::string Market::get_exchange_name() const { return exchange_name; }

std::string Market::get_exchange_ticker() const { return exchange_ticker; }

std::size_t Market::get_num_stocks() const { return stocks.size(); }

bool Market::add_stock(const std::string &stock_name,
                       const std::string &stock_ticker) {
  stocks.emplace_back(stock_name, stock_ticker,
                      static_cast<Stock::id_t>(get_num_stocks()));
  return true;
}

bool Market::add_stock(std::string &&stock_name, std::string &&stock_ticker) {
  stocks.emplace_back(std::move(stock_name), std::move(stock_ticker),
                      static_cast<Stock::id_t>(get_num_stocks()));
  return true;
}

template <clob::LimitOrder::OrderType order_type>
clob::LimitOrder::id_t Market::add_order(const clob::Stock::id_t stock_id, const clob::price_t price, const clob::quantity_t quantity) {
  auto ns = std::chrono::system_clock::now().time_since_epoch().count();
  clob::LimitOrder::id_t order_id = static_cast<LimitOrder::id_t>(orders.size());
  orders.emplace_back(static_cast<LimitOrder::id_t>(orders.size()), ns, price, quantity);
  if (stock_id >= order_books.size()) {
    orders.back().is_cancelled = true;
    return order_id;
  }
  if constexpr (order_type == LimitOrder::OrderType::Bid) {
    order_books[stock_id].add_bid_order(&orders.back());
  } else {
    order_books[stock_id].add_ask_order(&orders.back());
  }
  return order_id;
}

bool Market::cancel_order(const clob::LimitOrder::id_t order_id) {
  if (order_id >= orders.size()) {
    return false;
  }
  auto & order = orders[order_id];
  if (order.is_cancelled || order.filled_quantity == order.quantity) {
    return false;
  }
  order.is_cancelled = true;
  return true;
}

const LimitOrder *Market::query_order(const clob::LimitOrder::id_t order_id) const {
  if (order_id >= orders.size()) {
    return nullptr;
  }
  return &orders[order_id];
}

const OrderBook *Market::get_order_book(const clob::Stock::id_t stock_id) const {
  if (stock_id >= order_books.size()) {
    return nullptr;
  }
  return &order_books[stock_id];
}

} // namespace clob