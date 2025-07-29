#include "doctest/doctest.h"

#include <cstdint>
#include <type_traits>

#include "misc/TestUtilities.h"

#include "clob/Market.h"

TEST_SUITE_BEGIN("Market");

using namespace clob;

/***/
TEST_CASE("market_construction") {
  Market market1{"stock1", "AAPL"};
  std::string market2_name = "stock2";
  std::string market2_ticker = "GOOG";
  Market market2{market2_name, market2_ticker};

  CHECK(market1.get_exchange_name() == "stock1");
  CHECK(market1.get_exchange_ticker() == "AAPL");
  CHECK(market2.get_exchange_name() == "stock2");
  CHECK(market2.get_exchange_ticker() == "GOOG");
  CHECK(market1.get_num_stocks() == 0);
  CHECK(market2.get_num_stocks() == 0);
}

/***/
TEST_CASE("market_copy_constructor") {
  Market market1{"stock1", "AAPL"};
  CHECK_FALSE(std::is_copy_constructible_v<Market>);
  CHECK_FALSE(std::is_copy_assignable_v<Market>);
  CHECK_FALSE(std::is_move_constructible_v<Market>);
  CHECK_FALSE(std::is_move_assignable_v<Market>);
}

/***/
TEST_CASE("market_add_stock") {
  Market market{"stock1", "AAPL"};
  CHECK(market.get_num_stocks() == 0);
  CHECK(market.add_stock("stock2", "GOOG"));
  CHECK(market.get_num_stocks() == 1);
  CHECK(market.add_stock("stock3", "MSFT"));
  CHECK(market.get_num_stocks() == 2);
  CHECK(market.add_stock("stock4", "AMZN"));
  CHECK(market.get_num_stocks() == 3);
  CHECK(market.add_stock("stock5", "TSLA"));
  CHECK(market.get_num_stocks() == 4);
  std::string stock6_name = "stock6";
  std::string stock6_ticker = "NVDA";
  CHECK(market.add_stock(stock6_name, stock6_ticker));
  CHECK(market.get_num_stocks() == 5);
  CHECK(market.add_stock(std::move(stock6_name), std::move(stock6_ticker)));
  CHECK(market.get_num_stocks() == 6);
}

TEST_SUITE_END();