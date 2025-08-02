/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#include "clob/MarketMaker.h"

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

using std::chrono::system_clock;

namespace clob {

MarketMakerImpl::MarketMakerImpl(const std::string &exchange_name,
                                 const std::string &exchange_ticker)
    : market_(exchange_name, exchange_ticker) {
  std::cout << "Market Maker Server initialized with exchange name: "
            << exchange_name << " and exchange ticker: " << exchange_ticker
            << std::endl;
  std::cout << "The current time is: "
            << std::chrono::system_clock::now().time_since_epoch().count()
            << std::endl;
  std::cout << "Adding stock: " << market_.add_stock("Apple", "APPL")
            << std::endl;
}

Status MarketMakerImpl::PlaceOrder(ServerContext *context,
                                   const PlaceOrderRequest *request,
                                   LimitOrderId *response) {
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

Status MarketMakerImpl::GetOrderStatus(ServerContext *context,
                                       const GetOrderRequest *request,
                                       LimitOrderInfo *response) {
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
    const auto order_ptr =
        market_.query_order<clob::OrderType::Bid>(request->order_id());
    if (order_ptr) {
      setter(*order_ptr);
    }
  } else {
    const auto order_ptr =
        market_.query_order<clob::OrderType::Ask>(request->order_id());
    if (order_ptr) {
      setter(*order_ptr);
    }
  }
  return Status::OK;
}

Status MarketMakerImpl::CancelOrder(ServerContext *context,
                                    const CancelOrderRequest *request,
                                    CancelOrderResponse *response) {
  if (request->order_type() == marketmaker::OrderType::Bid) {
    market_.cancel_order<clob::OrderType::Bid>(request->order_id());
  } else {
    market_.cancel_order<clob::OrderType::Ask>(request->order_id());
  }
  response->set_success(true);
  return Status::OK;
}

Status MarketMakerImpl::QuoteBestBidAsk(ServerContext *context,
                                        const StockId *request,
                                        BestBidAskResponse *response) {
  auto order_book = market_.get_order_book(request->stock_id());
  auto best_bid = order_book->get_best_order<clob::OrderType::Bid>();
  auto best_ask = order_book->get_best_order<clob::OrderType::Ask>();
  response->set_best_bid(best_bid ? best_bid->price : 0);
  response->set_best_ask(best_ask ? best_ask->price : 0);
  return Status::OK;
}

MarketMakerServer::MarketMakerServer(const std::string &exchange_name,
                                     const std::string &exchange_ticker,
                                     const std::string &server_address)
    : service_(
          std::make_unique<MarketMakerImpl>(exchange_name, exchange_ticker)),
      server_address_(server_address) {}

MarketMakerServer::~MarketMakerServer() {
  if (server_) {
    server_->Shutdown();
  }
}

void MarketMakerServer::Run() {
  ServerBuilder builder;

  builder.AddListeningPort(server_address_, grpc::InsecureServerCredentials());
  builder.RegisterService(service_.get());
  server_ = builder.BuildAndStart();
  std::cout << "Server listening on " << server_address_ << std::endl;

  server_->Wait();
}

void MarketMakerServer::Shutdown() {
  if (server_) {
    server_->Shutdown();
  }
}

void RunServer(const std::string &exchange_name,
               const std::string &exchange_ticker,
               const std::string &server_address) {
  MarketMakerServer server(exchange_name, exchange_ticker, server_address);
  server.Run();
}

} // namespace clob
