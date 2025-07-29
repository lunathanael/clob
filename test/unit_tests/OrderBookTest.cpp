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

  CHECK(order_book.pending_orders.empty());
  CHECK(order_book.bids.empty());
  CHECK(order_book.asks.empty());
}

/***/
TEST_CASE("order_book_add_bid_order") {
  OrderBook order_book;

  auto bid_order = std::make_unique<LimitOrder>(1, 1000, 15000, 100);
  order_book.add_bid_order(bid_order.get());

  CHECK(order_book.pending_orders.size() == 1);
  CHECK(order_book.pending_orders.find(1) != order_book.pending_orders.end());
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

  CHECK(order_book.pending_orders.size() == 1);
  CHECK(order_book.pending_orders.find(2) != order_book.pending_orders.end());
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

  CHECK(order_book.pending_orders.size() == 6);
  CHECK(order_book.bids.size() == 3);
  CHECK(order_book.asks.size() == 3);

  for (LimitOrder::id_t id = 1; id <= 6; ++id) {
    CHECK(order_book.pending_orders.find(id) !=
          order_book.pending_orders.end());
  }
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
TEST_CASE("order_book_cancel_order_success") {
  OrderBook order_book;

  auto bid1 = std::make_unique<LimitOrder>(1, 1000, 15000, 100);
  auto ask1 = std::make_unique<LimitOrder>(2, 2000, 16000, 200);
  auto bid2 = std::make_unique<LimitOrder>(3, 3000, 14900, 150);
  order_book.add_bid_order(bid1.get());
  order_book.add_ask_order(ask1.get());
  order_book.add_bid_order(bid2.get());

  CHECK(order_book.pending_orders.size() == 3);

  bool cancelled = order_book.cancel_order(2);
  CHECK(cancelled == true);
  CHECK(order_book.pending_orders.size() == 2);
  CHECK(order_book.pending_orders.find(2) == order_book.pending_orders.end());
  CHECK(order_book.pending_orders.find(1) != order_book.pending_orders.end());
  CHECK(order_book.pending_orders.find(3) != order_book.pending_orders.end());
}

/***/
TEST_CASE("order_book_cancel_order_failure") {
  OrderBook order_book;

  auto bid1 = std::make_unique<LimitOrder>(1, 1000, 15000, 100);
  auto ask1 = std::make_unique<LimitOrder>(2, 2000, 16000, 200);
  order_book.add_bid_order(bid1.get());
  order_book.add_ask_order(ask1.get());

  CHECK(order_book.pending_orders.size() == 2);

  bool cancelled = order_book.cancel_order(999);
  CHECK(cancelled == false);
  CHECK(order_book.pending_orders.size() == 2);
  CHECK(order_book.pending_orders.find(1) != order_book.pending_orders.end());
  CHECK(order_book.pending_orders.find(2) != order_book.pending_orders.end());
}

/***/
TEST_CASE("order_book_cancel_all_orders") {
  OrderBook order_book;

  auto bid1 = std::make_unique<LimitOrder>(1, 1000, 15000, 100);
  auto ask1 = std::make_unique<LimitOrder>(2, 2000, 16000, 200);
  auto bid2 = std::make_unique<LimitOrder>(3, 3000, 14900, 150);
  auto ask2 = std::make_unique<LimitOrder>(4, 4000, 16100, 300);
  order_book.add_bid_order(bid1.get());
  order_book.add_ask_order(ask1.get());
  order_book.add_bid_order(bid2.get());
  order_book.add_ask_order(ask2.get());

  CHECK(order_book.pending_orders.size() == 4);

  CHECK(order_book.cancel_order(1) == true);
  CHECK(order_book.pending_orders.size() == 3);

  CHECK(order_book.cancel_order(2) == true);
  CHECK(order_book.pending_orders.size() == 2);

  CHECK(order_book.cancel_order(3) == true);
  CHECK(order_book.pending_orders.size() == 1);

  CHECK(order_book.cancel_order(4) == true);
  CHECK(order_book.pending_orders.size() == 0);
  CHECK(order_book.pending_orders.empty());
}

/***/
TEST_CASE("order_book_edge_cases") {
  OrderBook order_book;

  auto zero_order = std::make_unique<LimitOrder>(0, 0, 0, 0);
  order_book.add_bid_order(zero_order.get());
  CHECK(order_book.pending_orders.size() == 1);
  CHECK(order_book.pending_orders.find(0) != order_book.pending_orders.end());

  auto max_order = std::make_unique<LimitOrder>(
      UINT_FAST64_MAX, UINT_FAST64_MAX, UINT_FAST32_MAX, UINT_FAST32_MAX);
  order_book.add_ask_order(max_order.get());
  CHECK(order_book.pending_orders.size() == 2);
  CHECK(order_book.pending_orders.find(UINT_FAST64_MAX) !=
        order_book.pending_orders.end());

  CHECK(order_book.cancel_order(0) == true);
  CHECK(order_book.pending_orders.size() == 1);

  CHECK(order_book.cancel_order(UINT_FAST64_MAX) == true);
  CHECK(order_book.pending_orders.size() == 0);
}

/***/
TEST_CASE("order_book_types") {
  OrderBook order_book;

  CHECK(std::is_same_v<decltype(order_book.pending_orders),
                       std::unordered_set<LimitOrder::id_t>>);
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
