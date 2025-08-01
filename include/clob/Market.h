/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "clob/LimitOrder.h"
#include "clob/OrderBook.h"
#include "clob/Stock.h"
#include "clob/types.h"

namespace clob {

/**
 * @brief Interface for the Market Engine.
 *
 * @details This class is responsible for managing the entire market engine and
 * the stocks it holds. It exposes interfaces for adding and interacting
 * with the stocks.
 */
class Market {
  const std::string exchange_name;
  const std::string exchange_ticker;
  std::vector<Stock> stocks;
  std::vector<OrderBook> order_books;
  std::vector<std::unique_ptr<LimitOrder<OrderType::Bid>>> bid_orders;
  std::vector<std::unique_ptr<LimitOrder<OrderType::Ask>>> ask_orders;

public:
  static constexpr LimitOrderId_t ask_order_flag = (1UL << 63);
  Market() = delete;
  Market(const Market &) = delete;
  Market(Market &&) = delete;
  Market &operator=(const Market &) = delete;
  Market &operator=(Market &&) = delete;

  /**
   * @brief Constructor for the Market class.
   *
   * @param exchange_name The name of the market.
   * @param exchange_ticker The ticker of the market.
   */
  Market(const std::string &exchange_name, const std::string &exchange_ticker)
      : exchange_name(exchange_name), exchange_ticker(exchange_ticker) {}

  /**
   * @brief Constructor for the Market class.
   *
   * @param name The name of the market.
   * @param ticker The ticker of the market.
   */
  Market(std::string &&exchange_name, std::string &&exchange_ticker)
      : exchange_name(std::move(exchange_name)),
        exchange_ticker(std::move(exchange_ticker)) {}

  /**
   * @brief Get the name of the market.
   *
   * @return The name of the market.
   */
  std::string get_exchange_name() const;

  /**
   * @brief Get the ticker of the market.
   *
   * @return The ticker of the market.
   */
  std::string get_exchange_ticker() const;

  /**
   * @brief Get the number of stocks in the market.
   *
   * @return The number of stocks in the market.
   */
  std::size_t get_num_stocks() const;

  /**
   * @brief Add a stock to the market.
   *
   * @param stock The stock to add.
   */
  bool add_stock(const std::string &stock_name,
                 const std::string &stock_ticker);

  /**
   * @brief Add a stock to the market.
   *
   * @param stock The stock to add.
   */
  bool add_stock(std::string &&stock_name, std::string &&stock_ticker);

  /**
   * @brief Add an order to the market.
   *
   * @param order The order to add.
   */
  template <OrderType order_type>
  LimitOrderId_t add_order(const StockId_t stock_id, const price_t price,
                           const quantity_t quantity);

  /**
   * @brief Cancel an order.
   *
   * @param order_id The id of the order to cancel.
   */
  template <OrderType order_type>
  bool cancel_order(const LimitOrderId_t order_id);

  /**
   * @brief Query an order.
   *
   * @param order_id The id of the order to query.
   */
  template <OrderType order_type>
  const LimitOrder<order_type> *
  query_order(const LimitOrderId_t order_id) const;

  /**
   * @brief Get the order book for a stock.
   *
   * @param stock_id The id of the stock to get the order book for.
   */
  const OrderBook *get_order_book(const StockId_t stock_id) const;
};

} // namespace clob