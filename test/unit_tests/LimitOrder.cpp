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
  CHECK(order1.balance == 0);
  CHECK(order1.quantity == 100);
  CHECK(order1.filled_quantity == 0);
  CHECK(order1.is_cancelled == false);
  CHECK(order2.id == order2_id);
  CHECK(order2.timestamp == order2_timestamp);
  CHECK(order2.price == order2_price);
  CHECK(order2.balance == 0);
  CHECK(order2.quantity == order2_quantity);
  CHECK(order2.filled_quantity == 0);
  CHECK(order2.is_cancelled == false);
}

/***/
TEST_CASE("limit_order_types") {
  LimitOrder order{1, 1000, 15000, 100};

  REQUIRE(order.id == 1);
  REQUIRE(order.timestamp == 1000);
  REQUIRE(order.price == 15000);
  REQUIRE(order.quantity == 100);
  REQUIRE(order.filled_quantity == 0);
  REQUIRE(order.is_cancelled == false);
  CHECK(std::is_same_v<decltype(order.id), LimitOrder::id_t>);
  CHECK(std::is_same_v<decltype(order.timestamp), timestamp_ns_t>);
  CHECK(std::is_same_v<decltype(order.price), price_t>);
  CHECK(std::is_same_v<decltype(order.quantity), quantity_t>);
  CHECK(std::is_same_v<decltype(order.filled_quantity), quantity_t>);
  CHECK(std::is_same_v<decltype(order.is_cancelled), bool>);
  CHECK(sizeof(order.id) >= sizeof(uint_fast64_t));
  CHECK(sizeof(order.timestamp) >= sizeof(uint_fast64_t));
  CHECK(sizeof(order.price) >= sizeof(uint_fast32_t));
  CHECK(sizeof(order.quantity) >= sizeof(uint_fast32_t));
  CHECK(sizeof(order.filled_quantity) >= sizeof(uint_fast32_t));
  CHECK(sizeof(order.is_cancelled) == sizeof(bool));
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

  LimitOrder order1{1, 1000, 15000, 100};
  LimitOrder order2{2, 1000, 16000, 100};
  LimitOrder order3{3, 1000, 15000, 100};
  LimitOrder order4{4, 2000, 15000, 100};

  CHECK(bid_cmp(&order1, &order2) == true);
  CHECK(bid_cmp(&order2, &order1) == false);

  CHECK(bid_cmp(&order3, &order4) == false);
  CHECK(bid_cmp(&order4, &order3) == true);
}

/***/
TEST_CASE("limit_order_priority_queue_ask_comparator") {
  LimitOrder::PriceTimeQueuePriority::AskCmp ask_cmp;

  LimitOrder order1{1, 1000, 16000, 100};
  LimitOrder order2{2, 1000, 15000, 100};
  LimitOrder order3{3, 1000, 15000, 100};
  LimitOrder order4{4, 2000, 15000, 100};

  CHECK(ask_cmp(&order1, &order2) == true);
  CHECK(ask_cmp(&order2, &order1) == false);

  CHECK(ask_cmp(&order3, &order4) == false);
  CHECK(ask_cmp(&order4, &order3) == true);
}

/***/
TEST_CASE("limit_order_edge_cases") {
  LimitOrder zero_order{0, 0, 0, 0};
  CHECK(zero_order.id == 0);
  CHECK(zero_order.timestamp == 0);
  CHECK(zero_order.price == 0);
  CHECK(zero_order.quantity == 0);
  CHECK(zero_order.filled_quantity == 0);
  CHECK(zero_order.is_cancelled == false);

  LimitOrder max_order{UINT_FAST64_MAX, UINT_FAST64_MAX, UINT_FAST32_MAX,
                       UINT_FAST32_MAX};
  CHECK(max_order.id == UINT_FAST64_MAX);
  CHECK(max_order.timestamp == UINT_FAST64_MAX);
  CHECK(max_order.price == UINT_FAST32_MAX);
  CHECK(max_order.quantity == UINT_FAST32_MAX);
  CHECK(max_order.filled_quantity == 0);
  CHECK(max_order.is_cancelled == false);
}

/***/
TEST_CASE("limit_order_copy_constructors") {
  LimitOrder order1{1, 1000, 15000, 100};
  order1.balance = 10000;
  LimitOrder order2{order1};

  CHECK(order2.id == order1.id);
  CHECK(order2.timestamp == order1.timestamp);
  CHECK(order2.price == order1.price);
  CHECK(order2.quantity == order1.quantity);
  CHECK(order2.filled_quantity == order1.filled_quantity);
  CHECK(order2.is_cancelled == order1.is_cancelled);
  CHECK(order2.balance == order1.balance);
}

/***/
TEST_CASE("limit_order_move_constructor") {
  LimitOrder order1{1, 1000, 15000, 100};
  LimitOrder order2{std::move(order1)};

  CHECK(order2.id == 1);
  CHECK(order2.timestamp == 1000);
  CHECK(order2.price == 15000);
  CHECK(order2.quantity == 100);
  CHECK(order2.filled_quantity == 0);
  CHECK(order2.is_cancelled == false);
}

TEST_SUITE_END();