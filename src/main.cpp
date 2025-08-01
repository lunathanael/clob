/**
 * @page copyright
 * Copyright(c) 2025-present, Nathanael Lu.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#include <iostream>

#include "clob/Market.h"

using namespace std;

int main(int argc, char *argv[]) {

  if (argc != 3) {
    cout << "Usage: " << argv[0] << " <exchange_name> <exchange_ticker>"
         << endl;
    return 1;
  }

  const string exchange_name = argv[1];
  const string exchange_ticker = argv[2];

  clob::Market market(exchange_name, exchange_ticker);
}