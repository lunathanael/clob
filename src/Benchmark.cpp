#include <cmath>
#include <iostream>
#include <random>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "clob/CustomerClient.h"

double sampleOrderPrice(double best_price, clob::OrderType side,
                        double mean_log_price, double sigma_log_price,
                        std::mt19937 &rng) {

  std::lognormal_distribution<> lognorm_price(mean_log_price, sigma_log_price);
  double multiplier = lognorm_price(rng);

  double price = best_price * multiplier;
  return std::max(1.0, price);
}
double sampleOrderSize(double meanLog, double sigmaLog, std::mt19937 &rng) {
  static std::lognormal_distribution<> lognorm(meanLog, sigmaLog);
  return lognorm(rng);
}

static constexpr double lambda = 0.6;
static constexpr double mean_log_price = 0.0;
static constexpr double sigma_log_price = 0.3;
static constexpr double mean_log_qty = std::log(100);
static constexpr double sigma_log_qty = 0.5;

auto generate_random_order(double best_bid, double best_ask) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::bernoulli_distribution side_dist(0.5);
  clob::OrderType order_type =
      side_dist(gen) ? clob::OrderType::Bid : clob::OrderType::Ask;

  double best_price = order_type == clob::OrderType::Bid ? best_bid : best_ask;

  double price = sampleOrderPrice(best_price, order_type, mean_log_price,
                                  sigma_log_price, gen);
  double quantity =
      std::max(1.0, sampleOrderSize(mean_log_qty, sigma_log_qty, gen));

  return std::make_tuple(order_type, 0, static_cast<clob::price_t>(price),
                         static_cast<clob::quantity_t>(quantity));
}

int main(int argc, char **argv) {
  clob::CustomerClient customer(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));
  auto [best_bid, best_ask] = std::make_pair(100, 100);
  for (int i = 0; i < 10000; ++i) {
    auto [order_type, stock_id, price, quantity] =
        generate_random_order(best_bid, best_ask);
    customer.PlaceOrder(order_type, stock_id, price, quantity);
    auto [new_bid, new_ask] = customer.QuoteBestBidAsk(0);
    if(new_ask != 0)
        best_ask = new_ask;
    if(new_bid != 0)
        best_bid = new_bid;
    std::cout << best_bid << " " << best_ask << std::endl;
  }
  return 0;
}