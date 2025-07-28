/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#include <iostream>

#include "clob/Market.h"

int main() {
    clob::Market market("New York Stock Exchange", "NYSE");
    std::cout << market.get_exchange_name() << std::endl;
    std::cout << market.get_exchange_ticker() << std::endl;
    std::cout << market.get_num_stocks() << std::endl;
    market.add_stock("Apple", "AAPL");
    std::cout << market.get_num_stocks() << std::endl;
    market.add_stock("Microsoft", "MSFT");
    std::cout << market.get_num_stocks() << std::endl;
    market.add_stock("Google", "GOOGL");
    return 0;
}