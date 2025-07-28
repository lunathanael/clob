#include "doctest/doctest.h"

#include "clob/LimitOrder.h"
#include "clob/Market.h"
#include "clob/OrderBook.h"
#include "clob/Stock.h"
#include "clob/types.h"
#include "misc/TestUtilities.h"

using namespace clob;

/***/
TEST_CASE("market_construction_basic") {
  Market market1("market1", "AAPL");
  Market market2("market2", "GOOG");

  CHECK(market1.get_exchange_name() == "market1");
  CHECK(market1.get_exchange_ticker() == "AAPL");
  CHECK(market2.get_exchange_name() == "market2");
  CHECK(market2.get_exchange_ticker() == "GOOG");
  CHECK(market1.get_num_stocks() == 0);
  CHECK(market2.get_num_stocks() == 0);
}