#include "doctest/doctest.h"

#include "misc/TestUtilities.h"

#include "clob/Stock.h"

TEST_SUITE_BEGIN("Stock");

using namespace clob;

/***/
TEST_CASE("stock_construction") {
  Stock stock1{"stock1", "AAPL", 0};
  Stock stock2{"stock2", "GOOG", 1};

  CHECK(stock1.name == "stock1");
  CHECK(stock1.ticker == "AAPL");
  CHECK(stock2.name == "stock2");
  CHECK(stock2.ticker == "GOOG");
  CHECK(stock1.id == 0);
  CHECK(stock2.id == 1);
}

/***/
TEST_CASE("stock_move_constructor") {
  Stock stock1{"stock1", "AAPL", 1};
  Stock stock2{std::move(stock1)};
  CHECK(stock2.name == "stock1");
  CHECK(stock2.ticker == "AAPL");
  CHECK(stock2.id == 1);
}

TEST_SUITE_END();