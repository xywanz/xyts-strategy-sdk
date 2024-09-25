#pragma once

#include <string>

#include "xyts/core/contract_table.h"
#include "xyts/core/log.h"

namespace xyts::binance::futures {

inline std::string GetInstrBySymbol(const std::string& symbol) {
  std::string instr;
  auto pos = symbol.find_first_of('_');
  if (pos == std::string::npos) {
    instr = fmt::format("FUT_BN_{}-999912", symbol);
  } else {
    auto expiry = symbol.substr(pos + 1);
    if (expiry.size() != 6) {
      return nullptr;
    }
    instr = fmt::format("FUT_BN_{}-20{}", symbol.substr(0, pos), expiry.substr(0, 4));
  }
  return instr;
}

inline ContractPtr GetContractBySymbol(const std::string& symbol) {
  auto instr = GetInstrBySymbol(symbol);
  if (instr.empty()) {
    return nullptr;
  }
  return ContractTable::GetByInstrument(instr);
}

}  // namespace xyts::binance::futures

namespace xyts::binance::spot {

inline std::string GetInstrBySymbol(const std::string& symbol) {
  return fmt::format("SPT_BN_{}", symbol);
}

inline ContractPtr GetContractBySymbol(const std::string& symbol) {
  auto instr = GetInstrBySymbol(symbol);
  return ContractTable::GetByInstrument(instr);
}

}  // namespace xyts::binance::spot
