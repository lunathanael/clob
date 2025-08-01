#include "doctest/doctest.h"

#include <cstdint>
#include <type_traits>

#include "misc/TestUtilities.h"

#include "clob/LimitOrder.h"
#include "clob/Market.h"
#include "clob/types.h"

TEST_SUITE_BEGIN("Market");

using namespace clob;

/***/
TEST_CASE("market_construction") {
  Market market1{"nyse", "NYSE"};
  market1.add_stock("stock1", "AAPL");
  std::string market2_name = "stock2";
  std::string market2_ticker = "GOOG";
  Market market2{market2_name, market2_ticker};

  CHECK(market1.get_exchange_name() == "nyse");
  CHECK(market1.get_exchange_ticker() == "NYSE");
  CHECK(market2.get_exchange_name() == "stock2");
  CHECK(market2.get_exchange_ticker() == "GOOG");
  CHECK(market1.get_num_stocks() == 1);
  CHECK(market2.get_num_stocks() == 0);
}

/***/
TEST_CASE("market_construction_and_query") {
  Market market1{"nyse", "NYSE"};
  market1.add_stock("stock1", "AAPL");
  std::string market2_name = "stock2";
  std::string market2_ticker = "GOOG";
  Market market2{market2_name, market2_ticker};

  CHECK(market1.get_exchange_name() == "nyse");
  CHECK(market1.get_exchange_ticker() == "NYSE");
  CHECK(market2.get_exchange_name() == "stock2");
  CHECK(market2.get_exchange_ticker() == "GOOG");
  CHECK(market1.get_num_stocks() == 1);
  CHECK(market2.get_num_stocks() == 0);
  CHECK(market1.get_order_book(0) != nullptr);
  CHECK(market1.get_order_book(0)->get_best_order<OrderType::Bid>() == nullptr);
  CHECK(market1.get_order_book(0)->get_best_order<OrderType::Ask>() == nullptr);
}

/***/
TEST_CASE("market_copy_constructor") {
  Market market1{"nyse", "NYSE"};
  CHECK_FALSE(std::is_copy_constructible_v<Market>);
  CHECK_FALSE(std::is_copy_assignable_v<Market>);
  CHECK_FALSE(std::is_move_constructible_v<Market>);
  CHECK_FALSE(std::is_move_assignable_v<Market>);
}

/***/
TEST_CASE("market_add_stock") {
  Market market{"nyse", "NYSE"};
  CHECK(market.get_num_stocks() == 0);
  CHECK(market.add_stock("stock2", "GOOG"));
  CHECK(market.get_num_stocks() == 1);
  CHECK(market.add_stock("stock3", "MSFT"));
  CHECK(market.get_num_stocks() == 2);
  CHECK(market.add_stock("stock4", "AMZN"));
  CHECK(market.get_num_stocks() == 3);
  CHECK(market.add_stock("stock5", "TSLA"));
  CHECK(market.get_num_stocks() == 4);
  CHECK(market.get_order_book(0) != nullptr);
  CHECK(market.get_order_book(1) != nullptr);
  CHECK(market.get_order_book(2) != nullptr);
  CHECK(market.get_order_book(3) != nullptr);
  CHECK(market.get_order_book(4) == nullptr);
  std::string stock6_name = "stock6";
  std::string stock6_ticker = "NVDA";
  CHECK(market.add_stock(stock6_name, stock6_ticker));
  CHECK(market.get_num_stocks() == 5);
  CHECK(market.add_stock(std::move(stock6_name), std::move(stock6_ticker)));
  CHECK(market.get_num_stocks() == 6);
}

/***/
TEST_CASE("market_add_order") {
  Market market{"nyse", "NYSE"};
  market.add_stock("stock1", "AAPL");
  REQUIRE(market.get_num_stocks() == 1);
  CHECK(market.add_order<OrderType::Bid>(0, 100, 100) == 0);
  CHECK(market.add_order<OrderType::Ask>(0, 101, 100) ==
        Market::ask_order_flag);
  CHECK(market.add_order<OrderType::Bid>(0, 102, 100) == 1);
  REQUIRE(market.query_order<OrderType::Bid>(0) != nullptr);
  REQUIRE(market.query_order<OrderType::Ask>(Market::ask_order_flag) !=
          nullptr);
  REQUIRE(market.query_order<OrderType::Bid>(1) != nullptr);
  CHECK(market.query_order<OrderType::Bid>(0)->price == 100);
  CHECK(market.query_order<OrderType::Ask>(Market::ask_order_flag)->price ==
        101);
  CHECK(market.query_order<OrderType::Bid>(1)->price == 102);
  CHECK(market.query_order<OrderType::Bid>(0)->is_cancelled == false);
  CHECK(market.query_order<OrderType::Ask>(Market::ask_order_flag)
            ->is_cancelled == false);
  CHECK(market.query_order<OrderType::Bid>(1)->is_cancelled == false);
}

/***/
TEST_CASE("market_add_invalid_order") {
  Market market{"nyse", "NYSE"};
  market.add_stock("stock1", "AAPL");
  REQUIRE(market.get_num_stocks() == 1);
  CHECK(market.add_order<OrderType::Bid>(0, 100, 100) == 0);
  CHECK(market.add_order<OrderType::Ask>(0, 101, 100) ==
        Market::ask_order_flag);
  CHECK(market.add_order<OrderType::Bid>(0, 102, 100) == 1);
  REQUIRE(market.query_order<OrderType::Bid>(0) != nullptr);
  CHECK(market.query_order<OrderType::Bid>(0)->is_cancelled == false);
  REQUIRE(market.get_order_book(1) == nullptr);
  CHECK(market.add_order<OrderType::Bid>(1, 103, 100) == 2);
  REQUIRE(market.query_order<OrderType::Bid>(2) != nullptr);
  CHECK(market.query_order<OrderType::Bid>(2)->is_cancelled == true);
  CHECK(market.add_order<OrderType::Bid>(2, 103, 100) == 3);
  REQUIRE(market.query_order<OrderType::Bid>(3) != nullptr);
  CHECK(market.query_order<OrderType::Bid>(3)->is_cancelled == true);
  CHECK(market.add_order<OrderType::Ask>(3, 104, 100) ==
        Market::ask_order_flag + 1);
  REQUIRE(market.query_order<OrderType::Ask>(Market::ask_order_flag + 1) !=
          nullptr);
  CHECK(market.query_order<OrderType::Ask>(Market::ask_order_flag + 1)
            ->is_cancelled == true);
}

/***/
TEST_CASE("market_cancel_order") {
  Market market{"nyse", "NYSE"};
  market.add_stock("stock2", "GOOG");
  CHECK(market.cancel_order<OrderType::Bid>(0) == false);
  CHECK(market.add_order<OrderType::Bid>(0, 100, 100) == 0);
  CHECK(market.cancel_order<OrderType::Bid>(0) == true);
  CHECK(market.cancel_order<OrderType::Bid>(0) == false);
  REQUIRE(market.query_order<OrderType::Bid>(0) != nullptr);
  CHECK(market.query_order<OrderType::Bid>(0)->is_cancelled == true);
  CHECK(market.add_order<OrderType::Bid>(0, 100, 100) == 1);
  CHECK(market.add_order<OrderType::Ask>(0, 100, 100) ==
        Market::ask_order_flag);
  REQUIRE(market.query_order<OrderType::Ask>(Market::ask_order_flag) !=
          nullptr);
  CHECK(market.query_order<OrderType::Ask>(Market::ask_order_flag)
            ->is_cancelled == false);
  CHECK(market.cancel_order<OrderType::Ask>(Market::ask_order_flag) == false);
  REQUIRE(market.query_order<OrderType::Ask>(Market::ask_order_flag) !=
          nullptr);
  CHECK(market.query_order<OrderType::Ask>(Market::ask_order_flag)
            ->is_cancelled == false);
}

/***/
TEST_CASE("market_query_order") {
  Market market{"nyse", "NYSE"};
  market.add_stock("stock1", "AAPL");
  market.add_stock("stock2", "GOOG");
  CHECK(market.query_order<OrderType::Bid>(0) == nullptr);
  CHECK(market.add_order<OrderType::Bid>(0, 100, 100) == 0);
  REQUIRE(market.query_order<OrderType::Bid>(0) != nullptr);
  auto order = market.query_order<OrderType::Bid>(0);
  CHECK(order->id == 0);
  CHECK(order->price == 100);
  CHECK(order->quantity == 100);
  CHECK(order->is_cancelled == false);
}

/***/
TEST_CASE("market_get_order_book") {
  Market market{"nyse", "NYSE"};
  market.add_stock("stock1", "AAPL");
  market.add_stock("stock2", "GOOG");
  CHECK(market.get_order_book(0) != nullptr);
  CHECK(market.get_order_book(1) != nullptr);
  CHECK(market.get_order_book(2) == nullptr);
  CHECK(market.add_order<OrderType::Bid>(0, 100, 100) == 0);
  REQUIRE(market.get_order_book(0) != nullptr);
  REQUIRE(market.get_order_book(1) != nullptr);
  REQUIRE(market.get_order_book(2) == nullptr);
  auto order_book = market.get_order_book(0);
  REQUIRE(order_book->get_best_order<OrderType::Bid>() != nullptr);
  CHECK(order_book->get_best_order<OrderType::Bid>()->id == 0);
  CHECK(order_book->get_best_order<OrderType::Bid>()->price == 100);
  CHECK(order_book->get_best_order<OrderType::Bid>()->quantity == 100);
  CHECK(order_book->get_best_order<OrderType::Ask>() == nullptr);
  CHECK(order_book->get_best_order<OrderType::Bid>()->is_cancelled == false);
  CHECK(order_book->get_best_order<OrderType::Ask>() == nullptr);
}

TEST_SUITE_END();