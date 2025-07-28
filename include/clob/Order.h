/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#pragma once

#include "clob/Stock.h"
#include "clob/types.h"

namespace clob {

class Order {
public:
  using id_t = uint_fast64_t;
  const id_t id;
  const timestamp_ns_t timestamp;
  const Stock::id_t stock_id;
  const price_t price;
  const quantity_t quantity;
};

} // namespace clob