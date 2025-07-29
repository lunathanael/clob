#include "doctest/doctest.h"

#include <cstdint>
#include <type_traits>
#include <utility>

#include "misc/TestUtilities.h"

#include "clob/Stock.h"

TEST_SUITE_BEGIN("Stock");

using namespace clob;

/***/
TEST_CASE("stock_construction") {
  Stock stock1{"stock1", "AAPL", 2};
  std::string stock2_name = "stock2";
  std::string stock2_ticker = "GOOG";
  Stock::id_t stock2_id = 1;
  Stock stock2{stock2_name, stock2_ticker, stock2_id};

  CHECK(stock1.name == "stock1");
  CHECK(stock1.ticker == "AAPL");
  CHECK(stock2.name == "stock2");
  CHECK(stock2.ticker == "GOOG");
  CHECK(stock1.id == 2);
  CHECK(stock2.id == stock2_id);
}

/***/
TEST_CASE("stock_move_constructor") {
  Stock stock1{"stock1", "AAPL", 1};
  Stock stock2{std::move(stock1)};
  CHECK(stock2.name == "stock1");
  CHECK(stock2.ticker == "AAPL");
  CHECK(stock2.id == 1);
}

/***/
TEST_CASE("stock_copy_constructors") {
  Stock stock1{"stock1", "AAPL", 1};
  Stock stock2{stock1};
  CHECK(stock2.name == "stock1");
  CHECK(stock2.ticker == "AAPL");
  CHECK(stock2.id == 1);
}

/***/
TEST_CASE("stock_types") {
  Stock stock{"stock", "AAPL", 2};

  REQUIRE(stock.name == "stock");
  REQUIRE(stock.ticker == "AAPL");
  REQUIRE(stock.id == 2);

  CHECK(std::is_same_v<decltype(stock.id), const Stock::id_t>);
  CHECK(sizeof(stock.id) >= sizeof(int32_t));
}

/***/
TEST_CASE("stock_assignment_deleted") {
  CHECK_FALSE(std::is_copy_assignable_v<Stock>);
  CHECK_FALSE(std::is_move_assignable_v<Stock>);
}

TEST_SUITE_END();