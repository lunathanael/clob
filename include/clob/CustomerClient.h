/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#pragma once

#include <memory>
#include <string>

#include "market.grpc.pb.h"
#include "market.pb.h"

#include "clob/LimitOrder.h"
#include "clob/types.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using marketmaker::CancelOrderRequest;
using marketmaker::CancelOrderResponse;
using marketmaker::GetOrderRequest;
using marketmaker::LimitOrderId;
using marketmaker::MarketMaker;
using marketmaker::PlaceOrderRequest;

namespace clob {

/**
 * @brief Helper function to create a LimitOrderId protobuf message
 *
 * @param order_id The order ID
 * @return marketmaker::LimitOrderId protobuf message
 */
marketmaker::LimitOrderId MakeLimitOrderId(const clob::LimitOrderId_t order_id);

/**
 * @brief Helper function to create a GetOrderRequest protobuf message
 *
 * @param order_id The order ID to query
 * @param order_type The type of the order (Bid/Ask)
 * @return marketmaker::GetOrderRequest protobuf message
 */
marketmaker::GetOrderRequest
MakeGetOrderRequest(const clob::LimitOrderId_t order_id,
                    const clob::OrderType order_type);

/**
 * @brief Helper function to create a CancelOrderRequest protobuf message
 *
 * @param order_id The order ID to cancel
 * @param order_type The type of the order (Bid/Ask)
 * @return marketmaker::CancelOrderRequest protobuf message
 */
marketmaker::CancelOrderRequest
MakeCancelOrderRequest(const clob::LimitOrderId_t order_id,
                       const clob::OrderType order_type);

/**
 * @brief Helper function to create a PlaceOrderRequest protobuf message
 *
 * @param order_type The type of order (Bid/Ask)
 * @param stock_id The stock identifier
 * @param price The order price
 * @param quantity The order quantity
 * @return marketmaker::PlaceOrderRequest protobuf message
 */
marketmaker::PlaceOrderRequest
MakePlaceOrderRequest(const clob::OrderType order_type,
                      const clob::StockId_t stock_id, const clob::price_t price,
                      const clob::quantity_t quantity);

/**
 * @brief gRPC client for communicating with the market maker server
 *
 * @details This class provides a client interface for placing orders,
 * querying order status, and canceling orders through gRPC.
 */
class CustomerClient {
public:
  /**
   * @brief Constructor for CustomerClient
   *
   * @param channel gRPC channel for communication with the server
   */
  explicit CustomerClient(std::shared_ptr<grpc::Channel> channel);

  /**
   * @brief Destructor
   */
  ~CustomerClient();

  /**
   * @brief Get the status of an existing order
   *
   * @param order_id The ID of the order to query
   * @param order_type The type of the order (Bid/Ask)
   */
  void GetOrderStatus(const clob::LimitOrderId_t order_id,
                      const clob::OrderType order_type);

  /**
   * @brief Cancel an existing order
   *
   * @param order_id The ID of the order to cancel
   * @param order_type The type of the order (Bid/Ask)
   */
  void CancelOrder(const clob::LimitOrderId_t order_id,
                   const clob::OrderType order_type);

  /**
   * @brief Place a new order in the market
   *
   * @param order_type The type of order (Bid/Ask)
   * @param stock_id The stock identifier
   * @param price The order price
   * @param quantity The order quantity
   * @return The ID of the placed order
   */
  clob::LimitOrderId_t PlaceOrder(const clob::OrderType order_type,
                                  const clob::StockId_t stock_id,
                                  const clob::price_t price,
                                  const clob::quantity_t quantity);

  /**
   * @brief Quote the best bid and ask prices
   *
   * @param stock_id The stock identifier
   * @return The best bid and ask prices
   */
  std::pair<clob::price_t, clob::price_t> QuoteBestBidAsk(const clob::StockId_t stock_id);

  /**
   * @brief Parse and execute a command string
   *
   * @param command The command string to parse and execute
   *
   * @details Supported commands:
   * - "place_order <order_type> <stock_id> <price> <quantity>"
   * - "get_order_status <order_type> <order_id>"
   * - "cancel_order <order_type> <order_id>"
   */
  void ParseCommand(const std::string &command);

private:
  std::unique_ptr<marketmaker::MarketMaker::Stub> stub_;
};

} // namespace clob
