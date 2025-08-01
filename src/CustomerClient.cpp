/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>

#include "absl/flags/parse.h"
#include "absl/log/initialize.h"
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

LimitOrderId MakeLimitOrderId(const clob::LimitOrderId_t order_id) {
  LimitOrderId limit_order_id;
  limit_order_id.set_order_id(order_id);
  return limit_order_id;
}

GetOrderRequest MakeGetOrderRequest(const clob::LimitOrderId_t order_id,
                                    const clob::OrderType order_type) {
  GetOrderRequest get_order_request;
  get_order_request.set_order_id(order_id);
  get_order_request.set_order_type(
      static_cast<marketmaker::OrderType>(order_type));
  return get_order_request;
}

CancelOrderRequest MakeCancelOrderRequest(const clob::LimitOrderId_t order_id,
                                          const clob::OrderType order_type) {
  CancelOrderRequest cancel_order_request;
  cancel_order_request.set_order_id(order_id);
  cancel_order_request.set_order_type(
      static_cast<marketmaker::OrderType>(order_type));
  return cancel_order_request;
}

PlaceOrderRequest MakePlaceOrderRequest(const clob::OrderType order_type,
                                        const clob::StockId_t stock_id,
                                        const clob::price_t price,
                                        const clob::quantity_t quantity) {
  PlaceOrderRequest place_order_request;
  place_order_request.set_order_type(
      static_cast<marketmaker::OrderType>(order_type));
  place_order_request.set_stock_id(stock_id);
  place_order_request.set_price(price);
  place_order_request.set_quantity(quantity);
  place_order_request.set_order_type(
      static_cast<marketmaker::OrderType>(order_type));
  return place_order_request;
}

class CustomerClient {
  std::unique_ptr<MarketMaker::Stub> stub_;

public:
  CustomerClient(std::shared_ptr<Channel> channel)
      : stub_(MarketMaker::NewStub(channel)) {}

  void GetOrderStatus(const clob::LimitOrderId_t order_id,
                      const clob::OrderType order_type) {
    GetOrderRequest get_order_request =
        MakeGetOrderRequest(order_id, order_type);
    ClientContext context;
    marketmaker::LimitOrderInfo response;
    Status status =
        stub_->GetOrderStatus(&context, get_order_request, &response);

    std::cout << "Order id: " << response.order_id() << std::endl;
    std::cout << "Order timestamp: " << response.timestamp() << std::endl;
    std::cout << "Order balance: " << response.balance() << std::endl;
    std::cout << "Order price: " << response.price() << std::endl;
    std::cout << "Order quantity: " << response.quantity() << std::endl;
    std::cout << "Order filled quantity: " << response.filled_quantity() << std::endl;
    std::cout << "Is cancelled: " << response.is_cancelled() << std::endl;
    std::cout << "Order found: " << response.order_found() << std::endl;
  }

  void CancelOrder(const clob::LimitOrderId_t order_id,
                   const clob::OrderType order_type) {
    CancelOrderRequest cancel_order_request =
        MakeCancelOrderRequest(order_id, order_type);
    ClientContext context;
    marketmaker::CancelOrderResponse response;
    Status status =
        stub_->CancelOrder(&context, cancel_order_request, &response);
    std::cout << "Order cancelled: " << response.success() << std::endl;
  }

  clob::LimitOrderId_t PlaceOrder(const clob::OrderType order_type,
                  const clob::StockId_t stock_id, const clob::price_t price,
                  const clob::quantity_t quantity) {
    PlaceOrderRequest place_order_request =
        MakePlaceOrderRequest(order_type, stock_id, price, quantity);
    ClientContext context;
    marketmaker::LimitOrderId response;
    Status status = stub_->PlaceOrder(&context, place_order_request, &response);
    std::cout << "Order placed: " << response.order_id() << std::endl;
    return response.order_id();
  }
};

int main(int argc, char **argv) {
  absl::ParseCommandLine(argc, argv);
  absl::InitializeLog();
  // Expect only arg: --db_path=path/to/route_guide_db.json.
  CustomerClient customer(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));
  clob::LimitOrderId_t order_id =
      customer.PlaceOrder(clob::OrderType::Ask, 0, 100, 800);
  customer.GetOrderStatus(9223372036854775808, clob::OrderType::Ask);
  return 0;
}