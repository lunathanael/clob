/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <utility>
#include <vector>

#include "clob/OrderBook.h"
#include "clob/Stock.h"

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
  clob::LimitOrder::id_t next_order_id;

public:
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
      : exchange_name(exchange_name), exchange_ticker(exchange_ticker),
        next_order_id(0) {}

  /**
   * @brief Constructor for the Market class.
   *
   * @param name The name of the market.
   * @param ticker The ticker of the market.
   */
  Market(std::string &&exchange_name, std::string &&exchange_ticker)
      : exchange_name(std::move(exchange_name)),
        exchange_ticker(std::move(exchange_ticker)), next_order_id(0) {}

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
};

} // namespace clob