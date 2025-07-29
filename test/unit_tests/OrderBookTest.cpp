#include "doctest/doctest.h"

#include <cstdint>
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

  LimitOrder bid_order{1, 1000, 15000, 100};
  order_book.add_bid_order(std::move(bid_order));

  CHECK(order_book.pending_orders.size() == 1);
  CHECK(order_book.pending_orders.find(1) != order_book.pending_orders.end());
  CHECK(order_book.bids.size() == 1);
  CHECK(order_book.asks.empty());

  // Check that the order was added to bids with correct priority
  const LimitOrder &top_bid = order_book.bids.top();
  CHECK(top_bid.id == 1);
  CHECK(top_bid.timestamp == 1000);
  CHECK(top_bid.price == 15000);
  CHECK(top_bid.quantity == 100);
}

/***/
TEST_CASE("order_book_add_ask_order") {
  OrderBook order_book;

  LimitOrder ask_order{2, 2000, 16000, 200};
  order_book.add_ask_order(std::move(ask_order));

  CHECK(order_book.pending_orders.size() == 1);
  CHECK(order_book.pending_orders.find(2) != order_book.pending_orders.end());
  CHECK(order_book.asks.size() == 1);
  CHECK(order_book.bids.empty());

  // Check that the order was added to asks with correct priority
  const LimitOrder &top_ask = order_book.asks.top();
  CHECK(top_ask.id == 2);
  CHECK(top_ask.timestamp == 2000);
  CHECK(top_ask.price == 16000);
  CHECK(top_ask.quantity == 200);
}

/***/
TEST_CASE("order_book_multiple_orders") {
  OrderBook order_book;

  // Add multiple bid orders
  order_book.add_bid_order(LimitOrder{1, 1000, 15000, 100});
  order_book.add_bid_order(LimitOrder{2, 1100, 14900, 150});
  order_book.add_bid_order(LimitOrder{3, 1200, 15100, 75});

  // Add multiple ask orders
  order_book.add_ask_order(LimitOrder{4, 1300, 16000, 200});
  order_book.add_ask_order(LimitOrder{5, 1400, 15900, 125});
  order_book.add_ask_order(LimitOrder{6, 1500, 16100, 300});

  CHECK(order_book.pending_orders.size() == 6);
  CHECK(order_book.bids.size() == 3);
  CHECK(order_book.asks.size() == 3);

  // Check that all order IDs are in pending_orders
  for (LimitOrder::id_t id = 1; id <= 6; ++id) {
    CHECK(order_book.pending_orders.find(id) !=
          order_book.pending_orders.end());
  }
}

/***/
TEST_CASE("order_book_bid_priority_queue") {
  OrderBook order_book;

  // Add bids with different prices and timestamps
  order_book.add_bid_order(
      LimitOrder{1, 1000, 15000, 100}); // Lower price, earlier time
  order_book.add_bid_order(
      LimitOrder{2, 1100, 15100, 150}); // Higher price, later time
  order_book.add_bid_order(
      LimitOrder{3, 1200, 15000, 75}); // Same price, later time

  // The highest priority bid should be the one with highest price (15100) and
  // earliest timestamp (1100)
  LimitOrder top_bid = order_book.bids.top();
  CHECK(top_bid.id == 2);
  CHECK(top_bid.price == 15100);
  CHECK(top_bid.timestamp == 1100);

  // Pop the top bid and check the next one
  order_book.bids.pop();
  top_bid = order_book.bids.top();
  CHECK(top_bid.id == 1); // Same price but earlier timestamp
  CHECK(top_bid.price == 15000);
  CHECK(top_bid.timestamp == 1000);

  // Pop again and check the lower price bid
  order_book.bids.pop();
  top_bid = order_book.bids.top();
  CHECK(top_bid.id == 3);
  CHECK(top_bid.price == 15000);
  CHECK(top_bid.timestamp == 1200);
}

/***/
TEST_CASE("order_book_ask_priority_queue") {
  OrderBook order_book;

  // Add asks with different prices and timestamps
  order_book.add_ask_order(
      LimitOrder{1, 1000, 16000, 100}); // Higher price, earlier time
  order_book.add_ask_order(
      LimitOrder{2, 1100, 15900, 150}); // Lower price, later time
  order_book.add_ask_order(
      LimitOrder{3, 1200, 16000, 75}); // Same price, later time

  // The highest priority ask should be the one with lowest price (15900) and
  // earliest timestamp (1100)
  const LimitOrder &top_ask = order_book.asks.top();
  CHECK(top_ask.id == 2);
  CHECK(top_ask.price == 15900);
  CHECK(top_ask.timestamp == 1100);

  // Pop the top ask and check the next one
  order_book.asks.pop();
  const LimitOrder &top_ask2 = order_book.asks.top();
  CHECK(top_ask2.id == 1); // Same price but earlier timestamp
  CHECK(top_ask2.price == 16000);
  CHECK(top_ask2.timestamp == 1000);

  // Pop again and check the higher price ask
  order_book.asks.pop();
  const LimitOrder &top_ask3 = order_book.asks.top();
  CHECK(top_ask3.id == 3);
  CHECK(top_ask3.price == 16000);
  CHECK(top_ask3.timestamp == 1200);
}

/***/
TEST_CASE("order_book_cancel_order_success") {
  OrderBook order_book;

  // Add some orders
  order_book.add_bid_order(LimitOrder{1, 1000, 15000, 100});
  order_book.add_ask_order(LimitOrder{2, 2000, 16000, 200});
  order_book.add_bid_order(LimitOrder{3, 3000, 14900, 150});

  CHECK(order_book.pending_orders.size() == 3);

  // Cancel an existing order
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

  // Add some orders
  order_book.add_bid_order(LimitOrder{1, 1000, 15000, 100});
  order_book.add_ask_order(LimitOrder{2, 2000, 16000, 200});

  CHECK(order_book.pending_orders.size() == 2);

  // Try to cancel a non-existent order
  bool cancelled = order_book.cancel_order(999);
  CHECK(cancelled == false);
  CHECK(order_book.pending_orders.size() == 2);
  CHECK(order_book.pending_orders.find(1) != order_book.pending_orders.end());
  CHECK(order_book.pending_orders.find(2) != order_book.pending_orders.end());
}

/***/
TEST_CASE("order_book_cancel_all_orders") {
  OrderBook order_book;

  // Add multiple orders
  order_book.add_bid_order(LimitOrder{1, 1000, 15000, 100});
  order_book.add_ask_order(LimitOrder{2, 2000, 16000, 200});
  order_book.add_bid_order(LimitOrder{3, 3000, 14900, 150});
  order_book.add_ask_order(LimitOrder{4, 4000, 16100, 300});

  CHECK(order_book.pending_orders.size() == 4);

  // Cancel all orders one by one
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

  // Test with zero values
  order_book.add_bid_order(LimitOrder{0, 0, 0, 0});
  CHECK(order_book.pending_orders.size() == 1);
  CHECK(order_book.pending_orders.find(0) != order_book.pending_orders.end());

  // Test with maximum values
  order_book.add_ask_order(LimitOrder{UINT_FAST64_MAX, UINT_FAST64_MAX,
                                      UINT_FAST32_MAX, UINT_FAST32_MAX});
  CHECK(order_book.pending_orders.size() == 2);
  CHECK(order_book.pending_orders.find(UINT_FAST64_MAX) !=
        order_book.pending_orders.end());

  // Test cancelling zero ID
  CHECK(order_book.cancel_order(0) == true);
  CHECK(order_book.pending_orders.size() == 1);

  // Test cancelling maximum ID
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
        std::priority_queue<LimitOrder, std::vector<LimitOrder>,
                            LimitOrder::PriceTimeQueuePriority::BidCmp>>);
  CHECK(std::is_same_v<
        decltype(order_book.asks),
        std::priority_queue<LimitOrder, std::vector<LimitOrder>,
                            LimitOrder::PriceTimeQueuePriority::AskCmp>>);
}

TEST_SUITE_END();
