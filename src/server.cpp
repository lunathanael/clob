
/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#include "absl/flags/flag.h"
#include "absl/flags/parse.h" 
#include "absl/log/initialize.h"

#include "clob/MarketMaker.h"

ABSL_FLAG(std::string, exchange_name, "New York Stock Exchange", "The name of the exchange.");
ABSL_FLAG(std::string, exchange_ticker, "NYSE", "The ticker of the exchange.");
ABSL_FLAG(std::string, server_address, "0.0.0.0:50051", "The address of the server.");

int main(int argc, char **argv) {
    absl::ParseCommandLine(argc, argv);
    absl::InitializeLog();
    
    clob::RunServer(absl::GetFlag(FLAGS_exchange_name),
                    absl::GetFlag(FLAGS_exchange_ticker),
                    absl::GetFlag(FLAGS_server_address));
  
    return 0;
}