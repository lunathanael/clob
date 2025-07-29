#include "doctest/doctest.h"

#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>

#include "misc/TestUtilities.h"

#include "clob/OrderBook.h"

TEST_SUITE_BEGIN("OrderBook");

using namespace clob;

TEST_CASE("order_book_construction") {
  OrderBook order_book;

  CHECK(order_book.bids_size() == 0);
  CHECK(order_book.asks_size() == 0);
  CHECK(order_book.get_best_bid_order() == nullptr);
  CHECK(order_book.get_best_ask_order() == nullptr);
}

TEST_CASE("add_bid_order_single") {
  OrderBook order_book;

  auto bid_order = std::make_unique<LimitOrder>(1, 1000, 15000, 100);
  order_book.add_bid_order(bid_order.get());

  CHECK(order_book.bids_size() == 1);
  CHECK(order_book.asks_size() == 0);

  const LimitOrder *top_bid = order_book.get_best_bid_order();
  CHECK(top_bid->id == 1);
  CHECK(top_bid->timestamp == 1000);
  CHECK(top_bid->price == 15000);
  CHECK(top_bid->quantity == 100);
  CHECK(top_bid->filled_quantity == 0);
}

TEST_CASE("add_ask_order_single") {
  OrderBook order_book;

  auto ask_order = std::make_unique<LimitOrder>(2, 2000, 16000, 200);
  order_book.add_ask_order(ask_order.get());

  CHECK(order_book.asks_size() == 1);
  CHECK(order_book.bids_size() == 0);

  const LimitOrder *top_ask = order_book.get_best_ask_order();
  CHECK(top_ask->id == 2);
  CHECK(top_ask->timestamp == 2000);
  CHECK(top_ask->price == 16000);
  CHECK(top_ask->quantity == 200);
  CHECK(top_ask->filled_quantity == 0);
}

TEST_CASE("add_multiple_bid_orders") {
  OrderBook order_book;

  auto bid1 = std::make_unique<LimitOrder>(1, 1000, 15000, 100);
  auto bid2 = std::make_unique<LimitOrder>(2, 1100, 14900, 150);
  auto bid3 = std::make_unique<LimitOrder>(3, 1200, 15100, 75);

  order_book.add_bid_order(bid1.get());
  CHECK(order_book.bids_size() == 1);

  order_book.add_bid_order(bid2.get());
  CHECK(order_book.bids_size() == 2);

  order_book.add_bid_order(bid3.get());
  CHECK(order_book.bids_size() == 3);
  CHECK(order_book.asks_size() == 0);
}

TEST_CASE("add_multiple_ask_orders") {
  OrderBook order_book;

  auto ask1 = std::make_unique<LimitOrder>(4, 1300, 16000, 200);
  auto ask2 = std::make_unique<LimitOrder>(5, 1400, 15900, 125);
  auto ask3 = std::make_unique<LimitOrder>(6, 1500, 16100, 300);

  order_book.add_ask_order(ask1.get());
  CHECK(order_book.asks_size() == 1);

  order_book.add_ask_order(ask2.get());
  CHECK(order_book.asks_size() == 2);

  order_book.add_ask_order(ask3.get());
  CHECK(order_book.asks_size() == 3);
  CHECK(order_book.bids_size() == 0);
}

TEST_CASE("bid_order_priority_price") {
  OrderBook order_book;

  auto bid1 = std::make_unique<LimitOrder>(1, 1000, 15000, 100);
  auto bid2 = std::make_unique<LimitOrder>(2, 1100, 15100, 150);
  auto bid3 = std::make_unique<LimitOrder>(3, 1200, 14900, 75);

  order_book.add_bid_order(bid1.get());
  order_book.add_bid_order(bid2.get());
  order_book.add_bid_order(bid3.get());

  const LimitOrder *best_bid = order_book.get_best_bid_order();
  CHECK(best_bid->id == 2);
  CHECK(best_bid->price == 15100);
}

TEST_CASE("bid_order_priority_time") {
  OrderBook order_book;

  auto bid1 = std::make_unique<LimitOrder>(1, 1000, 15000, 100);
  auto bid2 = std::make_unique<LimitOrder>(2, 1200, 15000, 75);

  order_book.add_bid_order(bid1.get());
  order_book.add_bid_order(bid2.get());

  const LimitOrder *best_bid = order_book.get_best_bid_order();
  CHECK(best_bid->id == 1);
  CHECK(best_bid->timestamp == 1000);
}

TEST_CASE("ask_order_priority_price") {
  OrderBook order_book;

  auto ask1 = std::make_unique<LimitOrder>(1, 1000, 16000, 100);
  auto ask2 = std::make_unique<LimitOrder>(2, 1100, 15900, 150);
  auto ask3 = std::make_unique<LimitOrder>(3, 1200, 16100, 75);

  order_book.add_ask_order(ask1.get());
  order_book.add_ask_order(ask2.get());
  order_book.add_ask_order(ask3.get());

  const LimitOrder *best_ask = order_book.get_best_ask_order();
  CHECK(best_ask->id == 2);
  CHECK(best_ask->price == 15900);
}

TEST_CASE("ask_order_priority_time") {
  OrderBook order_book;

  auto ask1 = std::make_unique<LimitOrder>(1, 1000, 16000, 100);
  auto ask2 = std::make_unique<LimitOrder>(2, 1200, 16000, 75);

  order_book.add_ask_order(ask1.get());
  order_book.add_ask_order(ask2.get());

  const LimitOrder *best_ask = order_book.get_best_ask_order();
  CHECK(best_ask->id == 1);
  CHECK(best_ask->timestamp == 1000);
}

TEST_CASE("order_matching_exact_price") {
  OrderBook order_book;

  auto ask_order = std::make_unique<LimitOrder>(1, 1000, 15000, 100);
  order_book.add_ask_order(ask_order.get());

  auto bid_order = std::make_unique<LimitOrder>(2, 1100, 15000, 50);
  order_book.add_bid_order(bid_order.get());

  CHECK(bid_order->filled_quantity == 50);
  CHECK(ask_order->filled_quantity == 50);
  CHECK(bid_order->balance == -50 * 15000);
  CHECK(ask_order->balance == 50 * 15000);
  CHECK(order_book.bids_size() == 0);
  CHECK(order_book.asks_size() == 1);
}

TEST_CASE("order_matching_partial_fill") {
  OrderBook order_book;

  auto ask_order = std::make_unique<LimitOrder>(1, 1000, 15000, 100);
  order_book.add_ask_order(ask_order.get());

  CHECK(ask_order->balance == 0);

  auto bid_order = std::make_unique<LimitOrder>(2, 1100, 15000, 150);
  order_book.add_bid_order(bid_order.get());

  CHECK(bid_order->filled_quantity == 100);
  CHECK(ask_order->filled_quantity == 100);
  CHECK(bid_order->balance == -100 * 15000);
  CHECK(ask_order->balance == 100 * 15000);
  CHECK(order_book.bids_size() == 1);
  CHECK(order_book.asks_size() == 0);
}

TEST_CASE("order_matching_no_match") {
  OrderBook order_book;

  auto ask_order = std::make_unique<LimitOrder>(1, 1000, 16000, 100);
  order_book.add_ask_order(ask_order.get());

  auto bid_order = std::make_unique<LimitOrder>(2, 1100, 15000, 100);
  order_book.add_bid_order(bid_order.get());

  CHECK(bid_order->filled_quantity == 0);
  CHECK(ask_order->filled_quantity == 0);
  CHECK(bid_order->balance == 0);
  CHECK(ask_order->balance == 0);
  CHECK(order_book.bids_size() == 1);
  CHECK(order_book.asks_size() == 1);
}

TEST_CASE("order_matching_multiple_orders") {
  OrderBook order_book;

  auto ask1 = std::make_unique<LimitOrder>(1, 1000, 15000, 50);
  auto ask2 = std::make_unique<LimitOrder>(2, 1100, 15000, 50);
  order_book.add_ask_order(ask1.get());
  order_book.add_ask_order(ask2.get());

  auto bid_order = std::make_unique<LimitOrder>(3, 1200, 15000, 75);
  order_book.add_bid_order(bid_order.get());

  CHECK(bid_order->filled_quantity == 75);
  CHECK(ask1->filled_quantity == 50);
  CHECK(ask2->filled_quantity == 25);
  CHECK(bid_order->balance == -75 * 15000);
  CHECK(ask1->balance == 50 * 15000);
  CHECK(ask2->balance == 25 * 15000);
  CHECK(order_book.bids_size() == 0);
  CHECK(order_book.asks_size() == 1);
}

TEST_CASE("order_matching_price_time_priority") {
  OrderBook order_book;

  auto ask1 = std::make_unique<LimitOrder>(1, 1000, 15000, 50);
  auto ask2 = std::make_unique<LimitOrder>(2, 1100, 14900, 50);
  order_book.add_ask_order(ask1.get());
  order_book.add_ask_order(ask2.get());

  auto bid_order = std::make_unique<LimitOrder>(3, 1200, 15000, 75);
  order_book.add_bid_order(bid_order.get());

  CHECK(bid_order->filled_quantity == 75);
  CHECK(ask2->filled_quantity == 50);
  CHECK(ask1->filled_quantity == 25);
  CHECK(bid_order->balance == -50 * 14900 - 25 * 15000);
  CHECK(ask2->balance == 50 * 14900);
  CHECK(ask1->balance == 25 * 15000);
  CHECK(order_book.bids_size() == 0);
  CHECK(order_book.asks_size() == 1);
}

TEST_CASE("cancelled_order_skipped") {
  OrderBook order_book;

  auto ask_order = std::make_unique<LimitOrder>(1, 1000, 15000, 100);
  ask_order->is_cancelled = true;
  order_book.add_ask_order(ask_order.get());

  auto bid_order = std::make_unique<LimitOrder>(2, 1100, 15000, 100);
  order_book.add_bid_order(bid_order.get());

  CHECK(bid_order->filled_quantity == 0);
  CHECK(bid_order->balance == 0);
  CHECK(ask_order->balance == 0);
  CHECK(order_book.bids_size() == 1);
  CHECK(order_book.asks_size() == 0);
}

TEST_CASE("add_cancelled_bid_order") {
  OrderBook order_book;

  auto bid_order = std::make_unique<LimitOrder>(1, 1000, 15000, 100);
  bid_order->is_cancelled = true;
  order_book.add_bid_order(bid_order.get());

  CHECK(bid_order->balance == 0);
  CHECK(order_book.bids_size() == 0);
  CHECK(order_book.asks_size() == 0);
}

TEST_CASE("add_cancelled_ask_order") {
  OrderBook order_book;

  auto ask_order = std::make_unique<LimitOrder>(1, 1000, 15000, 100);
  ask_order->is_cancelled = true;
  order_book.add_ask_order(ask_order.get());

  CHECK(ask_order->balance == 0);
  CHECK(order_book.bids_size() == 0);
  CHECK(order_book.asks_size() == 0);
}

TEST_CASE("cancel_later_ask_order") {
  OrderBook order_book;

  auto ask_order = std::make_unique<LimitOrder>(1, 1000, 15000, 100);
  order_book.add_ask_order(ask_order.get());
  ask_order->is_cancelled = true;

  auto bid_order = std::make_unique<LimitOrder>(2, 1100, 15000, 100);
  order_book.add_bid_order(bid_order.get());

  CHECK(bid_order->filled_quantity == 0);
  CHECK(bid_order->balance == 0);
  CHECK(ask_order->balance == 0);
  CHECK(order_book.bids_size() == 1);
  CHECK(order_book.asks_size() == 0);
}

TEST_CASE("cancel_later_bid_order") {
  OrderBook order_book;

  auto bid_order = std::make_unique<LimitOrder>(1, 1000, 15000, 100);
  order_book.add_bid_order(bid_order.get());
  bid_order->is_cancelled = true;

  auto ask_order = std::make_unique<LimitOrder>(2, 1100, 15000, 100);
  order_book.add_ask_order(ask_order.get());

  CHECK(ask_order->filled_quantity == 0);
  CHECK(bid_order->balance == 0);
  CHECK(ask_order->balance == 0);
  CHECK(order_book.bids_size() == 0);
  CHECK(order_book.asks_size() == 1);
}

TEST_CASE("bid_matches_better_ask_price") {
  OrderBook order_book;

  auto ask_order = std::make_unique<LimitOrder>(1, 1000, 14900, 100);
  order_book.add_ask_order(ask_order.get());

  auto bid_order = std::make_unique<LimitOrder>(2, 1100, 15000, 100);
  order_book.add_bid_order(bid_order.get());

  CHECK(bid_order->filled_quantity == 100);
  CHECK(ask_order->filled_quantity == 100);
  CHECK(bid_order->balance == -100 * 14900);
  CHECK(ask_order->balance == 100 * 14900);
  CHECK(order_book.bids_size() == 0);
  CHECK(order_book.asks_size() == 0);
}

TEST_CASE("ask_matches_better_bid_price") {
  OrderBook order_book;

  auto bid_order = std::make_unique<LimitOrder>(1, 1000, 15100, 100);
  order_book.add_bid_order(bid_order.get());

  auto ask_order = std::make_unique<LimitOrder>(2, 1100, 15000, 100);
  order_book.add_ask_order(ask_order.get());

  CHECK(ask_order->filled_quantity == 100);
  CHECK(bid_order->filled_quantity == 100);
  CHECK(bid_order->balance == -100 * 15100);
  CHECK(ask_order->balance == 100 * 15100);
  CHECK(order_book.asks_size() == 0);
  CHECK(order_book.bids_size() == 0);
}

TEST_CASE("exact_match_single_order") {
  OrderBook order_book;

  auto bid1 = std::make_unique<LimitOrder>(101, 5000, 50000, 800);
  order_book.add_bid_order(bid1.get());
  CHECK(order_book.bids_size() == 1);
  CHECK(order_book.asks_size() == 0);

  auto ask1 = std::make_unique<LimitOrder>(201, 6000, 50000, 800);
  order_book.add_ask_order(ask1.get());
  CHECK(order_book.bids_size() == 0);
  CHECK(order_book.asks_size() == 0);
  CHECK(bid1->filled_quantity == 800);
  CHECK(ask1->filled_quantity == 800);
  CHECK(bid1->balance == -800 * 50000);
  CHECK(ask1->balance == 800 * 50000);
}

TEST_CASE("one_ask_matches_multiple_bids") {
  OrderBook order_book;

  auto bid1 = std::make_unique<LimitOrder>(101, 3000, 50000, 400);
  auto bid2 = std::make_unique<LimitOrder>(102, 7000, 49800, 600);
  auto bid3 = std::make_unique<LimitOrder>(103, 1500, 50200, 300);
  order_book.add_bid_order(bid1.get());
  order_book.add_bid_order(bid2.get());
  order_book.add_bid_order(bid3.get());
  CHECK(order_book.bids_size() == 3);
  CHECK(order_book.get_best_bid_order()->id == 103);

  auto ask1 = std::make_unique<LimitOrder>(201, 8000, 49500, 1000);
  order_book.add_ask_order(ask1.get());
  CHECK(order_book.bids_size() == 1);
  CHECK(order_book.asks_size() == 0);
  CHECK(ask1->filled_quantity == 1000);
  CHECK(bid3->filled_quantity == 300);
  CHECK(bid1->filled_quantity == 400);
  CHECK(bid2->filled_quantity == 300);
  CHECK(order_book.get_best_bid_order()->id == 102);
  CHECK(bid1->balance == -400 * 50000);
  CHECK(bid2->balance == -300 * 49800);
  CHECK(bid3->balance == -300 * 50200);
  CHECK(ask1->balance == 400 * 50000 + 300 * 49800 + 300 * 50200);
}

TEST_CASE("one_bid_matches_multiple_asks") {
  OrderBook order_book;

  auto ask1 = std::make_unique<LimitOrder>(201, 4000, 50500, 250);
  auto ask2 = std::make_unique<LimitOrder>(202, 9000, 50200, 400);
  auto ask3 = std::make_unique<LimitOrder>(203, 2000, 50800, 350);
  order_book.add_ask_order(ask1.get());
  order_book.add_ask_order(ask2.get());
  order_book.add_ask_order(ask3.get());
  CHECK(order_book.asks_size() == 3);
  CHECK(order_book.get_best_ask_order()->id == 202);

  auto bid1 = std::make_unique<LimitOrder>(101, 10000, 51000, 900);
  order_book.add_bid_order(bid1.get());
  CHECK(order_book.bids_size() == 0);
  CHECK(order_book.asks_size() == 1);
  CHECK(bid1->filled_quantity == 900);
  CHECK(ask2->filled_quantity == 400);
  CHECK(ask1->filled_quantity == 250);
  CHECK(ask3->filled_quantity == 250);
  CHECK(order_book.get_best_ask_order()->id == 203);
  CHECK(ask1->balance == 250 * 50500);
  CHECK(ask2->balance == 400 * 50200);
  CHECK(ask3->balance == 250 * 50800);
  CHECK(bid1->balance == -250 * 50500 - 400 * 50200 - 250 * 50800);
}

TEST_CASE("partial_fill_scenarios") {
  OrderBook order_book;

  auto bid1 = std::make_unique<LimitOrder>(101, 5000, 50000, 1200);
  order_book.add_bid_order(bid1.get());

  auto ask1 = std::make_unique<LimitOrder>(201, 6000, 50000, 800);
  order_book.add_ask_order(ask1.get());
  CHECK(order_book.bids_size() == 1);
  CHECK(order_book.asks_size() == 0);
  CHECK(bid1->filled_quantity == 800);
  CHECK(ask1->filled_quantity == 800);
  CHECK(order_book.get_best_bid_order()->id == 101);

  auto ask2 = std::make_unique<LimitOrder>(202, 7000, 49900, 600);
  order_book.add_ask_order(ask2.get());
  CHECK(order_book.bids_size() == 0);
  CHECK(order_book.asks_size() == 1);
  CHECK(bid1->filled_quantity == 1200);
  CHECK(ask2->filled_quantity == 400);
  CHECK(order_book.get_best_ask_order()->id == 202);
  CHECK(bid1->balance == -1200 * 50000);
  CHECK(ask1->balance == 800 * 50000);
  CHECK(ask2->balance == 400 * 50000);
}

TEST_CASE("no_match_orders_go_to_book") {
  OrderBook order_book;

  auto bid1 = std::make_unique<LimitOrder>(101, 3000, 49500, 500);
  auto bid2 = std::make_unique<LimitOrder>(102, 8000, 49000, 300);
  order_book.add_bid_order(bid1.get());
  order_book.add_bid_order(bid2.get());
  CHECK(order_book.bids_size() == 2);

  auto ask1 = std::make_unique<LimitOrder>(201, 5000, 50000, 400);
  auto ask2 = std::make_unique<LimitOrder>(202, 12000, 50500, 600);
  order_book.add_ask_order(ask1.get());
  order_book.add_ask_order(ask2.get());
  CHECK(order_book.bids_size() == 2);
  CHECK(order_book.asks_size() == 2);
  CHECK(bid1->filled_quantity == 0);
  CHECK(bid2->filled_quantity == 0);
  CHECK(ask1->filled_quantity == 0);
  CHECK(ask2->filled_quantity == 0);
  CHECK(order_book.get_best_bid_order()->id == 101);
  CHECK(order_book.get_best_ask_order()->id == 201);
  CHECK(bid1->balance == 0);
  CHECK(bid2->balance == 0);
  CHECK(ask1->balance == 0);
  CHECK(ask2->balance == 0);
}

TEST_CASE("price_time_priority_validation") {
  OrderBook order_book;

  auto bid1 = std::make_unique<LimitOrder>(101, 2000, 50000, 300);
  auto bid2 = std::make_unique<LimitOrder>(102, 8000, 50000, 400);
  auto bid3 = std::make_unique<LimitOrder>(103, 5000, 50100, 200);
  order_book.add_bid_order(bid1.get());
  order_book.add_bid_order(bid2.get());
  order_book.add_bid_order(bid3.get());
  REQUIRE(order_book.bids_size() == 3);
  CHECK(order_book.get_best_bid_order()->id == 103);

  auto ask1 = std::make_unique<LimitOrder>(201, 3000, 50000, 500);
  order_book.add_ask_order(ask1.get());
  CHECK(order_book.bids_size() == 1);
  CHECK(order_book.asks_size() == 0);
  CHECK(ask1->filled_quantity == 500);
  CHECK(bid1->filled_quantity == 300);
  CHECK(bid3->filled_quantity == 200);
  CHECK(order_book.get_best_bid_order()->id == 102);

  auto ask2 = std::make_unique<LimitOrder>(202, 6000, 50000, 400);
  order_book.add_ask_order(ask2.get());
  CHECK(order_book.bids_size() == 0);
  CHECK(order_book.asks_size() == 0);
  CHECK(ask2->filled_quantity == 400);
  CHECK(bid1->filled_quantity == 300);
  CHECK(ask1->filled_quantity == 500);
  CHECK(bid1->balance == -300 * 50000);
  CHECK(bid2->balance == -400 * 50000);
  CHECK(bid3->balance == -200 * 50100);
  CHECK(ask1->balance == 200 * 50100 + 300 * 50000);
  CHECK(ask2->balance == 400 * 50000);
}

TEST_CASE("aggressive_cross_price_matching") {
  OrderBook order_book;

  auto bid1 = std::make_unique<LimitOrder>(101, 4000, 50000, 400);
  auto bid2 = std::make_unique<LimitOrder>(102, 7000, 49800, 600);
  auto bid3 = std::make_unique<LimitOrder>(103, 2000, 50200, 300);
  order_book.add_bid_order(bid1.get());
  order_book.add_bid_order(bid2.get());
  order_book.add_bid_order(bid3.get());

  auto ask1 = std::make_unique<LimitOrder>(201, 9000, 49500, 1500);
  order_book.add_ask_order(ask1.get());
  CHECK(order_book.bids_size() == 0);
  CHECK(order_book.asks_size() == 1);
  CHECK(ask1->filled_quantity == 1300);
  CHECK(bid3->filled_quantity == 300);
  CHECK(bid1->filled_quantity == 400);
  CHECK(bid2->filled_quantity == 600);
  CHECK(order_book.get_best_ask_order()->id == 201);
  CHECK(bid1->balance == -400 * 50000);
  CHECK(bid2->balance == -600 * 49800);
  CHECK(bid3->balance == -300 * 50200);
  CHECK(ask1->balance == 400 * 50000 + 600 * 49800 + 300 * 50200);
}

TEST_CASE("complex_interlaced_timestamps") {
  OrderBook order_book;

  auto bid1 = std::make_unique<LimitOrder>(101, 15000, 50000, 500);
  auto bid2 = std::make_unique<LimitOrder>(102, 3000, 50000, 300);
  auto bid3 = std::make_unique<LimitOrder>(103, 22000, 49900, 400);
  auto bid4 = std::make_unique<LimitOrder>(104, 8000, 50000, 200);
  order_book.add_bid_order(bid1.get());
  order_book.add_bid_order(bid2.get());
  order_book.add_bid_order(bid3.get());
  order_book.add_bid_order(bid4.get());
  CHECK(order_book.get_best_bid_order()->id == 102);

  auto ask1 = std::make_unique<LimitOrder>(201, 12000, 50000, 1000);
  order_book.add_ask_order(ask1.get());
  CHECK(order_book.bids_size() == 1);
  CHECK(order_book.asks_size() == 0);
  CHECK(ask1->filled_quantity == 1000);
  CHECK(bid2->filled_quantity == 300);
  CHECK(bid4->filled_quantity == 200);
  CHECK(bid3->filled_quantity == 0);
  CHECK(bid1->filled_quantity == 500);
  CHECK(order_book.get_best_bid_order()->id == 103);
  CHECK(bid1->balance == -500 * 50000);
  CHECK(bid2->balance == -300 * 50000);
  CHECK(bid3->balance == 0);
  CHECK(bid4->balance == -200 * 50000);
  CHECK(ask1->balance == 1000 * 50000);
}

TEST_CASE("ultimate_comprehensive_order_book_test") {
  OrderBook order_book;

  auto bid1 = std::make_unique<LimitOrder>(1001, 125000, 985000, 15000);
  auto bid2 = std::make_unique<LimitOrder>(1002, 87500, 982000, 8500);
  auto bid3 = std::make_unique<LimitOrder>(1003, 200000, 990000, 22000);
  auto bid4 = std::make_unique<LimitOrder>(1004, 45000, 988000, 12000);
  auto bid5 = std::make_unique<LimitOrder>(1005, 175000, 985000, 7500);

  order_book.add_bid_order(bid1.get());
  CHECK(order_book.bids_size() == 1);
  CHECK(order_book.asks_size() == 0);
  REQUIRE(order_book.get_best_bid_order() != nullptr);
  CHECK(order_book.get_best_bid_order()->id == 1001);
  CHECK(bid1->filled_quantity == 0);
  CHECK(bid1->balance == 0);

  order_book.add_bid_order(bid2.get());
  CHECK(order_book.bids_size() == 2);
  REQUIRE(order_book.get_best_bid_order() != nullptr);
  CHECK(order_book.get_best_bid_order()->id == 1001);
  CHECK(bid2->filled_quantity == 0);
  CHECK(bid2->balance == 0);

  order_book.add_bid_order(bid3.get());
  CHECK(order_book.bids_size() == 3);
  REQUIRE(order_book.get_best_bid_order() != nullptr);
  CHECK(order_book.get_best_bid_order()->id == 1003);
  CHECK(bid3->filled_quantity == 0);
  CHECK(bid3->balance == 0);

  order_book.add_bid_order(bid4.get());
  CHECK(order_book.bids_size() == 4);
  REQUIRE(order_book.get_best_bid_order() != nullptr);
  CHECK(order_book.get_best_bid_order()->id == 1003);
  CHECK(bid4->filled_quantity == 0);
  CHECK(bid4->balance == 0);

  order_book.add_bid_order(bid5.get());
  CHECK(order_book.bids_size() == 5);
  REQUIRE(order_book.get_best_bid_order() != nullptr);
  CHECK(order_book.get_best_bid_order()->id == 1003);
  CHECK(bid5->filled_quantity == 0);

  auto ask1 = std::make_unique<LimitOrder>(2001, 150000, 1005000, 18000);
  auto ask2 = std::make_unique<LimitOrder>(2002, 95000, 992000, 9500);
  auto ask3 = std::make_unique<LimitOrder>(2003, 220000, 998000, 25000);

  order_book.add_ask_order(ask1.get());
  CHECK(order_book.bids_size() == 5);
  CHECK(order_book.asks_size() == 1);
  REQUIRE(order_book.get_best_ask_order() != nullptr);
  CHECK(order_book.get_best_ask_order()->id == 2001);
  CHECK(ask1->filled_quantity == 0);

  order_book.add_ask_order(ask2.get());
  CHECK(order_book.bids_size() == 5);
  CHECK(order_book.asks_size() == 2);
  REQUIRE(order_book.get_best_ask_order() != nullptr);
  CHECK(order_book.get_best_ask_order()->id == 2002);
  CHECK(ask2->filled_quantity == 0);

  order_book.add_ask_order(ask3.get());
  CHECK(order_book.bids_size() == 5);
  CHECK(order_book.asks_size() == 3);
  REQUIRE(order_book.get_best_ask_order() != nullptr);
  CHECK(order_book.get_best_ask_order()->id == 2002);
  CHECK(ask3->filled_quantity == 0);

  auto bid6 = std::make_unique<LimitOrder>(1006, 275000, 992000, 9500);
  order_book.add_bid_order(bid6.get());
  CHECK(order_book.bids_size() == 5);
  CHECK(order_book.asks_size() == 2);
  REQUIRE(order_book.get_best_bid_order() != nullptr);
  CHECK(order_book.get_best_bid_order()->id == 1003);
  REQUIRE(order_book.get_best_ask_order() != nullptr);
  CHECK(order_book.get_best_ask_order()->id == 2003);
  CHECK(bid6->filled_quantity == 9500);
  CHECK(ask2->filled_quantity == 9500);

  auto ask4 = std::make_unique<LimitOrder>(2004, 300000, 980000, 55000);
  order_book.add_ask_order(ask4.get());
  CHECK(order_book.bids_size() == 2);
  CHECK(order_book.asks_size() == 2);
  REQUIRE(order_book.get_best_bid_order() != nullptr);
  CHECK(order_book.get_best_bid_order()->id == 1005);
  REQUIRE(order_book.get_best_ask_order() != nullptr);
  CHECK(order_book.get_best_ask_order()->id == 2003);
  CHECK(ask4->filled_quantity == 55000);
  CHECK(bid3->filled_quantity == 22000);
  CHECK(bid4->filled_quantity == 12000);
  CHECK(bid1->filled_quantity == 15000);
  CHECK(bid5->filled_quantity == 6000);

  auto bid7 = std::make_unique<LimitOrder>(1007, 350000, 1010000, 35000);
  order_book.add_bid_order(bid7.get());
  CHECK(order_book.bids_size() == 2);
  CHECK(order_book.asks_size() == 1);
  REQUIRE(order_book.get_best_bid_order() != nullptr);
  CHECK(order_book.get_best_bid_order()->id == 1005);
  REQUIRE(order_book.get_best_ask_order() != nullptr);
  CHECK(order_book.get_best_ask_order()->id == 2001);
  CHECK(bid7->filled_quantity == 35000);
  CHECK(ask3->filled_quantity == 25000);
  CHECK(ask1->filled_quantity == 10000);
  CHECK(ask4->filled_quantity == 55000);

  auto ask5 = std::make_unique<LimitOrder>(2005, 400000, 1015000, 12000);
  auto ask6 = std::make_unique<LimitOrder>(2006, 325000, 1008000, 8000);
  auto ask7 = std::make_unique<LimitOrder>(2007, 450000, 1012000, 15000);

  order_book.add_ask_order(ask5.get());
  CHECK(order_book.bids_size() == 2);
  CHECK(order_book.asks_size() == 2);
  REQUIRE(order_book.get_best_ask_order() != nullptr);
  CHECK(order_book.get_best_ask_order()->id == 2001);
  CHECK(ask5->filled_quantity == 0);

  order_book.add_ask_order(ask6.get());
  CHECK(order_book.bids_size() == 2);
  CHECK(order_book.asks_size() == 3);
  REQUIRE(order_book.get_best_ask_order() != nullptr);
  CHECK(order_book.get_best_ask_order()->id == 2001);
  CHECK(ask6->filled_quantity == 0);

  order_book.add_ask_order(ask7.get());
  CHECK(order_book.bids_size() == 2);
  CHECK(order_book.asks_size() == 4);
  REQUIRE(order_book.get_best_ask_order() != nullptr);
  CHECK(order_book.get_best_ask_order()->id == 2001);
  CHECK(ask7->filled_quantity == 0);

  auto bid8 = std::make_unique<LimitOrder>(1008, 500000, 1020000, 45000);
  order_book.add_bid_order(bid8.get());
  CHECK(order_book.bids_size() == 3);
  CHECK(order_book.asks_size() == 0);
  REQUIRE(order_book.get_best_bid_order() != nullptr);
  CHECK(order_book.get_best_bid_order()->id == 1008);
  CHECK(bid8->filled_quantity == 43000);
  CHECK(ask6->filled_quantity == 8000);
  CHECK(ask7->filled_quantity == 15000);
  CHECK(ask5->filled_quantity == 12000);

  auto ask8 = std::make_unique<LimitOrder>(2008, 600000, 975000, 75000);
  order_book.add_ask_order(ask8.get());
  CHECK(order_book.bids_size() == 0);
  CHECK(order_book.asks_size() == 1);
  CHECK(order_book.get_best_bid_order() == nullptr);
  REQUIRE(order_book.get_best_ask_order() != nullptr);
  CHECK(order_book.get_best_ask_order()->id == 2008);
  CHECK(ask8->filled_quantity == 12000);
  CHECK(bid7->filled_quantity == 35000);
  CHECK(bid8->filled_quantity == 45000);
  CHECK(bid2->filled_quantity == 8500);

  auto bid9 = std::make_unique<LimitOrder>(1009, 700000, 1025000, 90000);
  auto bid10 = std::make_unique<LimitOrder>(1010, 550000, 1018000, 25000);
  auto bid11 = std::make_unique<LimitOrder>(1011, 750000, 1030000, 35000);

  order_book.add_bid_order(bid9.get());
  CHECK(order_book.bids_size() == 1);
  CHECK(order_book.asks_size() == 0);
  REQUIRE(order_book.get_best_bid_order() != nullptr);
  CHECK(order_book.get_best_bid_order()->id == 1009);
  CHECK(bid9->filled_quantity == 63000);
  CHECK(ask8->filled_quantity == 75000);

  order_book.add_bid_order(bid10.get());
  CHECK(order_book.bids_size() == 2);
  CHECK(order_book.asks_size() == 0);
  REQUIRE(order_book.get_best_bid_order() != nullptr);
  CHECK(order_book.get_best_bid_order()->id == 1009);
  CHECK(order_book.get_best_ask_order() == nullptr);
  CHECK(bid10->filled_quantity == 0);

  order_book.add_bid_order(bid11.get());
  CHECK(order_book.bids_size() == 3);
  CHECK(order_book.asks_size() == 0);
  REQUIRE(order_book.get_best_bid_order() != nullptr);
  CHECK(order_book.get_best_bid_order()->id == 1011);
  CHECK(bid11->filled_quantity == 0);

  auto ask9 = std::make_unique<LimitOrder>(2009, 800000, 1000000, 125000);
  order_book.add_ask_order(ask9.get());
  CHECK(order_book.bids_size() == 0);
  CHECK(order_book.asks_size() == 1);
  REQUIRE(order_book.get_best_ask_order() != nullptr);
  CHECK(order_book.get_best_ask_order()->id == 2009);
  CHECK(ask9->filled_quantity == 87000);
  CHECK(bid11->filled_quantity == 35000);
  CHECK(bid9->filled_quantity == 90000);
  CHECK(bid10->filled_quantity == 25000);
  CHECK(bid10->balance == -25000LL * 1018000);
  CHECK(bid11->balance == -35000LL * 1030000);
  CHECK(ask9->balance ==
        35000LL * 1030000 + 27000LL * 1025000 + 25000LL * 1018000);
}

TEST_SUITE_END();
