/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#include "clob/market/market.h"

namespace clob {

std::string Market::get_exchange_name() const { return exchange_name; }

std::string Market::get_exchange_ticker() const { return exchange_ticker; }

std::size_t Market::get_num_stocks() const { return stocks.size(); }

bool Market::add_stock(std::string stock_name, std::string stock_ticker) {
  stocks.emplace_back(stock_name, stock_ticker, static_cast<uint_fast32_t>(get_num_stocks()));
  return true;
}


} // namespace clob