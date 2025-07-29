#include "doctest/doctest.h"

#include <cstdint>
#include <type_traits>
#include <utility>

#include "misc/TestUtilities.h"

#include "clob/LimitOrder.h"

TEST_SUITE_BEGIN("LimitOrder");

using namespace clob;

/***/
TEST_CASE("limit_order_construction") {
  LimitOrder order1{1, 1000, 15000, 100};
  LimitOrder::id_t order2_id = 2;
  timestamp_ns_t order2_timestamp = 2000;
  price_t order2_price = 16000;
  quantity_t order2_quantity = 200;
  LimitOrder order2{order2_id, order2_timestamp, order2_price, order2_quantity};

  CHECK(order1.id == 1);
  CHECK(order1.timestamp == 1000);
  CHECK(order1.price == 15000);
  CHECK(order1.quantity == 100);
  CHECK(order2.id == order2_id);
  CHECK(order2.timestamp == order2_timestamp);
  CHECK(order2.price == order2_price);
  CHECK(order2.quantity == order2_quantity);
}

/***/
TEST_CASE("limit_order_types") {
  LimitOrder order{1, 1000, 15000, 100};

  REQUIRE(order.id == 1);
  REQUIRE(order.timestamp == 1000);
  REQUIRE(order.price == 15000);
  REQUIRE(order.quantity == 100);

  CHECK(std::is_same_v<decltype(order.id), LimitOrder::id_t>);
  CHECK(std::is_same_v<decltype(order.timestamp), timestamp_ns_t>);
  CHECK(std::is_same_v<decltype(order.price), price_t>);
  CHECK(std::is_same_v<decltype(order.quantity), quantity_t>);
  CHECK(sizeof(order.id) >= sizeof(uint_fast64_t));
  CHECK(sizeof(order.timestamp) >= sizeof(uint_fast64_t));
  CHECK(sizeof(order.price) >= sizeof(uint_fast32_t));
  CHECK(sizeof(order.quantity) >= sizeof(uint_fast32_t));
}

/***/
TEST_CASE("limit_order_order_type_enum") {
  CHECK(static_cast<int>(LimitOrder::OrderType::Bid) == 0);
  CHECK(static_cast<int>(LimitOrder::OrderType::Ask) == 1);
  CHECK(LimitOrder::OrderType::Bid != LimitOrder::OrderType::Ask);
}

/***/
TEST_CASE("limit_order_priority_queue_bid_comparator") {
  LimitOrder::PriceTimeQueuePriority::BidCmp bid_cmp;

  // Test price priority (lower price = higher priority for bids)
  LimitOrder order1{1, 1000, 15000, 100}; // Lower price
  LimitOrder order2{2, 1000, 16000, 100}; // Higher price

  CHECK(bid_cmp(order1, order2) == true);  // order2 should have higher priority
  CHECK(bid_cmp(order2, order1) == false); // order1 should have lower priority

  // Test time priority (earlier timestamp = higher priority when prices equal)
  LimitOrder order3{3, 1000, 15000, 100}; // Earlier timestamp
  LimitOrder order4{4, 2000, 15000, 100}; // Later timestamp

  CHECK(bid_cmp(order3, order4) == false); // order3 should have higher priority
  CHECK(bid_cmp(order4, order3) == true);  // order4 should have lower priority
}

/***/
TEST_CASE("limit_order_priority_queue_ask_comparator") {
  LimitOrder::PriceTimeQueuePriority::AskCmp ask_cmp;

  // Test price priority (higher price = higher priority for asks)
  LimitOrder order1{1, 1000, 16000, 100}; // Higher price
  LimitOrder order2{2, 1000, 15000, 100}; // Lower price

  CHECK(ask_cmp(order1, order2) == true);  // order1 should have higher priority
  CHECK(ask_cmp(order2, order1) == false); // order2 should have lower priority

  // Test time priority (earlier timestamp = higher priority when prices equal)
  LimitOrder order3{3, 1000, 15000, 100}; // Earlier timestamp
  LimitOrder order4{4, 2000, 15000, 100}; // Later timestamp

  CHECK(ask_cmp(order3, order4) == false); // order3 should have higher priority
  CHECK(ask_cmp(order4, order3) == true);  // order4 should have lower priority
}

/***/
TEST_CASE("limit_order_edge_cases") {
  // Test with zero values
  LimitOrder zero_order{0, 0, 0, 0};
  CHECK(zero_order.id == 0);
  CHECK(zero_order.timestamp == 0);
  CHECK(zero_order.price == 0);
  CHECK(zero_order.quantity == 0);

  // Test with maximum values
  LimitOrder max_order{UINT_FAST64_MAX, UINT_FAST64_MAX, UINT_FAST32_MAX,
                       UINT_FAST32_MAX};
  CHECK(max_order.id == UINT_FAST64_MAX);
  CHECK(max_order.timestamp == UINT_FAST64_MAX);
  CHECK(max_order.price == UINT_FAST32_MAX);
  CHECK(max_order.quantity == UINT_FAST32_MAX);
}

/***/
TEST_CASE("limit_order_copy_constructors") {
  LimitOrder order1{1, 1000, 15000, 100};
  LimitOrder order2{order1};

  CHECK(order2.id == order1.id);
  CHECK(order2.timestamp == order1.timestamp);
  CHECK(order2.price == order1.price);
  CHECK(order2.quantity == order1.quantity);
}

/***/
TEST_CASE("limit_order_move_constructor") {
  LimitOrder order1{1, 1000, 15000, 100};
  LimitOrder order2{std::move(order1)};

  CHECK(order2.id == 1);
  CHECK(order2.timestamp == 1000);
  CHECK(order2.price == 15000);
  CHECK(order2.quantity == 100);
}

TEST_SUITE_END();