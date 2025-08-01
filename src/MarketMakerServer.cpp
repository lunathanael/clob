/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/log/initialize.h"
#include "market.grpc.pb.h"


#include "clob/Market.h"
#include "clob/types.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using marketmaker::CancelOrderResponse;
using marketmaker::CancelOrderRequest;
using marketmaker::GetOrderRequest;
using marketmaker::LimitOrderId;
using marketmaker::LimitOrderInfo;
using marketmaker::MarketMaker;
using marketmaker::PlaceOrderRequest;
using std::chrono::system_clock;

class MarketMakerImpl final : public MarketMaker::Service {
  clob::Market market_;

public:
  explicit MarketMakerImpl(const std::string &exchange_name,
                           const std::string &exchange_ticker)
      : market_(exchange_name, exchange_ticker) {
        std::cout << "Market Maker Server initialized with exchange name: " << exchange_name << " and exchange ticker: " << exchange_ticker << std::endl;
        std::cout << "The current time is: " << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;
      }

  Status PlaceOrder(ServerContext *context, const PlaceOrderRequest *request,
                    LimitOrderId *response) override {
    clob::LimitOrderId_t order_id;
    if (request->order_type() == marketmaker::OrderType::Bid) {
      order_id = market_.add_order<clob::OrderType::Bid>(
          request->stock_id(), request->price(), request->quantity());
    } else {
      order_id = market_.add_order<clob::OrderType::Ask>(
          request->stock_id(), request->price(), request->quantity());
    }
    response->set_order_id(order_id);
    return Status::OK;
  }

  Status GetOrderStatus(ServerContext *context, const GetOrderRequest *request,
                        LimitOrderInfo *response) override {
    auto setter = [&response](const auto &order) {
      response->set_order_id(order.id);
      response->set_timestamp(order.timestamp);
      response->set_balance(order.balance);
      response->set_price(order.price);
      response->set_quantity(order.quantity);
      response->set_filled_quantity(order.filled_quantity);
      response->set_is_cancelled(order.is_cancelled);
      response->set_order_found(true);
    };
    response->set_order_found(false);
    if (request->order_type() == marketmaker::OrderType::Bid) {
      const auto order_ptr = market_.query_order<clob::OrderType::Bid>(
          request->order_id());
      if (order_ptr) {
        setter(*order_ptr);
      }
    } else {
      const auto order_ptr = market_.query_order<clob::OrderType::Ask>(
          request->order_id());
      if (order_ptr) {
        setter(*order_ptr);
      }
    }
    return Status::OK;
  }

  Status CancelOrder(ServerContext *context, const CancelOrderRequest *request,
                     CancelOrderResponse *response) override {
    if (request->order_type() == marketmaker::OrderType::Bid) {
      market_.cancel_order<clob::OrderType::Bid>(request->order_id());
    } else {
      market_.cancel_order<clob::OrderType::Ask>(request->order_id());
    }
    response->set_success(true);
    return Status::OK;
  }
  
};

void RunServer(const std::string &exchange_name,
               const std::string &exchange_ticker) {
  std::string server_address("0.0.0.0:50051");
  MarketMakerImpl service(exchange_name, exchange_ticker);

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

ABSL_FLAG(std::string, exchange_name, "New York Stock Exchange", "The name of the exchange.");
ABSL_FLAG(std::string, exchange_ticker, "NYSE", "The ticker of the exchange.");

int main(int argc, char **argv) {
  absl::ParseCommandLine(argc, argv);
  absl::InitializeLog();
  RunServer(absl::GetFlag(FLAGS_exchange_name),
            absl::GetFlag(FLAGS_exchange_ticker));

  return 0;
}