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
  const std::string name;
  const std::string ticker;
  const uint_fast32_t id;
};

} // namespace clob