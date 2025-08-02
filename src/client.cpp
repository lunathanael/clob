#include "clob/CustomerClient.h"
#include <absl/flags/parse.h>
#include <absl/log/initialize.h>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

void RunCustomerClient() {
  clob::CustomerClient customer(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));
  std::string command;
  while (std::getline(std::cin, command)) {
    customer.ParseCommand(command);
  }
}

int main(int argc, char **argv) {
  absl::ParseCommandLine(argc, argv);
  absl::InitializeLog();
  RunCustomerClient();
  return 0;
}