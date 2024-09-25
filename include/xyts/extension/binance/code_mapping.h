#pragma once

#include <string>

#include "xyts/core/contract_table.h"

namespace xyts::binance::futures {

std::string GetInstrBySymbol(const std::string& symbol);

ContractPtr GetContractBySymbol(const std::string& symbol);

}  // namespace xyts::binance::futures

namespace xyts::binance::spot {

std::string GetInstrBySymbol(const std::string& symbol);

ContractPtr GetContractBySymbol(const std::string& symbol);

}  // namespace xyts::binance::spot
