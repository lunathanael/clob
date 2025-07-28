/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#pragma once

#include <cstdint>
#include <string>

namespace clob {

class Stock {
public:
  using id_t = uint_fast32_t;
  const std::string name;
  const std::string ticker;
  const Stock::id_t id;
};

} // namespace clob