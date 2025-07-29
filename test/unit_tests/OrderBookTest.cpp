#include "doctest/doctest.h"

#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>

#include "misc/TestUtilities.h"

#include "clob/OrderBook.h"

TEST_SUITE_BEGIN("OrderBook");

using namespace clob;

/***/
TEST_CASE("order_book_construction") {
  OrderBook order_book;

  CHECK(order_book.bids.empty());
  CHECK(order_book.asks.empty());
}

/***/
TEST_CASE("order_book_add_bid_order") {
  OrderBook order_book;

  auto bid_order = std::make_unique<LimitOrder>(1, 1000, 15000, 100);
  order_book.add_bid_order(bid_order.get());

  CHECK(order_book.bids.size() == 1);
  CHECK(order_book.asks.empty());

  const LimitOrder *top_bid = order_book.bids.top();
  CHECK(top_bid->id == 1);
  CHECK(top_bid->timestamp == 1000);
  CHECK(top_bid->price == 15000);
  CHECK(top_bid->quantity == 100);
}

/***/
TEST_CASE("order_book_add_ask_order") {
  OrderBook order_book;

  auto ask_order = std::make_unique<LimitOrder>(2, 2000, 16000, 200);
  order_book.add_ask_order(ask_order.get());

  CHECK(order_book.asks.size() == 1);
  CHECK(order_book.bids.empty());

  const LimitOrder *top_ask = order_book.asks.top();
  CHECK(top_ask->id == 2);
  CHECK(top_ask->timestamp == 2000);
  CHECK(top_ask->price == 16000);
  CHECK(top_ask->quantity == 200);
}

/***/
TEST_CASE("order_book_multiple_orders") {
  OrderBook order_book;

  auto bid1 = std::make_unique<LimitOrder>(1, 1000, 15000, 100);
  auto bid2 = std::make_unique<LimitOrder>(2, 1100, 14900, 150);
  auto bid3 = std::make_unique<LimitOrder>(3, 1200, 15100, 75);
  order_book.add_bid_order(bid1.get());
  order_book.add_bid_order(bid2.get());
  order_book.add_bid_order(bid3.get());

  auto ask1 = std::make_unique<LimitOrder>(4, 1300, 16000, 200);
  auto ask2 = std::make_unique<LimitOrder>(5, 1400, 15900, 125);
  auto ask3 = std::make_unique<LimitOrder>(6, 1500, 16100, 300);
  order_book.add_ask_order(ask1.get());
  order_book.add_ask_order(ask2.get());
  order_book.add_ask_order(ask3.get());

  CHECK(order_book.bids.size() == 3);
  CHECK(order_book.asks.size() == 3);
}

/***/
TEST_CASE("order_book_bid_priority_queue") {
  OrderBook order_book;

  auto bid1 = std::make_unique<LimitOrder>(1, 1000, 15000, 100);
  auto bid2 = std::make_unique<LimitOrder>(2, 1100, 15100, 150);
  auto bid3 = std::make_unique<LimitOrder>(3, 1200, 15000, 75);
  order_book.add_bid_order(bid1.get());
  order_book.add_bid_order(bid2.get());
  order_book.add_bid_order(bid3.get());

  LimitOrder *top_bid = order_book.bids.top();
  CHECK(top_bid->id == 2);
  CHECK(top_bid->price == 15100);
  CHECK(top_bid->timestamp == 1100);

  order_book.bids.pop();
  top_bid = order_book.bids.top();
  CHECK(top_bid->id == 1);
  CHECK(top_bid->price == 15000);
  CHECK(top_bid->timestamp == 1000);

  order_book.bids.pop();
  top_bid = order_book.bids.top();
  CHECK(top_bid->id == 3);
  CHECK(top_bid->price == 15000);
  CHECK(top_bid->timestamp == 1200);
}

/***/
TEST_CASE("order_book_ask_priority_queue") {
  OrderBook order_book;

  auto ask1 = std::make_unique<LimitOrder>(1, 1000, 16000, 100);
  auto ask2 = std::make_unique<LimitOrder>(2, 1100, 15900, 150);
  auto ask3 = std::make_unique<LimitOrder>(3, 1200, 16000, 75);
  order_book.add_ask_order(ask1.get());
  order_book.add_ask_order(ask2.get());
  order_book.add_ask_order(ask3.get());

  const LimitOrder *top_ask = order_book.asks.top();
  CHECK(top_ask->id == 2);
  CHECK(top_ask->price == 15900);
  CHECK(top_ask->timestamp == 1100);

  order_book.asks.pop();
  const LimitOrder *top_ask2 = order_book.asks.top();
  CHECK(top_ask2->id == 1);
  CHECK(top_ask2->price == 16000);
  CHECK(top_ask2->timestamp == 1000);

  order_book.asks.pop();
  const LimitOrder *top_ask3 = order_book.asks.top();
  CHECK(top_ask3->id == 3);
  CHECK(top_ask3->price == 16000);
  CHECK(top_ask3->timestamp == 1200);
}

/***/
TEST_CASE("order_book_types") {
  OrderBook order_book;

  CHECK(std::is_same_v<
        decltype(order_book.bids),
        std::priority_queue<LimitOrder *, std::vector<LimitOrder *>,
                            LimitOrder::PriceTimeQueuePriority::BidCmp>>);
  CHECK(std::is_same_v<
        decltype(order_book.asks),
        std::priority_queue<LimitOrder *, std::vector<LimitOrder *>,
                            LimitOrder::PriceTimeQueuePriority::AskCmp>>);
}

TEST_SUITE_END();
