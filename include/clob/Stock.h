/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <utility>

#include "clob/types.h"

namespace clob {

/**
 * @brief A class representing a stock
 *
 * @details Includes the name, ticker, and id of the stock.
 */
class Stock {
public:
  const std::string name;
  const std::string ticker;
  const StockId_t id;

  /**
   * @brief Construct a new Stock object
   *
   * @param name The name of the stock
   * @param ticker The ticker of the stock
   * @param id The id of the stock
   */
  Stock(const std::string &name, const std::string &ticker, const StockId_t id)
      : name(name), ticker(ticker), id(id) {}

  /**
   * @brief Construct a new Stock object
   *
   * @param name The name of the stock
   * @param ticker The ticker of the stock
   * @param id The id of the stock
   */
  Stock(std::string &&name, std::string &&ticker, const StockId_t id)
      : name(std::move(name)), ticker(std::move(ticker)), id(id) {}

  Stock(const Stock &) = default;
  Stock(Stock &&) = default;
  ~Stock() = default;

  Stock() = delete;
  Stock &operator=(const Stock &) = delete;
  Stock &operator=(Stock &&) = delete;
};

} // namespace clob