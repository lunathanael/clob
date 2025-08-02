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

#include "clob/Market.h"
#include "clob/types.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using marketmaker::CancelOrderRequest;
using marketmaker::CancelOrderResponse;
using marketmaker::GetOrderRequest;
using marketmaker::LimitOrderId;
using marketmaker::LimitOrderInfo;
using marketmaker::MarketMaker;
using marketmaker::BestBidAskResponse;
using marketmaker::PlaceOrderRequest;
using marketmaker::StockId;

namespace clob {

/**
 * @brief Implementation of the MarketMaker gRPC service
 * 
 * @details This class implements the MarketMaker service defined in the protobuf
 * and provides order placement, cancellation, status querying, and midprice calculation.
 */
class MarketMakerImpl final : public marketmaker::MarketMaker::Service {
public:
    /**
     * @brief Constructor for MarketMakerImpl
     * 
     * @param exchange_name The name of the exchange
     * @param exchange_ticker The ticker symbol of the exchange
     */
    explicit MarketMakerImpl(const std::string &exchange_name,
                             const std::string &exchange_ticker);

    /**
     * @brief Destructor
     */
    ~MarketMakerImpl() = default;

    /**
     * @brief Place a new order in the market
     * 
     * @param context gRPC server context
     * @param request The order placement request
     * @param response The response containing the order ID
     * @return Status of the operation
     */
    Status PlaceOrder(ServerContext *context, const PlaceOrderRequest *request,
                      LimitOrderId *response) override;

    /**
     * @brief Get the status of an existing order
     * 
     * @param context gRPC server context
     * @param request The order status request
     * @param response The response containing order information
     * @return Status of the operation
     */
    Status GetOrderStatus(ServerContext *context, const GetOrderRequest *request,
                          LimitOrderInfo *response) override;

    /**
     * @brief Cancel an existing order
     * 
     * @param context gRPC server context
     * @param request The order cancellation request
     * @param response The response indicating success/failure
     * @return Status of the operation
     */
    Status CancelOrder(ServerContext *context, const CancelOrderRequest *request,
                       CancelOrderResponse *response) override;

    /**
     * @brief Get the midprice for a stock
     * 
     * @param context gRPC server context
     * @param request The stock ID request
     * @param response The response containing the midprice
     * @return Status of the operation
     */
    Status QuoteBestBidAsk(ServerContext *context, const StockId *request,
                           BestBidAskResponse *response) override;

private:
    clob::Market market_;  ///< The underlying market engine
};

/**
 * @brief MarketMaker server class for managing the gRPC server
 * 
 * @details This class handles server lifecycle including starting, stopping,
 * and managing the MarketMaker service implementation.
 */
class MarketMakerServer {
public:
    /**
     * @brief Constructor for MarketMakerServer
     * 
     * @param exchange_name The name of the exchange
     * @param exchange_ticker The ticker symbol of the exchange
     * @param server_address The address to bind the server to (default: "0.0.0.0:50051")
     */
    explicit MarketMakerServer(const std::string &exchange_name,
                               const std::string &exchange_ticker,
                               const std::string &server_address = "0.0.0.0:50051");

    /**
     * @brief Destructor
     */
    ~MarketMakerServer();

    /**
     * @brief Start the server and wait for it to shutdown
     * 
     * @details This method blocks until the server is shut down.
     */
    void Run();

    /**
     * @brief Shutdown the server
     */
    void Shutdown();

private:
    std::unique_ptr<MarketMakerImpl> service_;     ///< The service implementation
    std::unique_ptr<grpc::Server> server_;         ///< The gRPC server instance
    std::string server_address_;                   ///< The server bind address
};

/**
 * @brief Run the market maker server with the given configuration
 * 
 * @param exchange_name The name of the exchange
 * @param exchange_ticker The ticker symbol of the exchange
 * @param server_address The address to bind the server to (default: "0.0.0.0:50051")
 * 
 * @details Creates and runs a MarketMakerServer instance. This function
 * blocks until the server is shut down.
 */
void RunServer(const std::string &exchange_name,
               const std::string &exchange_ticker,
               const std::string &server_address = "0.0.0.0:50051");

} // namespace clob